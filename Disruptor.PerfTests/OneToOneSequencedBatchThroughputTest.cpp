#include "stdafx.h"
#include "OneToOneSequencedBatchThroughputTest.h"

#include "Disruptor/BasicExecutor.h"
#include "Disruptor/RoundRobinThreadAffinedTaskScheduler.h"
#include "Disruptor/YieldingWaitStrategy.h"

#include "Disruptor.TestTools/ScopeExitFunctor.h"


namespace Disruptor
{
namespace PerfTests
{

    OneToOneSequencedBatchThroughputTest::OneToOneSequencedBatchThroughputTest()
    {
        m_taskScheduler = std::make_shared< RoundRobinThreadAffinedTaskScheduler >();
        m_executor = std::make_shared< BasicExecutor >(m_taskScheduler);

        m_ringBuffer = RingBuffer< ValueEvent >::createSingleProducer(ValueEvent::eventFactory(), m_bufferSize, std::make_shared< YieldingWaitStrategy >());
        auto sequenceBarrier = m_ringBuffer->newBarrier();
        m_handler = std::make_shared< ValueAdditionEventHandler >();
        m_batchEventProcessor = std::make_shared< BatchEventProcessor< ValueEvent > >(m_ringBuffer, sequenceBarrier, m_handler);
        m_ringBuffer->addGatingSequences({ m_batchEventProcessor->sequence() });
    }

    std::int64_t OneToOneSequencedBatchThroughputTest::run(Stopwatch& stopwatch)
    {
        m_taskScheduler->start(requiredProcessorCount());
        TestTools::ScopeExitFunctor atScopeExit([this] { m_taskScheduler->stop(); });

        auto signal = std::make_shared< Tests::ManualResetEvent >(false);
        auto expectedCount = m_batchEventProcessor->sequence()->value() + m_iterations * m_batchSize;
        m_handler->reset(signal, expectedCount);
        auto processorTask = m_executor->execute([this] { m_batchEventProcessor->run(); });
        stopwatch.start();

        auto&& rb = *m_ringBuffer;

        for (auto i = 0; i < m_iterations; ++i)
        {
            auto hi = rb.next(m_batchSize);
            auto lo = hi - (m_batchSize - 1);
            for (auto l = lo; l <= hi; ++l)
            {
                rb[l].value = (i);
            }
            rb.publish(lo, hi);
        }

        signal->waitOne();
        stopwatch.stop();
        PerfTestUtil::waitForEventProcessorSequence(expectedCount, m_batchEventProcessor);
        m_batchEventProcessor->halt();
        processorTask.wait_for(std::chrono::milliseconds(2000));

        PerfTestUtil::failIfNot(m_expectedResult, m_handler->value(),
                                "Handler should have processed " + std::to_string(m_expectedResult) + " events, but was: " + std::to_string(m_handler->value()));

        return m_batchSize * m_iterations;
    }

    std::int32_t OneToOneSequencedBatchThroughputTest::requiredProcessorCount() const
    {
        return 2;
    }

} // namespace PerfTests
} // namespace Disruptor
