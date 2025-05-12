#pragma once

#include <common/concurrency/pointer_swap.hpp>
#include <common/util/panic.hpp>
#include <common/util/types.hpp>

#include <limits>
#include <thread>
#include <functional>

namespace common::http {

class IConnection {
public:
    virtual ~IConnection() {
    }

protected:
    friend class ConnectionPull;

    bool nowUse_{false};
};

class ConnectionPull {
public:
    class Connection {
    public:
        IConnection* GetConn() {
            return conn_;
        }

        ~Connection() {
            if (conn_ != nullptr) {
                INFO("Delete connection");
                delete conn_;
            }
        }

        friend class ConnectionPull;

    private:
        explicit Connection(IConnection* conn, Connection* nextConn = nullptr)
            : conn_(conn)
            , lastActiveTime_(time(nullptr))
            , nextConn_(nextConn) {
        }

    private:
        IConnection* conn_;
        time_t lastActiveTime_;
        Connection* nextConn_;
    };

    using AtomicConnPtr = std::atomic<Connection*>;
    using CreateConnection = std::function<IConnection*()>;
    using Connect = std::function<bool(IConnection*)>;
    using Disconnect = std::function<void(IConnection*&)>;
    using Ping = std::function<bool(IConnection*)>;

    ConnectionPull(CreateConnection createConnection, Connect connect, Disconnect disconnect,
                   int initConnectionsCnt = 0, int maxConnectionsCnt = std::numeric_limits<int>::max());

    ~ConnectionPull();

    Connection* Pop();
    void Push(Connection*, bool forceReconnect = false);

    void StartPinger(Ping ping, uint32_t pingPeriod);

private:
    void PingerRoutine();

    FORCE_INLINE void Push(AtomicConnPtr& atomicPtr, Connection* ptr) {
        ptr->nextConn_ = atomicPtr.load(std::memory_order_relaxed);
        while (!atomicPtr.compare_exchange_weak(ptr->nextConn_, ptr, std::memory_order_release,
                                                std::memory_order_relaxed)) {
        }
    }

    FORCE_INLINE Connection* Pop(AtomicConnPtr& atomicPtr) {
        Connection* ptr = atomicPtr.load(std::memory_order_relaxed);
        while (ptr != nullptr && !atomicPtr.compare_exchange_weak(ptr, ptr->nextConn_, std::memory_order_release,
                                                                  std::memory_order_relaxed)) {
        }
        return ptr;
    }

private:
    CreateConnection createConnection_;
    Connect connect_;
    Disconnect disconnect_;
    Ping ping_;

    std::atomic<int> connectionsCnt_;
    const int maxConnectionsCnt_;
    uint32_t pingPeriod_;

    bool nowRun_{true};
    std::unique_ptr<std::thread> pinger_;

    AtomicConnPtr stack_[2]{nullptr, nullptr};
    concurrency::AtomicPointers<AtomicConnPtr> stackPtrs_;
};

} // namespace common::http
