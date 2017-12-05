#include "stdafx.h"
#include "OneToOneSequencedLongArrayThroughputTest.h"

#include "Disruptor/BasicExecutor.h"
#include "Disruptor/RoundRobinThreadAffinedTaskScheduler.h"
#include "Disruptor/YieldingWaitStrategy.h"

#include "Disruptor.TestTools/ScopeExitFunctor.h"

#include "PerfTestUtil.h"


namespace Disruptor
{
namespace PerfTests
{

    OneToOneSequencedLongArrayThroughputTest::OneToOneSequencedLongArrayThroughputTest()
    {
        m_taskScheduler = std::make_shared< RoundRobinThreadAffinedTaskScheduler >();
        m_executor = std::make_shared< BasicExecutor >(m_taskScheduler);

        m_ringBuffer = RingBuffer< std::vector< std::int64_t > >::createSingleProducer([&] { return std::vector< std::int64_t >(m_arraySize); },
                                                                                       m_bufferSize,
                                                                                       std::make_shared< YieldingWaitStrategy >());
        auto sequenceBarrier = m_ringBuffer->newBarrier();
        m_handler = std::make_shared< LongArrayEventHandler >();
        m_batchEventProcessor = std::make_shared< BatchEventProcessor< std::vector< std::int64_t > > >(m_ringBuffer, sequenceBarrier, m_handler);
        m_ringBuffer->addGatingSequences({ m_batchEventProcessor->sequence() });
    }

    std::int64_t OneToOneSequencedLongArrayThroughputTest::run(Stopwatch& stopwatch)
    {
        m_taskScheduler->start(requiredProcessorCount());
        TestTools::ScopeExitFunctor atScopeExit([this] { m_taskScheduler->stop(); });

        auto signal = std::make_shared< Tests::ManualResetEvent >(false);
        auto expectedCount = m_batchEventProcessor->sequence()->value() + m_iterations;
        m_handler->reset(signal, m_iterations);
        auto processorTask = m_executor->execute([this] { m_batchEventProcessor->run(); });

        auto& rb = *m_ringBuffer;

        stopwatch.start();

        for (auto i = 0; i < m_iterations; ++i)
        {
            auto next = rb.next();
            auto& event = rb[next];
            for (auto j = 0u; j < event.size(); ++j)
            {
                event[j] = i;
            }
            rb.publish(next);
        }

        signal->waitOne();
        stopwatch.stop();
        waitForEventProcessorSequence(expectedCount);
        m_batchEventProcessor->halt();
        processorTask.wait_for(std::chrono::seconds(10));

        PerfTestUtil::failIf(0, m_handler->value(), "Handler has not processed any event");

        return m_iterations * m_arraySize;
    }

    std::int32_t OneToOneSequencedLongArrayThroughputTest::requiredProcessorCount() const
    {
        return 2;
    }

    void OneToOneSequencedLongArrayThroughputTest::waitForEventProcessorSequence(std::int64_t expectedCount)
    {
        while (m_batchEventProcessor->sequence()->value() != expectedCount)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }

} // namespace PerfTests
} // namespace Disruptor
