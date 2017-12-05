#include "stdafx.h"
#include "OneToOneSequencedThroughputTest.h"

#include "Disruptor/BasicExecutor.h"
#include "Disruptor/RoundRobinThreadAffinedTaskScheduler.h"
#include "Disruptor/YieldingWaitStrategy.h"

#include "Disruptor.TestTools/ScopeExitFunctor.h"


namespace Disruptor
{
namespace PerfTests
{

    OneToOneSequencedThroughputTest::OneToOneSequencedThroughputTest()
    {
        m_taskScheduler = std::make_shared< RoundRobinThreadAffinedTaskScheduler >();
        m_executor = std::make_shared< BasicExecutor >(m_taskScheduler);

        m_latch = std::make_shared< Tests::ManualResetEvent >(false);
        m_eventHandler = std::make_shared< ValueAdditionEventHandler >();
        m_ringBuffer = RingBuffer< ValueEvent >::createSingleProducer(ValueEvent::eventFactory(), m_bufferSize, std::make_shared< YieldingWaitStrategy >());
        auto sequenceBarrier = m_ringBuffer->newBarrier();
        m_batchEventProcessor = std::make_shared< BatchEventProcessor< ValueEvent > >(m_ringBuffer, sequenceBarrier, m_eventHandler);
        m_ringBuffer->addGatingSequences({ m_batchEventProcessor->sequence() });
    }

    std::int64_t OneToOneSequencedThroughputTest::run(Stopwatch& stopwatch)
    {
        m_taskScheduler->start(requiredProcessorCount());
        TestTools::ScopeExitFunctor atScopeExit([this] { m_taskScheduler->stop(); });

        auto expectedCount = m_batchEventProcessor->sequence()->value() + m_iterations;

        m_latch->reset();
        m_eventHandler->reset(m_latch, expectedCount);

        auto processorTask = m_executor->execute([this] { m_batchEventProcessor->run(); });
        stopwatch.start();

        auto&& rb = *m_ringBuffer;

        for (auto i = 0; i < m_iterations; ++i)
        {
            auto sequence = rb.next();
            rb[sequence].value = i;
            rb.publish(sequence);
        }

        m_latch->waitOne();
        stopwatch.stop();
        PerfTestUtil::waitForEventProcessorSequence(expectedCount, m_batchEventProcessor);
        m_batchEventProcessor->halt();
        processorTask.wait_for(std::chrono::seconds(10));

        PerfTestUtil::failIfNot(m_expectedResult, m_eventHandler->value(),
                                "Handler should have processed " + std::to_string(m_expectedResult) + " events, but was: " + std::to_string(m_eventHandler->value()));

        return m_iterations;
    }

    std::int32_t OneToOneSequencedThroughputTest::requiredProcessorCount() const
    {
        return 2;
    }

} // namespace PerfTests
} // namespace Disruptor
