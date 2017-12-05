#include "stdafx.h"
#include "LatencyTestSessionResult.h"

#include <iomanip>
#include <sstream>

#include "Disruptor.TestTools/DurationHumanizer.h"
#include "Disruptor.TestTools/HumanNumberFacet.h"


namespace Disruptor
{
namespace PerfTests
{

    LatencyTestSessionResult::LatencyTestSessionResult(const std::shared_ptr< Tests::LatencyRecorder >& latencyRecorder, const ClockConfig::Duration& duration)
        : m_latencyRecorder(latencyRecorder)
        , m_duration(duration)
    {
    }

    LatencyTestSessionResult::LatencyTestSessionResult(const std::exception& exception)
        : m_exception(exception)
    {
    }

    std::string LatencyTestSessionResult::toString()
    {
        std::ostringstream result;
        result.imbue(std::locale(result.getloc(), new Tests::HumanNumberFacet()));

        if (m_exception)
            result << "Run: FAILED: " << m_exception.get().what();
        else
        {
            auto humanDuration = Tests::DurationHumanizer::deduceHumanDuration(duration());

            result << "Run: "
                << "Duration (" << humanDuration.shortUnitName << "): " << std::setprecision(3) << humanDuration.value
                << ", Latency: ";
            m_latencyRecorder->writeReport(result);
        }

        return result.str();
    }

    ClockConfig::Duration LatencyTestSessionResult::duration() const
    {
        return m_duration;
    }

} // namespace PerfTests
} // namespace Disruptor
