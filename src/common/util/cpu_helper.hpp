#pragma once

#include <cstring>
#include <iostream>
#include <sstream>
#include <string>

namespace common::system {

class CpuIdHelper {
private:
    static void ReadRegisterChars(std::ostream& os, uint32_t reg) {
        char const* str = reinterpret_cast<char const*>(&reg);
        os.write(str, strnlen(str, sizeof(reg)));
    }

public:
    struct CpuIdResult {
        uint32_t eax;
        uint32_t ebx;
        uint32_t ecx;
        uint32_t edx;
    };

    static std::string GetProcessorDescription() {
        std::stringstream ret;
        CpuIdResult vendorRes = Query(0);
        ReadRegisterChars(ret, vendorRes.ebx);
        ReadRegisterChars(ret, vendorRes.edx);
        ReadRegisterChars(ret, vendorRes.ecx);
        ret << ' ';
        for (uint32_t leaf = 0x80000002; leaf <= 0x80000004; ++leaf) {
            CpuIdResult brandRes = Query(leaf);
            ReadRegisterChars(ret, brandRes.eax);
            ReadRegisterChars(ret, brandRes.ebx);
            ReadRegisterChars(ret, brandRes.ecx);
            ReadRegisterChars(ret, brandRes.edx);
        }
        return ret.str();
    }

    // return nominal freq in KHz, or zero if cannot determine
    // static uint32_t GetNominalFrequencyKHz() {
    //     std::string procDesc = getProcessorDescription();
    //     boost::regex re("([0-9.]+) ?GHz");
    //     boost::smatch m;
    //     if (boost::regex_search(procDesc, m, re)) {
    //         const std::string& s = m[1];
    //         return (uint32_t)(boost::lexical_cast<double>(s) * 1e6);
    //     } else {
    //         return 0;
    //     }
    // }

    /** Returns cache line size in bytes */
    static size_t GetCacheLineSize() {
        CpuIdResult res = Query(1);
        return 8 * ((res.ebx >> 8) & 0xFF);
    }

    /** Returns true iff invariant TSC */
    static bool IsInvariantTsc() {
        CpuIdResult res = Query(0x80000007);
        return (res.edx & 0x100) != 0u;
    }

    static CpuIdResult Query(uint32_t leaf, uint32_t subleaf = 0) {
        CpuIdResult result;
        __asm__ __volatile__("cpuid"
                             : "=a"(result.eax), // outputs
                               "=b"(result.ebx), "=c"(result.ecx),
                               "=d"(result.edx)
                             : "a"(leaf), // inputs
                               "c"(subleaf)
                             :); // no other clobbering
        return result;
    }
};

} // namespace common::system
