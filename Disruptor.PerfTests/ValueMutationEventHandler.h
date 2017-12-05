#pragma once

#include <boost/thread/barrier.hpp>

#include "Disruptor/IEventHandler.h"

#include "Disruptor.PerfTests/Operation.h"
#include "Disruptor.PerfTests/PaddedLong.h"
#include "Disruptor.PerfTests/ValueEvent.h"


namespace Disruptor
{
namespace PerfTests
{

    class ValueMutationEventHandler : public IEventHandler< ValueEvent >
    {
    public:
        explicit ValueMutationEventHandler(Operation operation);

        std::int64_t value() const;

        void reset(const std::shared_ptr< boost::barrier >& latch, std::int64_t expectedCount);

        void onEvent(ValueEvent& data, std::int64_t sequence, bool endOfBatch);

    private:
        Operation m_operation;
        PaddedLong m_value;
        std::int64_t m_iterations = 0;
        std::shared_ptr< boost::barrier > m_latch;
    };

} // namespace PerfTests
} // namespace Disruptor
