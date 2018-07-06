#pragma once

#include "Disruptor/BatchEventProcessor.h"
#include "Disruptor/IExecutor.h"
#include "Disruptor/ITaskScheduler.h"
#include "Disruptor/RingBuffer.h"

#include "Disruptor.TestTools/ManualResetEvent.h"

#include "Disruptor.PerfTests/IThroughputTest.h"
#include "Disruptor.PerfTests/PerfTestUtil.h"
#include "Disruptor.PerfTests/ValueAdditionEventHandler.h"
#include "Disruptor.PerfTests/ValueEvent.h"


namespace Disruptor
{
namespace PerfTests
{

    /**
     * UniCast a series of items between 1 publisher and 1 event processor.
     *
     * +----+    +-----+
     * | P1 |--->| EP1 |
     * +----+    +-----+
     *
     * Disruptor:
     * ==========
     *              track to prevent wrap
     *              +------------------+
     *              |                  |
     *              |                  v
     * +----+    +====+    +====+   +-----+
     * | P1 |---\| RB |/---| SB |   | EP1 |
     * +----+    +====+    +====+   +-----+
     *      claim       get   ^        |
     *                        |        |
     *                        +--------+
     *                          waitFor
     *
     * P1  - Publisher 1
     * RB  - RingBuffer
     * SB  - SequenceBarrier
     * EP1 - EventProcessor 1
     *
     */
    class OneToOneSequencedThroughputTest : public IThroughputTest
    {
    public:
        OneToOneSequencedThroughputTest();

        std::int64_t run(Stopwatch& stopwatch) override;

        std::int32_t requiredProcessorCount() const override;

    private:
        const std::int32_t m_bufferSize = 1024 * 64;

#ifdef _DEBUG
        const std::int64_t m_iterations = 100L * 100L * 10L;
#else
        const std::int64_t m_iterations = 1000L * 1000L * 100L;
#endif

        std::shared_ptr< RingBuffer< ValueEvent > > m_ringBuffer;
        std::shared_ptr< ValueAdditionEventHandler > m_eventHandler;
        std::shared_ptr< Tests::ManualResetEvent > m_latch;
        const std::int64_t m_expectedResult = PerfTestUtil::accumulatedAddition(m_iterations);
        std::shared_ptr< BatchEventProcessor< ValueEvent > > m_batchEventProcessor;
        std::shared_ptr< IExecutor > m_executor;
        std::shared_ptr< ITaskScheduler > m_taskScheduler;
    };

} // namespace PerfTests
} // namespace Disruptor
