#pragma once

#include <common/concurrency/spsc_queue.hpp>
#include <common/util/types.hpp>

#include <array>
#include <mutex>
#include <string>
#include <vector>

namespace postboy::notifier {

/* Represents a singletone used to send messages from multiple threads to single thread */

/* Should be used with single reader for best performance */
class MessageNotifier {
private:
    static constexpr int kSpscQueueSize = 1024;
    static constexpr int kResultBufferSize = 1024;
    static constexpr int kMessageStorageSize = 4096;

public:
    using Iterator = std::array<std::string, kResultBufferSize>::const_iterator;

    static void Send(std::string&& message);

    static std::pair<Iterator, Iterator> Handle();

private:
    MessageNotifier();

    ~MessageNotifier();

    void SendImpl(std::string&& message);

private:
    /* protects all queues */
    static std::mutex lock;

    /* per-thread message notifier */
    static thread_local MessageNotifier notifier;

    /* list of all active notifiers */
    static std::vector<MessageNotifier*> allNotifiers;

    /* result message buffer */
    static std::array<std::string, kResultBufferSize> resultBuffer;

    /* queue buffer */
    common::concurrency::SpscQueue<std::string*, kSpscQueueSize> spscQueue_;
    std::array<std::string, kMessageStorageSize> messageStorage_;
    int messageStorageIdx_{0};
};

} // namespace postboy::notifier
