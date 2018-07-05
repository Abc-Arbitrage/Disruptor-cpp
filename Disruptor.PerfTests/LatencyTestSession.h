#pragma once

#include <memory>
#include <string>

#include "Disruptor.PerfTests/TestFactory.h"
#include "LatencyTestSessionResult.h"


namespace Disruptor
{
namespace PerfTests
{

    class ILatencyTest;


    class LatencyTestSession
    {
    public:
        explicit LatencyTestSession(const LatencyTestInfo& testInfo);

        void run();

        static std::int64_t convertStopwatchTicksToNano(double durationInTicks);
        static double convertNanoToStopwatchTicks(std::int64_t pauseDurationInNanos);

    private:
        static void checkProcessorsRequirements(const std::shared_ptr< ILatencyTest >& test);

        const std::int32_t m_runs = 3;

        LatencyTestInfo m_testInfo;
        std::vector< LatencyTestSessionResult > m_results;
    };

} // namespace PerfTests
} // namespace Disruptor
