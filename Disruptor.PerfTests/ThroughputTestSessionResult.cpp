#include "stdafx.h"
#include "ThroughputTestSessionResult.h"

#include <iomanip>
#include <sstream>

#include "Disruptor.TestTools/DurationHumanizer.h"
#include "Disruptor.TestTools/HumanNumberFacet.h"


namespace Disruptor
{
namespace PerfTests
{

    ThroughputTestSessionResult::ThroughputTestSessionResult(std::int64_t totalOperationsInRun, ClockConfig::Duration duration)
        : m_totalOperationsInRun(totalOperationsInRun)
        , m_duration(duration)
    {}

    ThroughputTestSessionResult::ThroughputTestSessionResult(const std::exception& exception)
        : m_exception(exception)
    {}

    void ThroughputTestSessionResult::appendDetailedHtmlReport(std::int32_t runId, std::ostringstream& os)
    {
        if (m_exception)
        {
            os
                << " <tr>" << std::endl
                << "     <td>" << runId << "</td>" << std::endl
                << "     <td>FAILED</td>" << std::endl
                << "     <td>" << m_exception.get().what() << "</td>" << std::endl
                << "</tr>" << std::endl;
        }
        else
        {
            os
                << "<tr>" << std::endl
                << "     <td>" << runId << "</td>" << std::endl
                << "     <td>" << ops() << "</td>" << std::endl
                << "     <td>" << durationInMilliseconds() << "</td>" << std::endl
                << "</tr>" << std::endl;
        }
    }

    std::string ThroughputTestSessionResult::toString()
    {
        std::ostringstream result;
        result.imbue(std::locale(result.getloc(), new Tests::HumanNumberFacet()));

        if (m_exception)
            result << "Run: FAILED: " << m_exception.get().what();
        else
        {
            auto humanDuration = Tests::DurationHumanizer::deduceHumanDuration(duration());

            result << "Run: Ops: " << ops()
                   << " - Duration (" << humanDuration.shortUnitName << "): " << std::setprecision(3) << humanDuration.value;
        }

        return result.str();
    }

    std::int64_t ThroughputTestSessionResult::totalOperationsInRun() const
    {
        return m_totalOperationsInRun;
    }

    ClockConfig::Duration ThroughputTestSessionResult::duration() const
    {
        return m_duration;
    }

    std::int64_t ThroughputTestSessionResult::ops() const
    {
        auto elapsed = std::chrono::duration_cast< std::chrono::microseconds >(m_duration).count();
        return static_cast< std::int64_t >(static_cast< double >(1000 * 1000 * m_totalOperationsInRun) / elapsed);
    }

    std::int64_t ThroughputTestSessionResult::durationInMilliseconds() const
    {
        auto totalMilliseconds = std::chrono::duration_cast< std::chrono::milliseconds >(m_duration);
        return static_cast< std::int64_t >(totalMilliseconds.count());
    }

} // namespace PerfTests
} // namespace Disruptor
