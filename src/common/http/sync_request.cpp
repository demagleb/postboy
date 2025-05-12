#include "sync_request.hpp"

#include <common/util/panic.hpp>

#include <boost/beast/http/status.hpp>
#include <boost/beast/version.hpp>

namespace common::http {

namespace {

namespace bst = boost::beast;

}

SyncRequest::SyncRequest(boost::asio::io_context& newIoc, boost::asio::ssl::context& newCtx, const std::string& newIp,
                         const char* const newPort)
    : ioc(newIoc)
    , ctx(newCtx)
    , ip(newIp)
    , port(newPort)
    , inited(false)
    , connected(false)
    , resolver(ioc)
    , stream(std::make_unique<bst::ssl_stream<bst::tcp_stream>>(ioc, ctx))
    , linger_(true, 0) {
}

SyncRequest::~SyncRequest() {
    response = bst::http::response<bst::http::string_body>();
    if (connected) {
        Shutdown();
    }
}

bool SyncRequest::Connect() {
    if (UNLIKELY(!SSL_set_tlsext_host_name((*stream).native_handle(), host.c_str()))) {
        bst::error_code err{static_cast<int>(::ERR_get_error()), boost::asio::error::get_ssl_category()};
        return false;
    }

    boost::system::error_code err;
    auto results = resolver.resolve(host, port, err);
    if (UNLIKELY(err)) {
        return false;
    }

    if (!ip.empty()) {
        bst::get_lowest_layer(*stream).socket();
        bst::get_lowest_layer(*stream).socket().open(boost::asio::ip::tcp::v4(), err);

        if (UNLIKELY(err)) {
            return false;
        }

        boost::asio::ip::tcp::endpoint ep(boost::asio::ip::address::from_string(ip), 0);
        bst::get_lowest_layer(*stream).socket().bind(ep, err);
        if (UNLIKELY(err)) {
            return false;
        }
    }

    bst::get_lowest_layer(*stream).connect(*(results.begin()), err);
    if (UNLIKELY(err)) {
        return false;
    }

    bst::get_lowest_layer(*stream).socket().set_option(linger_, err);
    if (UNLIKELY(err)) {
        return false;
    }

    (*stream).handshake(boost::asio::ssl::stream_base::client, err);
    if (UNLIKELY(err)) {
        return false;
    }

    connected = true;

    return true;
}

const Result& SyncRequest::Send(boost::system::error_code& err) {
    response = bst::http::response<bst::http::string_body>();

    if (UNLIKELY(!connected && !Connect())) {
        return lastResult(kConnectError, "Error during connection");
    }

    bst::http::write(*stream, request, err);
    if (UNLIKELY(err)) {
        Shutdown(err);
        return lastResult(kWriteError, "Error during write");
    }

    bst::http::read(*stream, buffer, response, err);
    if (UNLIKELY(err)) {
        Shutdown(err);
        return lastResult(kReadError, "Error during read");
    }

    return Response();
}

void SyncRequest::Shutdown(boost::system::error_code& err) {
    bst::get_lowest_layer(*stream).socket().shutdown(boost::asio::ip::tcp::socket::shutdown_both, err);
    if (UNLIKELY(err && err != bst::errc::not_connected)) {
        INFO("Unexpected error code");
    }
    bst::get_lowest_layer(*stream).socket().close();
    try {
        auto s = bst::get_lowest_layer(*stream).socket().release();
        struct linger l;
        l.l_onoff = 1;
        l.l_linger = 0;
        if (UNLIKELY(::setsockopt(s, SOL_SOCKET, SO_LINGER, &l, sizeof(l)) == -1)) {
            INFO("Get error" << int(errno));
        }
        if (UNLIKELY(::shutdown(s, 2) == -1)) {
            INFO("Get error" << int(errno));
        }
        if (UNLIKELY(::close(s))) {
            INFO("Get error" << int(errno));
        }
    } catch (const std::exception& e) {
        INFO("Get error" << e.what());
    }
    stream = std::make_unique<bst::ssl_stream<bst::tcp_stream>>(ioc, ctx);
    connected = false;
}

void SyncRequest::Shutdown() {
    if (!connected) {
        boost::system::error_code err;
        Shutdown(err);
    }
}

void SyncRequest::PrepareRequest(const std::string& preparedVerb, const std::string& preparedHost,
                                 const char* preparedContentType, const std::string& preparedBody,
                                 const std::string& preparedPath) {
    verb = preparedVerb;
    host = preparedHost;
    request.body() = preparedBody;
    request.prepare_payload();

    if (UNLIKELY(!inited)) {
        request.version(11);
        request.set(bst::http::field::host, host);
        if (preparedContentType != nullptr) {
            request.set(bst::http::field::content_type, preparedContentType);
        }
        request.set(bst::http::field::user_agent, BOOST_BEAST_VERSION_STRING);
        request.keep_alive();
        inited = true;
    }

    switch (verb[0]) {
        case 'P':
            if (LIKELY(verb[1] == 'O')) {
                request.method(bst::http::verb::post);
            } else {
                request.method(bst::http::verb::put);
            }
            break;
        case 'D':
            request.method(bst::http::verb::delete_);
            break;
        case 'G':
            request.method(bst::http::verb::get);
            break;
    }

    if (UNLIKELY(!preparedPath.empty())) {
        request.target(preparedPath);
    }
}

Result& SyncRequest::Response(const char*) const {
    if (UNLIKELY(response.result_int() == 0)) {
        std::string s(response.body());
        return lastResult(kZeroRespCode, "Zero response code");
    }

    for (const auto& field : response.base()) {
        lastResult.headers[field.name_string().data()] = field.value().data();
    }

    return lastResult(static_cast<int64_t>(response.result_int()), response.body());
}

std::unique_ptr<SyncRequest> CreateSyncRequest(boost::asio::io_context& ioc, boost::asio::ssl::context& ctx,
                                               const std::string& ip, const char* const port) {
    return std::make_unique<SyncRequest>(ioc, ctx, ip, port);
}

} // namespace common::http
