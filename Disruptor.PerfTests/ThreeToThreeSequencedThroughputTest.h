#pragma once

#include "Disruptor/RingBuffer.h"

#include "Disruptor.TestTools/CountdownEvent.h"

#include "Disruptor.PerfTests/IThroughputTest.h"
#include "Disruptor.PerfTests/LongArrayEventHandler.h"
#include "Disruptor.PerfTests/MultiBufferBatchEventProcessor.h"


namespace Disruptor
{
namespace PerfTests
{

    /**
     *
     * Sequence a series of events from multiple publishers going to one event processor.
     * 
     * Disruptor:
     * ==========
     *             track to prevent wrap
     *             +--------------------+
     *             |                    |
     *             |                    |
     * +----+    +====+    +====+       |
     * | P1 |--->| RB |--->| SB |--+    |
     * +----+    +====+    +====+  |    |
     *                             |    v
     * +----+    +====+    +====+  | +----+
     * | P2 |--->| RB |--->| SB |--+>| EP |
     * +----+    +====+    +====+  | +----+
     *                             |
     * +----+    +====+    +====+  |
     * | P3 |--->| RB |--->| SB |--+
     * +----+    +====+    +====+
     *
     *
     * P1 - Publisher 1
     * P2 - Publisher 2
     * P3 - Publisher 3
     * RB - RingBuffer
     * SB - SequenceBarrier
     * EP - EventProcessor
     *
     */
    class ThreeToThreeSequencedThroughputTest : public IThroughputTest
    {
        using LongArrayPublisher = std::function< void(const std::shared_ptr< Tests::CountdownEvent >&,
                                                       const std::shared_ptr< RingBuffer< std::vector< std::int64_t > > >&,
                                                       std::int64_t,
                                                       std::int32_t) >;

    public:
        ThreeToThreeSequencedThroughputTest();

        std::int64_t run(Stopwatch& stopwatch) override;

        std::int32_t requiredProcessorCount() const override;

    private:
        static void valuePublisher(const std::shared_ptr< Tests::CountdownEvent >& countdownEvent,
                                   const std::shared_ptr< RingBuffer< std::vector< std::int64_t > > >& ringBuffer,
                                   std::int64_t iterations,
                                   std::int32_t arraySize);

        const std::int32_t m_numPublishers = 3;
        const std::int32_t m_arraySize = 3;
        const std::int32_t m_bufferSize = 1024 * 64;

#ifdef _DEBUG
        const std::int64_t m_iterations = 100L * 100L * 18L;
#else
        const std::int64_t m_iterations = 1000L * 1000L * 180L;
#endif

        std::shared_ptr< Tests::CountdownEvent > m_cyclicBarrier = std::make_shared< Tests::CountdownEvent >(m_numPublishers + 1);

        std::vector< std::shared_ptr< RingBuffer< std::vector< std::int64_t > > > > m_buffers = std::vector< std::shared_ptr< RingBuffer< std::vector< std::int64_t > > > >(m_numPublishers);
        std::vector< std::shared_ptr< ISequenceBarrier > > m_barriers = std::vector< std::shared_ptr< ISequenceBarrier > >(m_numPublishers);
        std::vector< LongArrayPublisher > m_valuePublishers = std::vector< LongArrayPublisher >(m_numPublishers);
        
        std::shared_ptr< LongArrayEventHandler > m_handler = std::make_shared< LongArrayEventHandler >();
        std::shared_ptr< MultiBufferBatchEventProcessor< std::vector< std::int64_t > > > m_batchEventProcessor;
    };

} // namespace PerfTests
} // namespace Disruptor
