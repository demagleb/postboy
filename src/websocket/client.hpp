#pragma once

#include <common/util/move_traits.hpp>

#include <ev.h>
#include <libwebsockets.h>
#include <sstream>

#include <cstdint>
#include <functional>
#include <queue>
#include <string_view>

namespace websocket {

class Client : common::util::MoveOnly {
public:
    enum class State : uint8_t { NotConnected, Connecting, Reconnecting, Connected, ClosingConnection };

    using OnReceiveCallback = std::function<void(std::string&& msg)>;
    using OnStateChangeCallback = std::function<void(State state, std::string&& reason)>;

public:
    Client(struct ev_loop* evLoop, OnReceiveCallback&& onReceiveCallback,
           OnStateChangeCallback&& onStateChangeCallback);

    ~Client();

    void Connect(std::string_view host, std::string_view path, int port);

    void Send(std::string_view message);

    void Close() {
        Close("Intended close");
    }

    void Finish();

private:
    static int OnEvent(struct lws* wsi, enum lws_callback_reasons reason, void* user, void* in, size_t len) {
        auto* websocketClient = reinterpret_cast<Client*>(user);
        return websocketClient->OnEventImpl(wsi, reason, in, len);
    }

    static void ConnectImpl(lws_sorted_usec_list_t* sul);

    int OnEventImpl(struct lws* wsi, enum lws_callback_reasons reason, void* in, size_t len);

    void ChangeState(State state, std::string&& reason);

    void Close(std::string&& reason);

    void ResetContext();

private:
    /** struct lws_protocols -	List of protocols and handlers client or server supports. */
    static constexpr auto kProtocolName = "ws";
    static constexpr struct lws_protocols kProtocols[] = {
        {kProtocolName, &Client::OnEvent, 0, 65536, 0, nullptr, 0},
        {nullptr, nullptr, 0, 0, 0, nullptr, 0},
    };

    // https://github.com/warmcat/libwebsockets/blob/main/minimal-examples/client/binance/main.c#L36
    static constexpr struct lws_extension kExtensions[] = {
        {"permessage-deflate", lws_extension_callback_pm_deflate,
         "permessage-deflate; client_no_context_takeover; client_max_window_bits"},
        {nullptr, nullptr, nullptr /* terminator */}};

    // https://github.com/warmcat/libwebsockets/blob/main/minimal-examples-lowlevel/ws-client/minimal-ws-client/minimal-ws-client.c#L37
    static constexpr uint32_t kBackoffMs[] = {1000, 2000, 3000, 4000, 5000};

    // https://github.com/warmcat/libwebsockets/blob/main/minimal-examples-lowlevel/ws-client/minimal-ws-client/minimal-ws-client.c#L39
    static constexpr lws_retry_bo_t kRetry = {
        .retry_ms_table = kBackoffMs,
        .retry_ms_table_count = LWS_ARRAY_SIZE(kBackoffMs),
        .conceal_count = LWS_ARRAY_SIZE(kBackoffMs),

        .secs_since_valid_ping = 3,    /* force PINGs after secs idle */
        .secs_since_valid_hangup = 10, /* hangup after secs idle */

        .jitter_percent = 20,
    };

private:
    struct ev_loop* evLoop_;

    OnReceiveCallback onReceive_{nullptr};
    OnStateChangeCallback onStateChange_{nullptr};
    State state_{State::NotConnected};
    std::queue<std::string> msgsToSend_;
    std::string buffer_;

    lws_sorted_usec_list_t sul_{};
    lws_client_connect_info connectInfo_{};
    uint16_t retryCount_{0};
    struct lws* wsi_{nullptr};
};

} // namespace websocket
