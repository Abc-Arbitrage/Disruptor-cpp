#pragma once

#include <chrono>
#include <ratio>
#include <type_traits>


namespace Disruptor
{

    struct ClockConfig
    {
        using Clock = std::conditional< std::chrono::high_resolution_clock::is_steady, std::chrono::high_resolution_clock, std::chrono::steady_clock >::type;
        
        using TimePoint = Clock::time_point;
        using Duration = Clock::duration;

        using Frequency = std::ratio< Duration::period::den, Duration::period::num >;
    };

} // namespace Disruptor
