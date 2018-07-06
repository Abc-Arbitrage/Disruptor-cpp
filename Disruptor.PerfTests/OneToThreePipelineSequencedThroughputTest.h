#pragma once

#include "Disruptor/RingBuffer.h"
#include "Disruptor/YieldingWaitStrategy.h"

#include "Disruptor.PerfTests/ExecutorService.h"
#include "Disruptor.PerfTests/FunctionEvent.h"
#include "Disruptor.PerfTests/FunctionEventHandler.h"
#include "Disruptor.PerfTests/IThroughputTest.h"


namespace Disruptor
{
namespace PerfTests
{

    /**
     * Pipeline a series of stages from a publisher to ultimate event processor.
     * Each event processor depends on the output of the event processor.
     * 
     * +----+    +-----+    +-----+    +-----+
     * | P1 |--->| EP1 |--->| EP2 |--->| EP3 |
     * +----+    +-----+    +-----+    +-----+
     * 
     * Disruptor:
     * ==========
     *                           track to prevent wrap
     *              +----------------------------------------------------------------+
     *              |                                                                |
     *              |                                                                v
     * +----+    +====+    +=====+    +-----+    +=====+    +-----+    +=====+    +-----+
     * | P1 |--->| RB |    | SB1 |/---| EP1 |/---| SB2 |/---| EP2 |/---| SB3 |/---| EP3 |
     * +----+    +====+    +=====+    +-----+    +=====+    +-----+    +=====+    +-----+
     *      claim   ^  get    |   waitFor           |   waitFor           |  waitFor
     *              |         |                     |                     |
     *              +---------+---------------------+---------------------+
     *        
     * P1  - Publisher 1
     * RB  - RingBuffer
     * SB1 - SequenceBarrier 1
     * EP1 - EventProcessor 1
     * SB2 - SequenceBarrier 2
     * EP2 - EventProcessor 2
     * SB3 - SequenceBarrier 3
     * EP3 - EventProcessor 3
     * 
     */
    class OneToThreePipelineSequencedThroughputTest : public IThroughputTest
    {
    public:
        OneToThreePipelineSequencedThroughputTest();

        std::int64_t run(Stopwatch& stopwatch) override;

        std::int32_t requiredProcessorCount() const override;

    private:
        const std::int32_t m_bufferSize = 1024 * 8;

#ifdef _DEBUG
        const std::int64_t m_iterations = 100L * 100L * 10L;
#else
        const std::int64_t m_iterations = 1000L * 1000L * 100L;
#endif

        std::shared_ptr< ExecutorService< FunctionEvent > > m_executor = std::make_shared< ExecutorService< FunctionEvent > >();

        const std::int64_t m_operandTwoInitialValue = 777L;
        std::int64_t m_expectedResult;

        std::shared_ptr< RingBuffer< FunctionEvent > > m_ringBuffer = RingBuffer< FunctionEvent >::createSingleProducer(FunctionEvent::eventFactory(), m_bufferSize, std::make_shared< YieldingWaitStrategy >());

        std::shared_ptr< BatchEventProcessor< FunctionEvent > > m_stepOneBatchProcessor;
        std::shared_ptr< BatchEventProcessor< FunctionEvent > > m_stepTwoBatchProcessor;
        std::shared_ptr< BatchEventProcessor< FunctionEvent > > m_stepThreeBatchProcessor;
        std::shared_ptr< FunctionEventHandler > m_stepThreeFunctionHandler;
    };

} // namespace PerfTests
} // namespace Disruptor
