#pragma once

#include "Disruptor/BatchEventProcessor.h"
#include "Disruptor/RingBuffer.h"
#include "Disruptor/YieldingWaitStrategy.h"

#include "Disruptor.PerfTests/FizzBuzzEvent.h"
#include "Disruptor.PerfTests/FizzBuzzEventHandler.h"
#include "Disruptor.PerfTests/IThroughputTest.h"


namespace Disruptor
{
namespace PerfTests
{

    /**
     * Produce an event replicated to two event proces
     *           +-----+
     *    +----->| EP1 |------+
     *    |      +-----+      |
     *    |                   v
     * +----+              +-----+
     * | P1 |              | EP3 |
     * +----+              +-----+
     *    |                   ^
     *    |      +-----+      |
     *    +----->| EP2 |------+
     *           +-----+
     * Disruptor:
     * ==========
     *                    track to prevent wrap
     *              +-------------------------------+
     *              |                               |
     *              |                               v
     * +----+    +====+               +=====+    +----
     * | P1 |---\| RB |/--------------| SB2 |/---| EP3
     * +----+    +====+               +=====+    +----
     *      claim   ^  get               |   waitFor
     *              |                    |
     *           +=====+    +-----+      |
     *           | SB1 |/---| EP1 |/-----+
     *           +=====+    +-----+      |
     *              ^                    |
     *              |       +-----+      |
     *              +-------| EP2 |/-----+
     *             waitFor  +-----+
     *
     * P1  - Publisher 1
     * RB  - RingBuffer
     * SB1 - SequenceBarrier 1
     * EP1 - EventProcessor 1
     * EP2 - EventProcessor 2
     * SB2 - SequenceBarrier 2
     * EP3 - EventProcessor 3
     *
     */
    class OneToThreeDiamondSequencedThroughputTest : public IThroughputTest
    {
    public:
        OneToThreeDiamondSequencedThroughputTest();

        std::int64_t run(Stopwatch& stopwatch) override;

        std::int32_t requiredProcessorCount() const override;

    private:
        const std::int32_t m_bufferSize = 1024 * 8;

#ifdef _DEBUG
        const std::int64_t m_iterations = 100L * 100L * 10L;
#else
        const std::int64_t m_iterations = 1000L * 1000L * 100L;
#endif

        std::int64_t m_expectedResult;

        std::shared_ptr< RingBuffer< FizzBuzzEvent > > m_ringBuffer = RingBuffer< FizzBuzzEvent >::createSingleProducer(FizzBuzzEvent::eventFactory(), m_bufferSize, std::make_shared< YieldingWaitStrategy >());
        std::shared_ptr< BatchEventProcessor< FizzBuzzEvent > > m_batchProcessorFizz;
        std::shared_ptr< BatchEventProcessor< FizzBuzzEvent > > m_batchProcessorBuzz;
        std::shared_ptr< BatchEventProcessor< FizzBuzzEvent > > m_batchProcessorFizzBuzz;
        std::shared_ptr< FizzBuzzEventHandler > m_fizzBuzzHandler;
    };

} // namespace PerfTests
} // namespace Disruptor
