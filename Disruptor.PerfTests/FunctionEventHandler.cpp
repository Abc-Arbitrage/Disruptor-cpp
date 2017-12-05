#include "stdafx.h"
#include "FunctionEventHandler.h"


namespace Disruptor
{
namespace PerfTests
{

    FunctionEventHandler::FunctionEventHandler(FunctionStep functionStep)
        : m_functionStep(functionStep)
    {
    }

    std::int64_t FunctionEventHandler::stepThreeCounter() const
    {
        return m_counter.value;
    }

    void FunctionEventHandler::reset(const std::shared_ptr< Tests::ManualResetEvent >& latch, std::int64_t iterations)
    {
        m_counter.value = 0;
        m_iterations = iterations;
        m_latch = latch;
    }

    void FunctionEventHandler::onEvent(FunctionEvent& data, std::int64_t sequence, bool /*endOfBatch*/)
    {
        switch (m_functionStep)
        {
        case FunctionStep::One:
            data.stepOneResult = data.operandOne + data.operandTwo;
            break;

        case FunctionStep::Two:
            data.stepTwoResult = data.stepOneResult + 3L;
            break;

        case FunctionStep::Three:
            if ((data.stepTwoResult & 4L) == 4L)
            {
                m_counter.value = m_counter.value + 1;
            }
            break;
        }

        if (sequence == m_iterations - 1)
        {
            m_latch->set();
        }
    }

} // namespace PerfTests
} // namespace Disruptor
