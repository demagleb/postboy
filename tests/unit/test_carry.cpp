#include <gtest/gtest.h>

#include <carry/context.hpp>
#include <carry/empty.hpp>
#include <carry/wrap.hpp>
#include <carry/new.hpp>
#include <carry/mutable.hpp>
#include <carry/current.hpp>
#include <carry/scope.hpp>

#include <carry/carrier.hpp>

#include <compass/map.hpp>

TEST(Carry, Entries) {
    auto ctx = carry::New().SetInt64("test", 123).SetString("id", "12345").Done();

    ASSERT_EQ(ctx.Get<int64_t>("test"), 123);
    ASSERT_EQ(ctx.Get<std::string>("id"), "12345");

    ASSERT_FALSE(ctx.TryGet<int64_t>("missing"));

    ASSERT_EQ(ctx.GetOr<std::string>("missing", "or"), "or");
}

TEST(Carry, DefaultCtor) {
    carry::Context ctx;
    ASSERT_FALSE(ctx.TryGet<std::string>("key"));
}

TEST(Carry, Copy) {
    auto ctx1 = carry::New().SetInt64("test", 123).Done();

    auto ctx2 = ctx1;
    ASSERT_EQ(ctx2.Get<int64_t>("test"), 123);
}

TEST(Carry, Wrap) {
    auto ctx1 = carry::New().SetInt64("test", 123).Done();

    auto ctx2 = carry::Wrap(ctx1).SetInt64("test", 456).SetInt64("y", 7).Done();

    ASSERT_EQ(ctx1.Get<int64_t>("test"), 123);
    ASSERT_EQ(ctx2.Get<int64_t>("test"), 456);
    ASSERT_EQ(ctx2.Get<int64_t>("y"), 7);
    ASSERT_FALSE(ctx1.TryGet<int64_t>("y"));
}

TEST(Carry, WrapImplicitDone) {
    auto ctx1 = carry::Empty();

    carry::Context ctx2 = carry::Wrap(ctx1).SetString("key", "test");

    ASSERT_EQ(ctx2.GetString("key"), "test");
}

TEST(Carry, Wrapper) {
    auto ctx1 = carry::New().SetInt64("test", 123).Done();

    carry::Wrapper wrapper(ctx1);
    wrapper.SetInt64("test", 456);
    wrapper.SetInt64("port", 80).SetString("name", "test");

    auto ctx2 = wrapper.Done();

    ASSERT_EQ(ctx2.GetInt64("test"), 456);
    ASSERT_EQ(ctx2.GetInt64("port"), 80);
    ASSERT_EQ(ctx2.GetString("name"), "test");
}

TEST(Carry, Builder) {
    carry::Builder builder;

    builder.SetString("key", "value");
    builder.SetInt64("port", 80).SetBool("flag", true);

    auto ctx = builder.Done();

    ASSERT_EQ(ctx.GetString("key"), "value");
    ASSERT_EQ(ctx.GetInt64("port"), 80);
    ASSERT_EQ(ctx.GetBool("flag"), true);
}

TEST(Carry, GetPanics) {
    auto ctx = carry::New().SetInt64("key", 123).Done();
    // ctx.GetUInt64("key");  // Panics
}

TEST(Carry, CollectKeys) {
    auto ctx1 = carry::New()
                    .SetString("header.timeout", "100500")
                    .SetString("header.trace_id", "abc-123")
                    .SetString("key1", "value1")
                    .Done();

    auto ctx2 = carry::Wrap(ctx1).SetString("header.timeout", "1000").SetString("key2", "value2").Done();

    auto headers = ctx2.CollectKeys("header.");

    ASSERT_EQ(headers.size(), 2);

    std::set<std::string> expectedHeaders;
    expectedHeaders.insert("header.timeout");
    expectedHeaders.insert("header.trace_id");

    ASSERT_EQ(headers, expectedHeaders);
}

TEST(Carry, Mutable) {
    carry::MutableContext mutCtx;
    auto ctx = mutCtx.View();

    mutCtx.Set<std::string>("key1", "value1");
    mutCtx.Set<int64_t>("key2", 100500);

    {
        ASSERT_EQ(ctx.Get<std::string>("key1"), "value1");
        ASSERT_EQ(ctx.Get<int64_t>("key2"), 100500);

        ASSERT_FALSE(ctx.TryGet<bool>("key3"));
    }

    mutCtx.Set<bool>("key3", true);

    ASSERT_EQ(ctx.GetBool("key3"), true);
}

class Carrier : public carry::ICarrier, public compass::Locator<carry::ICarrier> {
public:
    Carrier()
        : context_(carry::Empty()) {
        //
    }

    // carry::ICarrier

    void Set(carry::Context context) override {
        context_ = std::move(context);
    }

    const carry::Context& GetContext() override {
        return context_;
    }

    // compass::Locator

    carry::ICarrier* Locate(compass::Tag<carry::ICarrier>) override {
        return this;
    }

private:
    carry::Context context_;
};

TEST(Carry, Carrier) {
    Carrier carrier;

    compass::Map().Add(carrier);

    carrier.Set(carry::New().SetInt64("key", 7));

    {
        {
            auto value1 = carry::Current().TryGet<int64_t>("key");

            ASSERT_TRUE(value1);
            ASSERT_EQ(*value1, 7);
        }

        {
            carry::WrapScope<int64_t> scope("key", 9);

            auto value2 = carry::Current().TryGet<int64_t>("key");

            ASSERT_TRUE(value2);
            ASSERT_EQ(*value2, 9);
        }

        {
            auto value1 = carry::Current().TryGet<int64_t>("key");

            ASSERT_TRUE(value1);
            ASSERT_EQ(*value1, 7);
        }
    }

    compass::Map().Clear();
}
