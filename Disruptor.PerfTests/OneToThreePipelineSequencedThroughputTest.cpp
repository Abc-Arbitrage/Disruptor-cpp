#include "stdafx.h"
#include "OneToThreePipelineSequencedThroughputTest.h"

#include "PerfTestUtil.h"


namespace Disruptor
{
namespace PerfTests
{

    OneToThreePipelineSequencedThroughputTest::OneToThreePipelineSequencedThroughputTest()
    {
        auto stepOneFunctionHandler = std::make_shared< FunctionEventHandler >(FunctionStep::One);
        auto stepTwoFunctionHandler = std::make_shared< FunctionEventHandler >(FunctionStep::Two);
        m_stepThreeFunctionHandler = std::make_shared< FunctionEventHandler >(FunctionStep::Three);

        auto stepOneSequenceBarrier = m_ringBuffer->newBarrier();
        m_stepOneBatchProcessor = std::make_shared< BatchEventProcessor< FunctionEvent > >(m_ringBuffer, stepOneSequenceBarrier, stepOneFunctionHandler);

        auto stepTwoSequenceBarrier = m_ringBuffer->newBarrier({ m_stepOneBatchProcessor->sequence() });
        m_stepTwoBatchProcessor = std::make_shared< BatchEventProcessor< FunctionEvent > >(m_ringBuffer, stepTwoSequenceBarrier, stepTwoFunctionHandler);

        auto stepThreeSequenceBarrier = m_ringBuffer->newBarrier({ m_stepTwoBatchProcessor->sequence() });
        m_stepThreeBatchProcessor = std::make_shared< BatchEventProcessor< FunctionEvent > >(m_ringBuffer, stepThreeSequenceBarrier, m_stepThreeFunctionHandler);

        std::int64_t temp = 0;
        auto operandTwo = m_operandTwoInitialValue;

        for (std::int64_t i = 0; i < m_iterations; ++i)
        {
            auto stepOneResult = i + operandTwo--;
            auto stepTwoResult = stepOneResult + 3;

            if ((stepTwoResult & 4L) == 4L)
            {
                ++temp;
            }
        }
        m_expectedResult = temp;

        m_ringBuffer->addGatingSequences({ m_stepThreeBatchProcessor->sequence() });
    }

    std::int64_t OneToThreePipelineSequencedThroughputTest::run(Stopwatch& stopwatch)
    {
        auto latch = std::make_shared< Tests::ManualResetEvent >(false);
        m_stepThreeFunctionHandler->reset(latch, m_stepThreeBatchProcessor->sequence()->value() + m_iterations);

        auto processorTask1 = m_executor->submit(m_stepOneBatchProcessor);
        auto processorTask2 = m_executor->submit(m_stepTwoBatchProcessor);
        auto processorTask3 = m_executor->submit(m_stepThreeBatchProcessor);

        auto& rb = *m_ringBuffer;

        stopwatch.start();

        auto operandTwo = m_operandTwoInitialValue;
        for (std::int64_t i = 0; i < m_iterations; ++i)
        {
            auto sequence = rb.next();
            auto& event = rb[sequence];
            event.operandOne = i;
            event.operandTwo = operandTwo--;
            rb.publish(sequence);
        }

        latch->waitOne();
        stopwatch.stop();

        m_stepOneBatchProcessor->halt();
        m_stepTwoBatchProcessor->halt();
        m_stepThreeBatchProcessor->halt();

        processorTask1.wait();
        processorTask2.wait();
        processorTask3.wait();

        PerfTestUtil::failIfNot(m_expectedResult, m_stepThreeFunctionHandler->stepThreeCounter());

        return m_iterations;
    }

    std::int32_t OneToThreePipelineSequencedThroughputTest::requiredProcessorCount() const
    {
        return 4;
    }

} // namespace PerfTests
} // namespace Disruptor
