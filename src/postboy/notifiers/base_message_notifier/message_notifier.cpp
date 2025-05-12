#include "message_notifier.hpp"

namespace postboy::notifier {

void MessageNotifier::Send(std::string&& message) {
    notifier.SendImpl(std::move(message));
}

std::pair<MessageNotifier::Iterator, MessageNotifier::Iterator> MessageNotifier::Handle() {
    std::lock_guard<std::mutex> g(lock);
    int idx = 0;
    for (auto& notifier : allNotifiers) {
        while (true) {
            const auto message = notifier->spscQueue_.TryPop().value_or(nullptr);
            if (message != nullptr) {
                resultBuffer[idx++] = *message;
            }
            if (idx == kResultBufferSize || message == nullptr) {
                break;
            }
        }
        if (UNLIKELY(idx == kResultBufferSize)) {
            break;
        }
    }
    return std::make_pair(resultBuffer.begin(), resultBuffer.begin() + idx);
}

MessageNotifier::MessageNotifier() {
    std::lock_guard<std::mutex> g(lock);
    allNotifiers.push_back(this);
}

MessageNotifier::~MessageNotifier() {
    std::lock_guard<std::mutex> g(lock);
    allNotifiers.erase(std::remove(allNotifiers.begin(), allNotifiers.end(), this), allNotifiers.end());
}

void MessageNotifier::SendImpl(std::string&& message) {
    messageStorage_[messageStorageIdx_] = std::move(message);
    spscQueue_.Push(&messageStorage_[messageStorageIdx_]);
    messageStorageIdx_ = (messageStorageIdx_ + 1) % (kMessageStorageSize - 1);
}

std::mutex MessageNotifier::lock;
std::vector<MessageNotifier*> MessageNotifier::allNotifiers;
std::array<std::string, 1024> MessageNotifier::resultBuffer;
thread_local MessageNotifier MessageNotifier::notifier;

} // namespace postboy::notifier
