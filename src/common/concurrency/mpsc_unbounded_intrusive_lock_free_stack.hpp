#include <common/intrusive/forward_list.hpp>

#include <atomic>

namespace common::concurrency {

template <typename T>
using MPSCUnboundedIntrusiveLockFreeStackNode = intrusive::IntrusiveForwardListNode<T>;

template <typename T>
class MPSCUnboundedIntrusiveLockFreeStack {
    using Node = MPSCUnboundedIntrusiveLockFreeStackNode<T>;

public:
    void Push(Node* node) {
        auto* head = head_.load(std::memory_order_relaxed);

        while (true) {
            node->next_ = head;
            if (head_.compare_exchange_weak(head, node, std::memory_order_release)) {
                return;
            }
        }
    }

    intrusive::IntrusiveForwardList<T> TryPopAllReversed() {
        auto* head = TryPopAll();
        auto result = intrusive::IntrusiveForwardList<T>();
        while (head != nullptr) {
            auto* next = head->next_;
            result.PushFront(head);
            head = next;
        }
        return result;
    }

private:
    Node* TryPopAll() {
        return head_.exchange(nullptr, std::memory_order_acquire);
    }

private:
    std::atomic<Node*> head_{nullptr};
};

} // namespace common::concurrency
