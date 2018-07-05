#pragma once

#include <cstdint>
#include <memory>

#include <gmock/gmock.h>

#include "Disruptor/IWaitStrategy.h"


namespace Disruptor
{
namespace Tests
{

    class WaitStrategyMock : public IWaitStrategy
    {
    public:
        MOCK_METHOD4(waitFor, std::int64_t(std::int64_t sequence, Sequence& cursor, ISequence& dependentSequence, ISequenceBarrier& barrier));

        MOCK_METHOD0(signalAllWhenBlocking, void());
        
        MOCK_CONST_METHOD1(writeDescriptionTo, void(std::ostream& stream));
    };

} // namespace Tests
} // namespace Disruptor
