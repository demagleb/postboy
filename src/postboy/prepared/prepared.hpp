#pragma once

#include <config/proto/postboy.pb.h>

namespace postboy {

class Prepared {
public:
    using Config = google::protobuf::RepeatedPtrField<config::Postboy>;

public:
    explicit Prepared(const Config& config);

    void Run();

    ~Prepared();

private:
    class PreparedItem;

    static std::vector<std::unique_ptr<PreparedItem>> CollectItems(const Config& config);

private:
    std::vector<std::unique_ptr<PreparedItem>> preparedItems_;
};

} // namespace postboy
