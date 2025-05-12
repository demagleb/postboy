#include "client.hpp"

#include <common/concurrency/spinlock.hpp>
#include <common/util/panic.hpp>
#include <common/util/types.hpp>

#include <common/util/assert.hpp>

#include <ev.h>

#include <mutex>
#include <unordered_map>

using namespace common::util;

namespace websocket {

namespace {

struct Context {
    struct lws_context* context{nullptr};
    int64_t clientsCount{0};
};

std::unordered_map<struct ev_loop*, Context> contexts;
common::concurrency::Spinlock lock;

FORCE_INLINE auto TryEmplaceContext(struct ev_loop* evLoop) noexcept {
    std::lock_guard guard(lock);
    return contexts.try_emplace(evLoop);
}

FORCE_INLINE Context& GetContext(struct ev_loop* evLoop) noexcept {
    ASSERT(contexts.contains(evLoop), "websocket::(anonymous namespace)::contexts does not contain event loop");
    return contexts.at(evLoop);
}

} // namespace

Client::Client(struct ev_loop* evLoop, OnReceiveCallback&& onReceive, OnStateChangeCallback&& onStateChange)
    : evLoop_(evLoop)
    , onReceive_(std::move(onReceive))
    , onStateChange_(std::move(onStateChange)) {
}

void Client::Connect(std::string_view host, std::string_view path, int port) {
    INFO("Prepare to connecting to websocket server with host=[" << host << "], path=[" << path << "], port=[" << port
                                                                 << "]");

    ENSURE(state_ == State::NotConnected, "already connected");

    auto [contextIt, inserted] = TryEmplaceContext(evLoop_);
    auto& context = contextIt->second;
    context.clientsCount++;

    void* loops[1] = {evLoop_};

    if (inserted) {
        const lws_context_creation_info creationInfo = {.protocols = kProtocols,
                                                        .extensions = kExtensions,
                                                        .port = CONTEXT_PORT_NO_LISTEN_SERVER,
                                                        .options = LWS_SERVER_OPTION_LIBEV |
                                                                   LWS_SERVER_OPTION_DO_SSL_GLOBAL_INIT,
                                                        .fd_limit_per_thread = 128,
                                                        .foreign_loops = loops};

        ENSURE((context.context = lws_create_context(&creationInfo)) != nullptr, "failed to initialize context");
    }

    connectInfo_ = {.context = context.context,
                    .address = host.data(),
                    .port = port,
                    .ssl_connection = LCCSCF_USE_SSL | LCCSCF_PRIORITIZE_READS,
                    .path = path.data(),
                    .host = host.data(),
                    .protocol = nullptr,
                    .userdata = this,
                    .pwsi = &wsi_,
                    .local_protocol_name = kProtocolName,
                    .retry_and_idle_policy = &kRetry};

    lws_sul_schedule(context.context, 0, &sul_, Client::ConnectImpl, 1);
    ChangeState(State::Connecting, "Intended connect");
}

void Client::ConnectImpl(lws_sorted_usec_list_t* sul) {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Winvalid-offsetof"
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
    auto* websocketClient = lws_container_of(sul, Client, sul_);
#pragma GCC diagnostic pop
    INFO("Try to connecting...");

    if (lws_client_connect_via_info(&websocketClient->connectInfo_) == nullptr) {
        /*
         * Failed... schedule a retry... we can't use the _retry_wsi()
         * convenience wrapper api here because no valid wsi at this
         * point.
         */
        auto& context = GetContext(websocketClient->evLoop_);
        if (lws_retry_sul_schedule(context.context, 0, sul, &kRetry, Client::ConnectImpl,
                                   &websocketClient->retryCount_) != 0) {
            websocketClient->Close("Connection attempts exhausted");
        }
    }
}

int Client::OnEventImpl(struct lws* wsi, enum lws_callback_reasons reason, void* in, size_t len) {
    switch (reason) {
        case LWS_CALLBACK_CLIENT_CLOSED:
        case LWS_CALLBACK_CLIENT_CONNECTION_ERROR:
            ConnectImpl(&sul_);
            break;

        case LWS_CALLBACK_CLIENT_RECEIVE:
            buffer_ += std::string(reinterpret_cast<const char*>(in), len);
            if (lws_is_final_fragment(wsi) != 0) {
                onReceive_(std::string(buffer_));
                buffer_.clear();
            }
            break;

        case LWS_CALLBACK_CLIENT_ESTABLISHED:
            if (state_ < State::Connected) {
                ChangeState(State::Connected, "Connection is established");
            }
            break;

        case LWS_CALLBACK_CLIENT_WRITEABLE:
            // Disconnect client
            if (state_ == State::ClosingConnection) {
                return -1;
            }

            if (!msgsToSend_.empty()) {
                const int len = msgsToSend_.front().size() - LWS_PRE;
                auto* bytes = (unsigned char*)&(msgsToSend_.front()[LWS_PRE]);
                const int written = lws_write(wsi_, bytes, len, LWS_WRITE_TEXT);
                msgsToSend_.pop();
                if (written < len) {
                    Close("Msg send failed");
                    return -1;
                }
                if (!msgsToSend_.empty()) {
                    lws_callback_on_writable(wsi);
                }
            }
            break;

        default:
            break;
    }

    return lws_callback_http_dummy(wsi, reason, this, in, len);
}

void Client::Send(std::string_view message) {
    const auto callCallback = msgsToSend_.empty();
    msgsToSend_.push(message.data());
    if (callCallback) {
        lws_callback_on_writable(wsi_);
    }
}

void Client::Finish() {
    if (state_ == State::NotConnected) {
        return;
    }

    ChangeState(State::NotConnected, "Intended finish");
    ResetContext();
}

Client::~Client() {
    if (state_ == State::NotConnected) {
        return;
    }

    // log

    ResetContext();
}

void Client::ChangeState(Client::State state, std::string&& reason) {
    state_ = state;
    onStateChange_(state, std::move(reason));
}

void Client::ResetContext() {
    auto& context = GetContext(evLoop_);
    ASSERT(context.clientsCount > 0, "Context::clientsCount [" << context.clientsCount << "] <= 0");
    if (--context.clientsCount <= 0) {
        if (context.context != nullptr) {
            lws_context_destroy(context.context);
            context.context = nullptr;
        }
    }
}

void Client::Close(std::string&& reason) {
    if (state_ == State::ClosingConnection || state_ == State::NotConnected) {
        return;
    }

    ChangeState(State::ClosingConnection, std::move(reason));
    lws_callback_on_writable(wsi_);
}

} // namespace websocket
