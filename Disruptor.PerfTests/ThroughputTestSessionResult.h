#pragma once

#include <cstdint>
#include <exception>
#include <iosfwd>

#include <boost/optional.hpp>

#include "Disruptor/ClockConfig.h"


namespace Disruptor
{
namespace PerfTests
{

    class ThroughputTestSessionResult
    {
    public:
        ThroughputTestSessionResult(std::int64_t totalOperationsInRun, ClockConfig::Duration duration);

        explicit ThroughputTestSessionResult(const std::exception& exception);

        void appendDetailedHtmlReport(std::int32_t runId, std::ostringstream& stringBuilder);
        std::string toString();

        std::int64_t totalOperationsInRun() const;
        ClockConfig::Duration duration() const;

        std::int64_t ops() const;
        std::int64_t durationInMilliseconds() const;

    private:
        std::int64_t m_totalOperationsInRun = 0;
        ClockConfig::Duration m_duration;
        boost::optional< std::exception > m_exception;
    };

} // namespace PerfTests
} // namespace Disruptor
