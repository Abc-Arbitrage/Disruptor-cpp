#pragma once

#include "Disruptor/BatchEventProcessor.h"
#include "Disruptor/IExecutor.h"
#include "Disruptor/ITaskScheduler.h"
#include "Disruptor/RingBuffer.h"

#include "Disruptor.PerfTests/IThroughputTest.h"
#include "Disruptor.PerfTests/ValueEvent.h"
#include "Disruptor.PerfTests/ValueMutationEventHandler.h"


namespace Disruptor
{
namespace PerfTests
{

    /**
     * MultiCast a series of items between 1 publisher and 3 event processors.
     *           +-----+
     *    +----->| EP1 |
     *    |      +-----+
     *    |
     * +----+    +-----+
     * | P1 |--->| EP2 |
     * +----+    +-----+
     *    |
     *    |      +-----+
     *    +----->| EP3 |
     *           +-----+
     * Disruptor:
     * ==========
     *                             track to prevent wrap
     *             +--------------------+----------+----------+
     *             |                    |          |          |
     *             |                    v          v          v
     * +----+    +====+    +====+    +-----+    +-----+    +-----+
     * | P1 |---\| RB |/---| SB |    | EP1 |    | EP2 |    | EP3 |
     * +----+    +====+    +====+    +-----+    +-----+    +-----+
     *      claim      get    ^         |          |          |
     *                        |         |          |          |
     *                        +---------+----------+----------+
     *                                      waitFor
     * P1  - Publisher 1
     * RB  - RingBuffer
     * SB  - SequenceBarrier
     * EP1 - EventProcessor 1
     * EP2 - EventProcessor 2
     * EP3 - EventProcessor 3
     *
     */
    class OneToThreeSequencedThroughputTest : public IThroughputTest
    {
    public:
        OneToThreeSequencedThroughputTest();

        std::int64_t run(Stopwatch& stopwatch) override;

        std::int32_t requiredProcessorCount() const override;

    private:
        const std::int32_t m_numEventProcessors = 3;
        const std::int32_t m_bufferSize = 1024 * 8;

#ifdef _DEBUG
        const std::int64_t m_iterations = 100L * 100L * 10L;
#else
        const std::int64_t m_iterations = 1000L * 1000L * 100L;
#endif

        std::shared_ptr< RingBuffer< ValueEvent > > m_ringBuffer;
        std::vector< std::shared_ptr< BatchEventProcessor< ValueEvent > > > m_batchEventProcessors = std::vector< std::shared_ptr< BatchEventProcessor< ValueEvent > > >(m_numEventProcessors);
        std::vector< std::int64_t > m_results = std::vector< std::int64_t >(m_numEventProcessors);
        std::vector< std::shared_ptr< ValueMutationEventHandler > > m_handlers = std::vector< std::shared_ptr< ValueMutationEventHandler > >(m_numEventProcessors);
        
        std::shared_ptr< IExecutor > m_executor;
        std::shared_ptr< ITaskScheduler > m_taskScheduler;
    };

} // namespace PerfTests
} // namespace Disruptor
