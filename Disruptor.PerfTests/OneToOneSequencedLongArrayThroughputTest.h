#pragma once

#include "Disruptor/BatchEventProcessor.h"
#include "Disruptor/IExecutor.h"
#include "Disruptor/ITaskScheduler.h"
#include "Disruptor/RingBuffer.h"

#include "Disruptor.PerfTests/IThroughputTest.h"
#include "Disruptor.PerfTests/LongArrayEventHandler.h"


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
     * Disruptor:
     * ==========
     *              track to prevent wrap
     *              +------------------+
     *              |                  |
     *              |                  v
     * +----+    +====+    +====+   +-----+
     * | P1 |---›| RB |‹---| SB |   | EP1 |
     * +----+    +====+    +====+   +-----+
     *      claim      get    ^        |
     *                        |        |
     *                        +--------+
     *                          waitFor
     * P1  - Publisher 1
     * RB  - RingBuffer
     * SB  - SequenceBarrier
     * EP1 - EventProcessor 1
     *
     */
    class OneToOneSequencedLongArrayThroughputTest : public IThroughputTest
    {
    public:
        OneToOneSequencedLongArrayThroughputTest();

        std::int64_t run(Stopwatch& stopwatch) override;

        std::int32_t requiredProcessorCount() const override;

    private:
        void waitForEventProcessorSequence(std::int64_t expectedCount);

        const std::int32_t m_bufferSize = 1024 * 1;

#ifdef _DEBUG
        const std::int64_t m_iterations = 100L * 100L * 1L;
#else
        const std::int64_t m_iterations = 1000L * 1000L * 1L;
#endif

        const std::int32_t m_arraySize = 2 * 1024;

        std::shared_ptr< ITaskScheduler > m_taskScheduler;
        std::shared_ptr< IExecutor > m_executor;
        std::shared_ptr< RingBuffer< std::vector< std::int64_t > > > m_ringBuffer;
        std::shared_ptr< LongArrayEventHandler > m_handler;
        std::shared_ptr< BatchEventProcessor< std::vector< std::int64_t > > > m_batchEventProcessor;
    };

} // namespace PerfTests
} // namespace Disruptor
