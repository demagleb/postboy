#pragma once

#include <postboy/base/queues/queue.hpp>
#include <postboy/base/processor.hpp>
#include <postboy/base/binding.hpp>

namespace postboy {

template <typename... Inputs>
class TakeableProcessor : virtual public Processor {
public:
    using Processor::Config;
    using Processor::kInfCallNumber;
    using Processor::Processor;

    template <size_t K>
    using KthInput = std::tuple_element_t<K, std::tuple<Inputs...>>;

    virtual ~TakeableProcessor() = default;

    virtual void LinkInputQueue(size_t index, std::shared_ptr<IQueueBase> queue) final {
        ForIndex(index, [&]<size_t Idx> {
            ENSURE(!std::get<Idx>(inputQueues_).Valid(), "Valid queue already exists");
            std::get<Idx>(inputQueues_).SetImpl(queue);
        });
    }

    virtual void UnlinkInputQueues() final {
        ForEachIndex([&]<size_t Idx> { std::get<Idx>(inputQueues_).Clear(); });
    }

    virtual bool IsValidTakeableProcessor() const final {
        bool result = true;
        ForEachIndex([&]<size_t Idx> { result &= std::get<Idx>(inputQueues_).Valid(); });
        return result;
    }

    FORCE_INLINE bool IsTakeable() const final {
        return true;
    }

protected:
    template <size_t Idx>
        requires(Idx < sizeof...(Inputs))
    FORCE_INLINE auto Take() {
        return std::get<Idx>(inputQueues_).Take();
    }

    template <typename T>
        requires(... || std::same_as<T, Inputs>)
    FORCE_INLINE auto Take() {
        return std::get<T>(inputQueues_).Take();
    }

    FORCE_INLINE auto Take()
        requires(sizeof...(Inputs) == 1)
    {
        return std::get<0>(inputQueues_).Take();
    }

    using QueueTuple = decltype(std::make_tuple(BoxedConsumerQueue<Inputs...>{}));
    QueueTuple inputQueues_;

private:
    template <typename Func>
    static void ForEachIndex(Func func) {
        using Indices = std::make_index_sequence<sizeof...(Inputs)>;
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
};

} // namespace postboy
