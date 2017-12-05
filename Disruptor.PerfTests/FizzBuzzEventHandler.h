#pragma once

#include "Disruptor/IEventHandler.h"

#include "Disruptor.TestTools/ManualResetEvent.h"

#include "Disruptor.PerfTests/FizzBuzzEvent.h"
#include "Disruptor.PerfTests/FizzBuzzStep.h"
#include "Disruptor.PerfTests/PaddedLong.h"


namespace Disruptor
{
namespace PerfTests
{

    class FizzBuzzEventHandler : public IEventHandler< FizzBuzzEvent >
    {
    public:
        explicit FizzBuzzEventHandler(FizzBuzzStep fizzBuzzStep);

        std::int64_t fizzBuzzCounter() const;

        void reset(const std::shared_ptr< Tests::ManualResetEvent >& latch, std::int64_t iterations);

        void onEvent(FizzBuzzEvent& data, std::int64_t sequence, bool /*endOfBatch*/) override;

    private:
        FizzBuzzStep m_fizzBuzzStep;
        std::int64_t m_iterations = 0;
        std::shared_ptr< Tests::ManualResetEvent > m_latch;
        PaddedLong m_fizzBuzzCounter;
    };

} // namespace PerfTests
} // namespace Disruptor
