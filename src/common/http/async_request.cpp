#include "async_request.hpp"
#include "common/http/sync_request.hpp"

namespace common::http {

AsyncRequest::AsyncRequest(boost::asio::io_context& ioc, boost::asio::ssl::context& ctx, const Callback callback,
                           const std::string& ip, const char* const httpPort, const int asyncOperTimeoutSeconds,
                           const int asyncWaitTimeoutSeconds)
    : SyncRequest(ioc, ctx, ip, httpPort) {
    SetCallback(callback);
}

AsyncRequest::AsyncRequest(boost::asio::io_context& ioc, boost::asio::ssl::context& ctx, const std::string& ip,
                           const char* const httpPort, const int asyncOperTimeoutSeconds,
                           const int asyncWaitTimeoutSeconds)
    : AsyncRequest(ioc, ctx, nullptr, ip, httpPort, asyncOperTimeoutSeconds, asyncWaitTimeoutSeconds) {
}

void AsyncRequest::CallbackResult(Result& er) {
    nowUse_ = true;
    if (UNLIKELY(callbackDisabled_)) {
        INFO("AsyncContext callbackDisabled");
    } else if (UNLIKELY(cb_)) {
        cb_(er);
    }
    nowUse_ = false;
}

bool AsyncRequest::Write() {
    if (UNLIKELY(!cb_)) {
        INFO("AsyncContext the callback should be set");
    } else if (!connected) {
        Connect();
    } else {
        boost::beast::http::async_write(
            *stream, request,
            [this](boost::beast::error_code ec, std::size_t bytesTransferred) { OnWrite(ec, bytesTransferred); });
    }
    return connected;
}

void AsyncRequest::Connect() {
    if (!SSL_set_tlsext_host_name((*stream).native_handle(), host.c_str())) {
        boost::beast::error_code ec{static_cast<int>(::ERR_get_error()), boost::asio::error::get_ssl_category()};
        INFO("AsyncContext SSL_set_tlsext_host_name error: " << ec.message().c_str());
        CallbackResult(lastResult(kConnectError, ec.message()));
    } else {
        // Set a timeout on the operation
        boost::beast::get_lowest_layer(*stream).expires_after(std::chrono::seconds(kAsyncOperTimeoutSeconds));

        resolver.async_resolve(
            host, port, [this](boost::beast::error_code ec, boost::asio::ip::tcp::resolver::results_type results) {
                OnResolve(ec, results);
            });
    }
}

void AsyncRequest::OnResolve(boost::beast::error_code ec, boost::asio::ip::tcp::resolver::results_type results) {
    if (ec) {
        INFO("AsyncContext can't resolve host: " << ec.message().c_str());
        CallbackResult(lastResult(kConnectError, ec.message()));
    } else {
        if (!ip.empty()) {
            INFO("AsyncContext bind socket to: " << ip.c_str());
            boost::system::error_code errorCode;
            boost::beast::get_lowest_layer(*stream).socket().open(boost::asio::ip::tcp::v4(), errorCode);
            if (errorCode) {
                INFO("AsyncContext: can't open socket: %s" << errorCode.message().c_str());
                CallbackResult(lastResult(kConnectError, errorCode.message()));
            }
            boost::asio::ip::tcp::endpoint ep(boost::asio::ip::address::from_string(ip), 0);
            boost::beast::get_lowest_layer(*stream).socket().bind(ep, errorCode);
            if (errorCode) {
                INFO("AsyncContext: can't bind socket: %s" << errorCode.message().c_str());
                CallbackResult(lastResult(kConnectError, errorCode.message()));
            }
        }

        boost::beast::get_lowest_layer(*stream).expires_after(std::chrono::seconds(kAsyncOperTimeoutSeconds));

        boost::beast::get_lowest_layer(*stream).async_connect(*(results.begin()),
                                                              [this](boost::beast::error_code ec) { OnConnect(ec); });
    }
}

void AsyncRequest::OnConnect(boost::beast::error_code ec) {
    if (ec) {
        INFO("AsyncContext: can't connect to the host: " << ec.message().c_str());
        CallbackResult(lastResult(kConnectError, ec.message()));
    } else {
        INFO("AsyncContext: connected");
        boost::beast::get_lowest_layer(*stream).expires_after(std::chrono::seconds(kAsyncOperTimeoutSeconds));

        (*stream).async_handshake(boost::asio::ssl::stream_base::client,
                                  [this](boost::beast::error_code ec) { OnHandshake(ec); });
    }
}

void AsyncRequest::OnHandshake(boost::beast::error_code ec) {
    if (ec) {
        INFO("AsyncContext: can't handshake with the host: " << ec.message().c_str());
        CallbackResult(lastResult(kConnectError, ec.message()));
    } else {
        connected = true;
        boost::beast::get_lowest_layer(*stream).socket().set_option(boost::asio::ip::tcp::no_delay(true));

        boost::beast::get_lowest_layer(*stream).expires_after(std::chrono::seconds(kAsyncOperTimeoutSeconds));

        boost::beast::http::async_write(
            *stream, request,
            [this](boost::beast::error_code ec, std::size_t bytesTransferred) { OnWrite(ec, bytesTransferred); });
    }
}

void AsyncRequest::OnWrite(boost::beast::error_code ec, size_t bytesTransferred) {
    if (ec) {
        INFO("AsyncContext: can't send request: " << ec.message().c_str());
        CallbackResult(lastResult(kWriteError, ec.message()));
    } else {
        response = boost::beast::http::response<boost::beast::http::string_body>();
        boost::beast::http::async_read(
            *stream, buffer, response,
            [this](boost::beast::error_code ec, std::size_t bytesTransferred) { OnRead(ec, bytesTransferred); });
    }
}

void AsyncRequest::OnRead(boost::beast::error_code ec, size_t bytesTransferred) {
    if (ec) {
        INFO("AsyncContext: can't read message: %s" << ec.message().c_str());
        CallbackResult(lastResult(kReadError, ec.message()));
    } else {
        boost::beast::get_lowest_layer(*stream).expires_after(std::chrono::seconds(kAsyncWaitTimeoutSeconds));
        CallbackResult(Response("CONN AsyncContext"));
    }
}

} // namespace common::http
