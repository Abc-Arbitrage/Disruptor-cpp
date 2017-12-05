#pragma once

#include <memory>

#include "Disruptor/IEventHandler.h"

#include "Disruptor.TestTools/ManualResetEvent.h"

#include "Disruptor.PerfTests/FunctionEvent.h"
#include "Disruptor.PerfTests/FunctionStep.h"
#include "Disruptor.PerfTests/PaddedLong.h"


namespace Disruptor
{
namespace PerfTests
{

    class FunctionEventHandler : public IEventHandler< FunctionEvent >
    {
    public:
        explicit FunctionEventHandler(FunctionStep functionStep);

        std::int64_t stepThreeCounter() const;

        void reset(const std::shared_ptr< Tests::ManualResetEvent >& latch, std::int64_t iterations);

        void onEvent(FunctionEvent& data, std::int64_t sequence, bool endOfBatch);

    private:
        FunctionStep m_functionStep;
        PaddedLong m_counter;
        std::int64_t m_iterations{0};
        std::shared_ptr< Tests::ManualResetEvent > m_latch;
    };

} // namespace PerfTests
} // namespace Disruptor
