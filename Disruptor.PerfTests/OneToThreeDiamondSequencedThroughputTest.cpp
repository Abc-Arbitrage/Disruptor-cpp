#include "stdafx.h"
#include "OneToThreeDiamondSequencedThroughputTest.h"

#include "PerfTestUtil.h"


namespace Disruptor
{
namespace PerfTests
{

    OneToThreeDiamondSequencedThroughputTest::OneToThreeDiamondSequencedThroughputTest()
    {
        auto sequenceBarrier = m_ringBuffer->newBarrier();

        auto fizzHandler = std::make_shared< FizzBuzzEventHandler >(FizzBuzzStep::Fizz);
        m_batchProcessorFizz = std::make_shared< BatchEventProcessor< FizzBuzzEvent > >(m_ringBuffer, sequenceBarrier, fizzHandler);

        auto buzzHandler = std::make_shared< FizzBuzzEventHandler >(FizzBuzzStep::Buzz);
        m_batchProcessorBuzz = std::make_shared< BatchEventProcessor< FizzBuzzEvent > >(m_ringBuffer, sequenceBarrier, buzzHandler);

        auto sequenceBarrierFizzBuzz = m_ringBuffer->newBarrier({ m_batchProcessorFizz->sequence(), m_batchProcessorBuzz->sequence() });

        m_fizzBuzzHandler = std::make_shared< FizzBuzzEventHandler >(FizzBuzzStep::FizzBuzz);
        m_batchProcessorFizzBuzz = std::make_shared< BatchEventProcessor< FizzBuzzEvent > >(m_ringBuffer, sequenceBarrierFizzBuzz, m_fizzBuzzHandler);

        std::int64_t temp = 0;
        for (std::int64_t i = 0; i < m_iterations; ++i)
        {
            auto fizz = 0 == (i % 3L);
            auto buzz = 0 == (i % 5L);

            if (fizz && buzz)
            {
                ++temp;
            }
        }
        m_expectedResult = temp;

        m_ringBuffer->addGatingSequences({ m_batchProcessorFizzBuzz->sequence() });
    }

    std::int64_t OneToThreeDiamondSequencedThroughputTest::run(Stopwatch& stopwatch)
    {
        auto latch = std::make_shared< Tests::ManualResetEvent >(false);
        m_fizzBuzzHandler->reset(latch, m_batchProcessorFizzBuzz->sequence()->value() + m_iterations);

        auto processorTask1 = std::async(std::launch::async, [this] { m_batchProcessorFizz->run(); });
        auto processorTask2 = std::async(std::launch::async, [this] { m_batchProcessorBuzz->run(); });
        auto processorTask3 = std::async(std::launch::async, [this] { m_batchProcessorFizzBuzz->run(); });

        stopwatch.start();

        auto& rb = *m_ringBuffer;
        for (std::int64_t i = 0; i < m_iterations; ++i)
        {
            auto sequence = rb.next();
            rb[sequence].value = i;
            rb.publish(sequence);
        }

        latch->waitOne();
        stopwatch.stop();

        m_batchProcessorFizz->halt();
        m_batchProcessorBuzz->halt();
        m_batchProcessorFizzBuzz->halt();
        
        processorTask1.wait();
        processorTask2.wait();
        processorTask3.wait();

        PerfTestUtil::failIfNot(m_expectedResult, m_fizzBuzzHandler->fizzBuzzCounter());

        return m_iterations;
    }

    std::int32_t OneToThreeDiamondSequencedThroughputTest::requiredProcessorCount() const
    {
        return 4;
    }

} // namespace PerfTests
} // namespace Disruptor
