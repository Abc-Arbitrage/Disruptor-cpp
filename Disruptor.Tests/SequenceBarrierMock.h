#pragma once

#include <gmock/gmock.h>

#include "Disruptor/ISequenceBarrier.h"


namespace Disruptor
{
namespace Tests
{

    class SequenceBarrierMock : public ISequenceBarrier
    {
    public:
        MOCK_METHOD1(waitFor, std::int64_t(std::int64_t sequence));
        
        MOCK_METHOD0(cursor, std::int64_t());

        MOCK_METHOD0(isAlerted, bool());
        MOCK_METHOD0(alert, void());

        MOCK_METHOD0(clearAlert, void());
        MOCK_METHOD0(checkAlert, void());
    };

} // namespace Tests
} // namespace Disruptor
