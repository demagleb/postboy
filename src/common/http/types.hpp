#pragma once

#include <common/util/panic.hpp>
#include <common/util/types.hpp>

#include <sstream>
#include <string>
#include <unordered_map>

namespace common::http {

struct RequestParams {
    std::string verb;
    std::string host;
    const char* content_type;
    std::string body;
    std::string path;
};

struct Result {
    Result& operator()(const int64_t otherCode, const std::string& otherBody) {
        code = otherCode;
        body = otherBody;
        return *this;
    }

    std::string Debug() const {
        std::stringstream ss;
        ss << "code: " << code << ", body: " << body;
        return ss.str();
    }

    FORCE_INLINE void CheckHeader(const std::string& header) {
        auto it = headers.find(header);
        if (it != headers.end()) {
            INFO("Contain header : " << it->second);
        }
    }

    int64_t code{0};
    std::string body;
    std::unordered_map<std::string, std::string> headers;
};

} // namespace common::http
