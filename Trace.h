#pragma once

#include "ErrorUtils.h"
#include "StringFormat.h"

#include <string>
#include <chrono>

namespace internal 
{
    using Clock = std::chrono::high_resolution_clock;

    struct PerformaceTraceObj {
        std::string str;
        Clock::time_point start = Clock::now();

        ~PerformaceTraceObj() {
            LOG_PERFORMANCE("%s - %zu ns", str.c_str(), count());
        }

        explicit operator bool () const {
            return true;
        }

        uint64_t count() {
            Clock::time_point end = Clock::now();
            Clock::duration dur = end - start;

            auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(dur);
            return (uint64_t)ns.count();
        }
    };
}

#define PERFORMACE_TRACE_FUNC(...) ::internal::PerformaceTraceObj _perf_trace{StringUtils::printf(__VA_ARGS__)}
#define BEGIN_PERFORMACE_TRACE(...) if (::internal::PerformaceTraceObj _perf_trace{StringUtils::printf(__VA_ARGS__)}) 
#define PERFORMACE_TRACE_POINT(name, ...) \
    LOG_PERFORMANCE("%s - %zu ns", name, _perf_trace.count())