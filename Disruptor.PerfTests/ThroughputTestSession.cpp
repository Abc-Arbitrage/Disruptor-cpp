#include "stdafx.h"
#include "ThroughputTestSession.h"

#include <numeric>
#include <sstream>

#include <boost/filesystem.hpp>

#include "Disruptor.TestTools/Stopwatch.h"

#include "Disruptor.PerfTests/PerfTestUtil.h"
#include "Disruptor.PerfTests/ThroughputTestSessionResult.h"


namespace Disruptor
{
namespace PerfTests
{

    ThroughputTestSession::ThroughputTestSession(const ThroughputTestInfo& testInfo)
    {
        m_testInfo = testInfo;
    }

    void ThroughputTestSession::run()
    {
        checkProcessorsRequirements();

        std::cout << "Throughput Test to run => " << m_testInfo.name << ", Runs => " << m_runs << std::endl;
        std::cout << "Starting throughput tests" << std::endl;

        Stopwatch stopwatch;

        for (auto i = 0; i < m_runs; i++)
        {
            stopwatch.reset();

            std::int64_t totalOperationsInRun;

            try
            {
                totalOperationsInRun = m_testInfo.instance->run(stopwatch);
            }
            catch (std::exception& ex)
            {
                ThroughputTestSessionResult result(ex);
                std::cout << result.toString() << std::endl;

                m_results.push_back(result);
                continue;
            }

            ThroughputTestSessionResult result(totalOperationsInRun, stopwatch.elapsed());
            std::cout << result.toString() << std::endl;

            m_results.push_back(result);
        }
    }

    std::int64_t ThroughputTestSession::getAverageThroughput()
    {
        double averageThroughput = 0;
        for (auto&& x : m_results)
        {
            auto totalSeconds = std::chrono::duration_cast< std::chrono::seconds >(x.duration());
            averageThroughput += static_cast< double >(x.totalOperationsInRun()) / totalSeconds.count();
        }

        return static_cast< std::int64_t >(averageThroughput / static_cast< double >(m_results.size()));
    }


    void Disruptor::PerfTests::ThroughputTestSession::checkProcessorsRequirements() const
    {
        auto availableProcessors = static_cast< std::int32_t >(std::thread::hardware_concurrency());
        if (m_testInfo.instance->requiredProcessorCount() <= availableProcessors)
            return;

        std::cout << "*** Warning ***: your system has insufficient processors to execute the test efficiently. " << std::endl;
        std::cout << "Processors required = " << m_testInfo.instance->requiredProcessorCount() << ", available = " << availableProcessors << std::endl;
    }

} // namespace PerfTests
} // namespace Disruptor
