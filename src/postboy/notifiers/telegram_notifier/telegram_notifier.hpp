#pragma once

#include <config/proto/telegram_notifier.pb.h>
#include <string>

#include <boost/asio.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ssl/context.hpp>

namespace postboy::notifier {

class TelegramNotifier {
public:
    explicit TelegramNotifier(config::TelegramNotifier config);

    void SendMessage(const std::string& text);

private:
    const std::string url_;
    const std::string token_;
    const std::string chatId_;
    const std::string sendMessageRoute_;

    boost::asio::io_context ioc_;
    boost::asio::ssl::context ctx_{boost::asio::ssl::context::sslv23_client};
};

void ProcessTelegramNotifier(config::TelegramNotifier config);

} // namespace postboy::notifier