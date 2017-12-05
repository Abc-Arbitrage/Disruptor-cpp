#pragma once

#include <chrono>
#include <boost/optional.hpp>

#include "Disruptor/ClockConfig.h"


namespace Disruptor
{
namespace PerfTests
{

    class Stopwatch
    {
    public:
        Stopwatch();

        ClockConfig::Duration elapsed() const;

        bool isRunning() const;

        static Stopwatch startNew();

        static std::int64_t getTimestamp();

        void start();
        void stop();

        void reset();

        ClockConfig::TimePoint restart();

    private:
        boost::optional< bool > m_isRunning;
        ClockConfig::TimePoint m_begin;
        mutable ClockConfig::TimePoint m_end;
    };

} // namespace PerfTests
} // namespace Disruptor
