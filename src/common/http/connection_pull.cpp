#include "connection_pull.hpp"

#include <common/util/assert.hpp>

namespace common::http {

ConnectionPull::ConnectionPull(CreateConnection createConnection, Connect connect, Disconnect disconnect,
                               int initConnectionsCnt, int maxConnectionsCnt)
    : createConnection_(createConnection)
    , connect_(connect)
    , disconnect_(disconnect)
    , connectionsCnt_(initConnectionsCnt)
    , maxConnectionsCnt_(maxConnectionsCnt)
    , stackPtrs_(&stack_[0], &stack_[1]) {
    VERIFY(createConnection, "ConnectionPull createConnection function does not set");
    VERIFY(maxConnectionsCnt > 0, "ConnectionPull maxConnectionsCnt should be > 0");
    VERIFY(maxConnectionsCnt >= initConnectionsCnt, "ConnectionPull maxConnectionsCnt should be >= initConnectionsCnt");

    INFO("ConnectionPull init");

    for (uint32_t c = 0; c < connectionsCnt_; ++c) {
        auto connection = createConnection();
        connect(connection);
        stack_[0] = new Connection{connection, stack_[0]};

        INFO("ConnectionPull create connection");
    }
}

ConnectionPull::~ConnectionPull() {
    INFO("ConnectionPull destroy");

    nowRun_ = false;
    if (pinger_ != nullptr && pinger_->joinable()) {
        pinger_->join();
    }

    while (Connection* connection = Pop(*stackPtrs_.First())) {
        delete connection;
    }

    while (Connection* connection = Pop(*stackPtrs_.Second())) {
        delete connection;
    }
}

void ConnectionPull::StartPinger(Ping ping, uint32_t pingPeriod) {
    VERIFY(pingPeriod > 0, "ConnectionPull pingPeriod should be > 0");
    pingPeriod_ = pingPeriod;
    ping_ = ping;
    pinger_ = std::make_unique<std::thread>([this] { PingerRoutine(); });
}

void ConnectionPull::PingerRoutine() {
    for (time_t tp = time(nullptr) + pingPeriod_; nowRun_; tp += pingPeriod_) {
        while (time(nullptr) < tp && nowRun_) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));

            if (stackPtrs_.First()->load(std::memory_order_relaxed) == nullptr) {
                stackPtrs_.Exchange();
            }
        }

        while (Connection* connection = Pop(*stackPtrs_.First())) {
            if (!nowRun_) {
                Push(*stackPtrs_.Second(), connection);
                return;
            }

            if (time(nullptr) >= connection->lastActiveTime_ + pingPeriod_) {
                if (!ping_(connection->conn_) && nowRun_) {
                    INFO("ConnectionPull Ping error");
                    Push(connection, true); // force reconnect
                } else {
                    Push(connection);
                }
            } else {
                Push(*stackPtrs_.Second(), connection);
            }
        }

        stackPtrs_.Exchange();
    }
}

ConnectionPull::Connection* ConnectionPull::Pop() {
    for (int i = 0;; std::this_thread::sleep_for(std::chrono::seconds(0))) {
        Connection* connection = Pop(*stackPtrs_.First());
        if (connection != nullptr) {
            INFO("ConnectionPull pop connection from firstPtr");
            return connection;
        }

        connection = Pop(*stackPtrs_.Second());
        if (connection != nullptr) {
            INFO("ConnectionPull Pop connection from secondPtr");
            return connection;
        }

        if (UNLIKELY(i++ == 2)) {
            break;
        }
    }

    for (auto currentConnectionsAmt = connectionsCnt_.load(std::memory_order_relaxed);
         UNLIKELY(currentConnectionsAmt < maxConnectionsCnt_);) {
        if (connectionsCnt_.compare_exchange_strong(currentConnectionsAmt, currentConnectionsAmt + 1)) {
            Connection* connection = new Connection{createConnection_()};

            WARN("ConnectionPull create new connection, connectionsCnt " << currentConnectionsAmt + 1);
            return connection;
        }
    }

    WARN("ConnectionPull reached maxim count of connections " << maxConnectionsCnt_);
    return nullptr;
}

void ConnectionPull::Push(Connection* connection, bool forceReconnect) {
    INFO("ConnectionPull push connection");

    if (forceReconnect) {
        std::thread{[this, connection]() { // Should be replaced on fiber
            while (UNLIKELY(connection->conn_->nowUse_)) {
                std::this_thread::sleep_for(std::chrono::milliseconds(0));
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(0));
            disconnect_(connection->conn_);

            INFO("Connection disconnected");
            if (UNLIKELY(!connect_(connection->conn_))) {
                WARN("ConnectionPull failed to connect");
                return Push(connection, true);
            }
            INFO("ConnectionPull connection connected");

            connection->lastActiveTime_ = time(nullptr);
            Push(*stackPtrs_.Second(), connection);
        }}
            .detach();
    } else {
        // maybe check and wait on nowUse of conn?
        connection->lastActiveTime_ = time(nullptr);
        Push(*stackPtrs_.Second(), connection);
    }
}

} // namespace common::http
