#pragma once

#include <memory>

#include "Disruptor/IEventHandler.h"

#include "Disruptor.TestTools/ManualResetEvent.h"

#include "Disruptor.PerfTests/PaddedLong.h"
#include "Disruptor.PerfTests/ValueEvent.h"


namespace Disruptor
{
namespace PerfTests
{

    class ValueAdditionEventHandler : public IEventHandler< ValueEvent >
    {
    public:
        void reset(const std::shared_ptr< Tests::ManualResetEvent >& latch, std::int64_t expectedCount);

        void onEvent(ValueEvent& value, std::int64_t sequence, bool /*endOfBatch*/) override;

        std::int64_t count() const;

        std::int64_t value() const;

    private:
        PaddedLong m_value;
        std::int64_t m_count = 0;
        std::shared_ptr< Tests::ManualResetEvent > m_latch;
    };

} // namespace PerfTests
} // namespace Disruptor
