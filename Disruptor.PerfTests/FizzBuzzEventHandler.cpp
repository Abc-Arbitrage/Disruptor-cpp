#include "stdafx.h"
#include "FizzBuzzEventHandler.h"


namespace Disruptor
{
namespace PerfTests
{

    FizzBuzzEventHandler::FizzBuzzEventHandler(FizzBuzzStep fizzBuzzStep)
        : m_fizzBuzzStep(fizzBuzzStep)
    {
    }

    std::int64_t FizzBuzzEventHandler::fizzBuzzCounter() const
    {
        return m_fizzBuzzCounter.value;
    }

    void FizzBuzzEventHandler::reset(const std::shared_ptr< Tests::ManualResetEvent >& latch, std::int64_t iterations)
    {
        m_fizzBuzzCounter.value = 0;
        m_iterations = iterations;
        m_latch = latch;
    }

    void FizzBuzzEventHandler::onEvent(FizzBuzzEvent& data, std::int64_t sequence, bool)
    {
        switch (m_fizzBuzzStep)
        {
        case FizzBuzzStep::Fizz:
            data.fizz = data.value % 3 == 0;
            break;

        case FizzBuzzStep::Buzz:
            data.buzz = data.value % 5 == 0;
            break;

        case FizzBuzzStep::FizzBuzz:
            if (data.fizz && data.buzz)
            {
                m_fizzBuzzCounter.value = m_fizzBuzzCounter.value + 1;
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
