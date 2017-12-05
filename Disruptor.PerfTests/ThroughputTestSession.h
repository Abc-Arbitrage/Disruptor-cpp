#pragma once

#include "Disruptor.PerfTests/TestInfo.h"
#include "Disruptor.PerfTests/ThroughputTestSessionResult.h"


namespace Disruptor
{
namespace PerfTests
{

    class IThroughputTest;


    class ThroughputTestSession
    {
    public:
        explicit ThroughputTestSession(const ThroughputTestInfo& testInfo);

        void run();

    private:
        void checkProcessorsRequirements() const;
        std::int64_t getAverageThroughput();

#ifdef _DEBUG
        const std::int32_t m_runs = 3;
#else
        const std::int32_t m_runs = 7;
#endif

        std::vector< ThroughputTestSessionResult > m_results;
        ThroughputTestInfo m_testInfo;
    };

} // namespace PerfTests
} // namespace Disruptor
