#include "telegram_notifier.hpp"

#include <chrono>
#include <ostream>
#include <postboy/notifiers/base_message_notifier/message_notifier.hpp>

#include <common/http/sync_request.hpp>

#include <fmt/format.h>

#include <sstream>
#include <thread>

namespace postboy::notifier {

namespace {

using namespace fmt::literals;

} // namespace

TelegramNotifier::TelegramNotifier(config::TelegramNotifier config)
    : url_(config.url())
    , token_(config.token())
    , chatId_(config.chatid())
    , sendMessageRoute_(config.sendmessageroute()) {
}

void TelegramNotifier::SendMessage(const std::string& text) {
    std::stringstream ss;

    auto body = fmt::format("{{\"chat_id\": \"{chat_id}\", \"text\": \"{text}\", \"parse_mode\": \"MarkdownV2\"}}",

                            "chat_id"_a = chatId_, "text"_a = text);

    auto request = common::http::CreateSyncRequest(ioc_, ctx_, std::string{});

    request->PrepareRequest({.verb = "POST",
                             .host = url_,
                             .content_type = nullptr,
                             .body = body,
                             .path = "/bot" + token_ + "/" + sendMessageRoute_});
    request->SetRequestHeader("Content-Type", "application/json");

    boost::beast::error_code err;
    auto result = request->Send(err);

    if (UNLIKELY(result.code != 200)) {
        WARN("[TelegramNotifier] send message failed code != 200, body = " << result.body);
        return;
    }

    INFO("[TelegramNotifier] send message successfully");
}

void ProcessTelegramNotifier(config::TelegramNotifier config) {
    TelegramNotifier telegramNotifier(config);
    while (true) {
        auto [first, last] = MessageNotifier::Handle();
        while (first != last) {
            telegramNotifier.SendMessage(*first);
            ++first;
        }
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
}

} // namespace postboy::notifier