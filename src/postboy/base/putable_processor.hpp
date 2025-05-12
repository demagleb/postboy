#pragma once

#include <postboy/base/postboy.hpp>
#include <postboy/base/processor.hpp>
#include <postboy/base/queues/queue_base.hpp>
#include <postboy/base/queues/queue.hpp>
#include <postboy/base/takeable_processor.hpp>
#include <postboy/base/typed_binding.hpp>

#include <common/util/assert.hpp>

#include <concepts>
#include <memory>
#include <tuple>
#include <utility>

namespace postboy {

template <typename... Outputs>
class PutableProcessor : virtual public Processor {
public:
    using Processor::Config;
    using Processor::kInfCallNumber;
    using Processor::Processor;

    template <size_t K>
    using KthOutput = std::tuple_element_t<K, std::tuple<Outputs...>>;

    virtual ~PutableProcessor() = default;

    virtual void LinkOutputQueue(size_t index, std::shared_ptr<IQueueBase> queue) final {
        ForIndex(index, [&]<size_t Idx> {
            ENSURE(!std::get<Idx>(outputQueues_).Valid(), "Valid queue already exists");
            std::get<Idx>(outputQueues_).SetImpl(queue);
        });
    }

    virtual void UnlinkOutputQueues() final {
        ForEachIndex([&]<size_t Idx> { std::get<Idx>(outputQueues_).Clear(); });
    }

    virtual std::unique_ptr<Binding> CreateOutputBinding(size_t index) final {
        std::unique_ptr<Binding> binding;
        ForIndex(index, [&]<size_t Idx> { binding = std::make_unique<TypedBinding<KthOutput<Idx>>>(); });
        return binding;
    }

    virtual bool IsValidPutableProcessor() const final {
        bool result = true;
        ForEachIndex([&]<size_t Idx> { result &= std::get<Idx>(outputQueues_).Valid(); });
        return result;
    }

    virtual bool IsPutable() const final {
        return true;
    }

protected:
    template <size_t Idx>
        requires(Idx < sizeof...(Outputs))
    FORCE_INLINE void Put(KthOutput<Idx> el) {
        std::get<Idx>(outputQueues_).Put(std::move(el));
    }

    template <typename T>
        requires((... || std::same_as<T, Outputs>) && sizeof...(Outputs) > 1)
    FORCE_INLINE void Put(T&& el) {
        std::get<T>(outputQueues_).Put(std::forward<T>(el));
    }

    FORCE_INLINE void Put(KthOutput<0> el)
        requires(sizeof...(Outputs) == 1)
    {
        std::get<0>(outputQueues_).Put(std::move(el));
    }

private:
    template <typename Func>
    static void ForEachIndex(Func func) {
        using Indices = std::make_index_sequence<sizeof...(Outputs)>;
        [&]<size_t... Idxs>(std::index_sequence<Idxs...>) { (func.template operator()<Idxs>(), ...); }(Indices{});
    }

    template <typename Func>
    static void ForIndex(size_t index, Func func) {
        ForEachIndex([&]<size_t Idx> {
            if (index == Idx) {
                func.template operator()<Idx>();
            }
        });
    }

    using QueueTuple = std::tuple<BoxedProducerQueue<Outputs>...>;

    QueueTuple outputQueues_;
};

} // namespace postboy
