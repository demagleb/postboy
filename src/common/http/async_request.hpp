#pragma once

#include "sync_request.hpp"

namespace common::http {

class AsyncRequest : public SyncRequest {
public:
    using Callback = std::function<void(Result&)>;

    AsyncRequest(boost::asio::io_context& ioc, boost::asio::ssl::context& ctx, const Callback callback,
                 const std::string& ip, const char* const httpPort = "443",
                 const int asyncOperTimeoutSeconds = kAsyncOperTimeoutSeconds,
                 const int asyncWaitTimeoutSeconds = kAsyncWaitTimeoutSeconds);

    AsyncRequest(boost::asio::io_context& ioc, boost::asio::ssl::context& ctx, const std::string& ip,
                 const char* const httpPort = "443", const int asyncOperTimeoutSeconds = kAsyncOperTimeoutSeconds,
                 const int asyncWaitTimeoutSeconds = kAsyncWaitTimeoutSeconds);

    template <class... Args>
    static std::unique_ptr<AsyncRequest> Create(Args&&... args) {
        return std::make_unique<AsyncRequest>(std::forward<Args>(args)...);
    }

    FORCE_INLINE void SetCallback(Callback cb) {
        cb_ = cb;
        callbackDisabled_ = false;
    }

    FORCE_INLINE void ResetCallback() {
        callbackDisabled_ = true;
    }

    void CallbackResult(Result& er);

    bool Write();

    void Connect();

    void OnResolve(boost::beast::error_code ec, boost::asio::ip::tcp::resolver::results_type results);

    void OnConnect(boost::beast::error_code ec);

    void OnHandshake(boost::beast::error_code ec);

    void OnWrite(boost::beast::error_code ec, size_t bytesTransferred);

    void OnRead(boost::beast::error_code ec, size_t bytesTransferred);

private:
    static constexpr int kAsyncOperTimeoutSeconds = 10;
    static constexpr int kAsyncWaitTimeoutSeconds = 60;

    Callback cb_;
    bool callbackDisabled_{true};
};

} // namespace common::http
