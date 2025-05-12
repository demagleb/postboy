#pragma once

#include "connection_pull.hpp"

#include <common/util/types.hpp>
#include <common/http/types.hpp>

#include <boost/asio/io_service.hpp>
#include <boost/asio/ssl/context.hpp>

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/http/message.hpp>

#include <memory>

#include <limits.h>

namespace common::http {

class SyncRequest : public IConnection {
public:
    SyncRequest(boost::asio::io_context& newIoc, boost::asio::ssl::context& newCtx, const std::string& newIp,
                const char* const newPort = "443");

    ~SyncRequest();

    static std::unique_ptr<SyncRequest> Create(boost::asio::io_context& ioc, boost::asio::ssl::context& ctx,
                                               const std::string& ip, const char* const port = "443");

    bool Connect();
    const Result& Send(boost::system::error_code& ec);
    void Shutdown(boost::system::error_code& ec);
    void Shutdown();

    template <class Field>
    FORCE_INLINE void SetRequestHeader(const Field& field, const std::string& value) {
        request.set(field, value);
    }

    FORCE_INLINE void SetTarget(const std::string& target) {
        request.target(target);
    }

    FORCE_INLINE void SetBody(const std::string& body) {
        request.body() = body;
        request.prepare_payload();
    }

    FORCE_INLINE void PrepareRequest(const RequestParams& params) {
        PrepareRequest(params.verb, params.host, params.content_type, params.body, params.path);
    }

    FORCE_INLINE const auto& GetResponseHeader() const {
        return response.base();
    }

    FORCE_INLINE const auto& GetResponseBody() const {
        return response.body();
    }

    FORCE_INLINE bool Connected() const {
        return connected;
    }

    FORCE_INLINE void SetHttpHost(std::string otherHost) {
        host = otherHost;
    }

    boost::asio::io_context& ioc;
    boost::asio::ssl::context& ctx;

    std::string verb;
    std::string host;
    const std::string ip;
    const char* const port;

    bool inited;
    bool connected;

    boost::asio::ip::tcp::resolver resolver;
    std::unique_ptr<boost::beast::ssl_stream<boost::beast::tcp_stream>> stream;
    boost::beast::flat_buffer buffer;
    boost::beast::http::request<boost::beast::http::string_body> request;
    boost::beast::http::response<boost::beast::http::string_body> response;

protected:
    Result& Response(const char* caller = "SyncRequest") const;

    mutable Result lastResult;

private:
    void PrepareRequest(const std::string& preparedVerb, const std::string& preparedHost,
                        const char* preparedContentType, const std::string& preparedBody = "",
                        const std::string& preparedPath = "");

protected:
    static constexpr int64_t kConnectError = std::numeric_limits<int64_t>::min() + 0;
    static constexpr int64_t kWriteError = std::numeric_limits<int64_t>::min() + 1;
    static constexpr int64_t kReadError = std::numeric_limits<int64_t>::min() + 2;
    static constexpr int64_t kZeroRespCode = std::numeric_limits<int64_t>::min() + 3;

private:
    boost::asio::socket_base::linger linger_;
};

std::unique_ptr<SyncRequest> CreateSyncRequest(boost::asio::io_context& ioc, boost::asio::ssl::context& ctx,
                                               const std::string& ip, const char* const port = "443");

} // namespace common::http
