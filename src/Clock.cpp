#include "Clock.h"

namespace
{
    Clock::TimePoint ClockStartPoint = Clock::ClockType::now();
}

COMMON_API uint64_t Clock::GetMSecSinceStart() {
    typedef std::chrono::duration<uint64_t,std::milli> MilliDur; 
    return std::chrono::duration_cast<MilliDur>(
        ClockType::now() - ClockStartPoint
    ).count();
}