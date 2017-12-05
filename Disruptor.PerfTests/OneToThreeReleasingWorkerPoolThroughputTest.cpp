#include "stdafx.h"
#include "OneToThreeReleasingWorkerPoolThroughputTest.h"

#include "Disruptor/BasicExecutor.h"
#include "Disruptor/FatalExceptionHandler.h"
#include "Disruptor/RoundRobinThreadAffinedTaskScheduler.h"

#include "Disruptor.TestTools/ScopeExitFunctor.h"

#include "PerfTestUtil.h"


namespace Disruptor
{
namespace PerfTests
{

    OneToThreeReleasingWorkerPoolThroughputTest::OneToThreeReleasingWorkerPoolThroughputTest()
    {
        for (auto i = 0; i < m_numWorkers; ++i)
        {
            m_counters[i] = std::make_shared< PaddedLong >();
        }

        for (auto i = 0; i < m_numWorkers; ++i)
        {
            m_handlers[i] = std::make_shared< EventCountingAndReleasingWorkHandler >(m_counters, i);
        }

        m_workerPool = std::make_shared< WorkerPool< ValueEvent > >(m_ringBuffer, m_ringBuffer->newBarrier(), std::make_shared< FatalExceptionHandler< ValueEvent > >(), m_handlers);

        m_ringBuffer->addGatingSequences({ m_workerPool->getWorkerSequences() });
    }

    std::int64_t OneToThreeReleasingWorkerPoolThroughputTest::run(Stopwatch& stopwatch)
    {
        auto scheduler = std::make_shared< RoundRobinThreadAffinedTaskScheduler >();
        scheduler->start(requiredProcessorCount());
        TestTools::ScopeExitFunctor atScopeExit([scheduler] { scheduler->stop(); });

        resetCounters();
        auto& ringBuffer = *m_workerPool->start(std::make_shared< BasicExecutor >(scheduler));
        stopwatch.start();

        for (std::int64_t i = 0; i < m_iterations; ++i)
        {
            std::int64_t sequence = ringBuffer.next();
            ringBuffer[sequence].value = i;
            ringBuffer.publish(sequence);
        }

        m_workerPool->drainAndHalt();

        // Workaround to ensure that the last worker(s) have completed after releasing their events
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        stopwatch.stop();

        PerfTestUtil::failIfNot(m_iterations, sumCounters());

        return m_iterations;
    }

    std::int32_t OneToThreeReleasingWorkerPoolThroughputTest::requiredProcessorCount() const
    {
        return 4;
    }

    void OneToThreeReleasingWorkerPoolThroughputTest::resetCounters()
    {
        for (auto i = 0; i < m_numWorkers; ++i)
        {
            m_counters[i]->value = 0L;
        }
    }

    std::int64_t OneToThreeReleasingWorkerPoolThroughputTest::sumCounters()
    {
        std::int64_t sumJobs = 0L;
        for (auto i = 0; i < m_numWorkers; ++i)
        {
            sumJobs += m_counters[i]->value;
        }

        return sumJobs;
    }

    OneToThreeReleasingWorkerPoolThroughputTest::EventCountingAndReleasingWorkHandler::EventCountingAndReleasingWorkHandler(const std::vector< std::shared_ptr< PaddedLong > >& counters, std::int32_t index)
        : m_counters(counters)
        , m_index(index)
    {
    }

    void OneToThreeReleasingWorkerPoolThroughputTest::EventCountingAndReleasingWorkHandler::onEvent(ValueEvent& /*evt*/)
    {
        m_eventReleaser->release();
        m_counters[m_index]->value = m_counters[m_index]->value + 1L;
    }

    void OneToThreeReleasingWorkerPoolThroughputTest::EventCountingAndReleasingWorkHandler::setEventReleaser(const std::shared_ptr< IEventReleaser >& eventReleaser)
    {
        m_eventReleaser = eventReleaser;
    }

} // namespace PerfTests
} // namespace Disruptor
