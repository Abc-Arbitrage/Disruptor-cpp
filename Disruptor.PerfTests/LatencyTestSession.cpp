#include "stdafx.h"
#include "LatencyTestSession.h"

#include "Disruptor.TestTools/LatencyRecorder.h"
#include "Disruptor.TestTools/Stopwatch.h"


namespace Disruptor
{
namespace PerfTests
{

    LatencyTestSession::LatencyTestSession(const LatencyTestInfo& testInfo)
    {
        m_testInfo = testInfo;
    }

    void LatencyTestSession::run()
    {
        auto testInstance = m_testInfo.factory();

        checkProcessorsRequirements(testInstance);

        std::cout << "Latency Test to run => " << m_testInfo.name << ", Runs => " << m_runs << std::endl;
        std::cout << "Starting latency tests" << std::endl;;

        Stopwatch stopwatch;
        for (auto i = 0; i < m_runs; ++i)
        {
            stopwatch.reset();
            auto histogram = std::make_shared< Tests::LatencyRecorder >(100000000L);

            try
            {
                testInstance->run(stopwatch, histogram);
            }
            catch (std::exception& ex)
            {
                LatencyTestSessionResult result(ex);
                std::cout << result.toString() << std::endl;
                m_results.push_back(result);
                continue;
            }

            LatencyTestSessionResult result(histogram, stopwatch.elapsed());
            std::cout << result.toString() << std::endl;
            m_results.push_back(result);
        }
    }

    void LatencyTestSession::checkProcessorsRequirements(const std::shared_ptr< ILatencyTest >& test)
    {
        auto availableProcessors = static_cast< std::int32_t >(std::thread::hardware_concurrency());
        if (test->requiredProcessorCount() <= availableProcessors)
            return;

        std::cout << "*** Warning ***: your system has insufficient processors to execute the test efficiently. " << std::endl;
        std::cout << "Processors required = " << test->requiredProcessorCount() << ", available = " << availableProcessors << std::endl;
    }

    std::int64_t LatencyTestSession::convertStopwatchTicksToNano(double durationInTicks)
    {
        auto clockFrequency = static_cast< double >(ClockConfig::Duration::period::den) / static_cast< double >(ClockConfig::Duration::period::num);
        auto durationInNano = (durationInTicks / clockFrequency) * std::pow(10, 9);
        return static_cast< std::int64_t >(durationInNano);
    }

    double LatencyTestSession::convertNanoToStopwatchTicks(std::int64_t pauseDurationInNanos)
    {
        auto clockFrequency = static_cast< double >(ClockConfig::Duration::period::den) / static_cast< double >(ClockConfig::Duration::period::num);
        return pauseDurationInNanos * std::pow(10, -9) * clockFrequency;
    }

} // namespace PerfTests
} // namespace Disruptor
