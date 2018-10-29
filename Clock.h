#pragma once

#include "build_config.h"
#include <chrono>

/// @todo check if windows provides a good chrono clock.
class Clock {
public:
    typedef std::chrono::high_resolution_clock ClockType;
    typedef ClockType::time_point TimePoint;
    typedef TimePoint::duration Duration;
    typedef std::chrono::duration<double> Second;

    static COMMON_API uint64_t GetMSecSinceStart();
    
public:
    void start() {
        if( mIsPaused ) {
            TimePoint curTime = ClockType::now();
            Duration curDuration = mPausedPoint - mStartPoint;
            
            mStartPoint = curTime - curDuration;
            mIsPaused = false;
        }
        else {
            mStartPoint = ClockType::now();
        }
    }
    
    void pause() {
        if( !mIsPaused ) {
            mPausedPoint = ClockType::now();
            mIsPaused = true;
        }
    }
    
    void restart() {
        mStartPoint = ClockType::now();
        if( mIsPaused ) {
            mPausedPoint = mStartPoint;
        }
    }
    double seconds() const {
        TimePoint now;
        if( mIsPaused ) {
            now = mPausedPoint;
        }
        else {
            now = ClockType::now();
        }
        
        return std::chrono::duration_cast<Second>(now-mStartPoint).count();
    }
    
    void tooglePause() {
        if( mIsPaused ) {
            start();
        }
        else {
            pause();
        }
    }
    
    bool isPaused() const {
        return mIsPaused;
    }
    
private:
    TimePoint mStartPoint = ClockType::now(), 
              mPausedPoint;
    bool mIsPaused = false;
};