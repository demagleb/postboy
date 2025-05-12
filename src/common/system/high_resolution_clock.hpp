#include <common/util/types.hpp>
#include <common/util/panic.hpp>

#include <iostream>
#include <cstdint>

namespace common::system {

static FORCE_INLINE uint64_t CurrentTime(void) {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return ((ts.tv_sec * 1000000000) + ts.tv_nsec);
}

class TimeRdtsc {
    static const uint64_t kMaxRetries = 1000;
    static const uint64_t kInterval = (1ull << 24);

public:
    static void Init() {
        uint64_t rt2(0);
        uint64_t err1 = kInterval;

        for (unsigned int i = 0; i < kMaxRetries; ++i) {
            uint64_t tsc1 = StartImpl();
            uint64_t rt1 = CurrentTime();

            uint64_t timestampSecond = StopImpl();
            while (timestampSecond - tsc1 < kInterval) {
                timestampSecond = StopImpl();
            }
            uint64_t rateTimeSecond = CurrentTime();

            uint64_t err;
            if ((timestampSecond - tsc1) > kInterval) {
                err = (timestampSecond - tsc1) - kInterval;
            } else {
                err = kInterval - (timestampSecond - tsc1);
            }

            if (err < err1) {
                err1 = err;
                Tscbase() = tsc1;
                Ref() = rt1;
                rt2 = rateTimeSecond;
            }
        }
        Scale() = rt2 - Ref();
        uint64_t maxVal = 1ull << 63;
        Resetlimit() = (maxVal / Scale()) >> 1;
    }

    static inline uint64_t GetCurrentTime() {
        uint64_t cyclesHigh, cyclesLow;
        asm volatile("rdtscp\n\t"
                     "mov %%rdx, %0\n\t"
                     "mov %%rax, %1\n\t"
                     : "=r"(cyclesHigh), "=r"(cyclesLow)::"%rax", "%rbx", "%rcx", "%rdx");
        uint64_t tsc = (cyclesHigh << 32) + cyclesLow;
        return (tsc * Scale()) >> 24;

        if (tsc - Tscbase() > Resetlimit()) {
            Ref() = CurrentTime();
            Tscbase() = StartImpl();
            return Ref();
        }

        return (((tsc - Tscbase()) * Scale()) >> 24) + Ref();
    }

    void Start() {
        start_ = StartImpl();
    }

    void Stop() {
        end_ = StopImpl();
    }

    uint64_t GetElapsedTicks() {
        return end_ - start_;
    }
    uint64_t GetElapsedTime() {
        return ((end_ - start_) * Scale()) >> 24;
    }

private:
    static inline uint64_t& Tscbase() {
        static uint64_t x;
        return x;
    }

    static inline uint64_t& Ref() {
        static uint64_t x;
        return x;
    }

    static inline uint64_t& Scale() {
        static uint64_t x;
        return x;
    }

    static inline uint64_t& Resetlimit() {
        static uint64_t x;
        return x;
    }

    static uint64_t StartImpl() {
        uint64_t cyclesHigh, cyclesLow;
        asm volatile("cpuid\n\t"
                     "rdtsc\n\t"
                     "mov %%rdx, %0\n\t"
                     "mov %%rax, %1\n\t"
                     : "=r"(cyclesHigh), "=r"(cyclesLow)::"%rax", "%rbx", "%rcx", "%rdx");

        return (cyclesHigh << 32) + cyclesLow;
    }

    static uint64_t StopImpl() {
        uint64_t cyclesHigh, cyclesLow;
        asm volatile("rdtscp\n\t"
                     "mov %%rdx, %0\n\t"
                     "mov %%rax, %1\n\t"
                     "cpuid\n\t"
                     : "=r"(cyclesHigh), "=r"(cyclesLow)::"%rax", "%rbx", "%rcx", "%rdx");
        return (cyclesHigh << 32) + cyclesLow;
    }

private:
    uint64_t start_;
    uint64_t end_;
};

} // namespace common::system
