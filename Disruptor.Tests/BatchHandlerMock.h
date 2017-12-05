#pragma once

#include <cstdint>
#include <memory>

#include <gmock/gmock.h>

#include "Disruptor/IEventHandler.h"


namespace Disruptor
{
namespace Tests
{

    template <class T>
    class BatchHandlerMock : public IEventHandler< T >
    {
    public:
        MOCK_METHOD3_T(onEvent, void(T& data, std::int64_t sequence, bool endOfBatch));
    };

} // namespace Tests
} // namespace Disruptor
