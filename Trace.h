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
            log();
        }

        explicit operator bool () const {
            return true;
        }

        void log() const {
            log(str.c_str());
        }

        void log( const char *str) const {
            size_t c = count();

            size_t s = c / size_t(1000000000);
            size_t n = (c%size_t(1000000000)) / size_t(100000);

            LOG_PERFORMANCE("%s - %zu.%04zu s", str, s, n);
        }

        uint64_t count() const {
            Clock::time_point end = Clock::now();
            Clock::duration dur = end - start;

            auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(dur);
            return (uint64_t)ns.count();
        }
    };
}

#define PERFORMACE_TRACE_FUNC(...) ::internal::PerformaceTraceObj _perf_trace{StringUtils::printf(__VA_ARGS__)}
#define BEGIN_PERFORMACE_TRACE(...) if (::internal::PerformaceTraceObj _perf_trace{StringUtils::printf(__VA_ARGS__)}) 
#define PERFORMACE_TRACE_POINT(name) _perf_trace.log(name)
