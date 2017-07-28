#include "Clock.h"


Clock::TimePoint Clock::StartPoint = Clock::ClockType::now();

uint64_t Clock::GetMSecSinceStart() {
    typedef std::chrono::duration<uint64_t,std::milli> MilliDur; 
    return std::chrono::duration_cast<MilliDur>(
        ClockType::now() - StartPoint
    ).count();
}