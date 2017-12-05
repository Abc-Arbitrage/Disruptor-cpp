#pragma once

#include <cstdint>
#include <memory>

#include <gmock/gmock.h>

#include "Disruptor/IEventHandler.h"
#include "Disruptor/ILifecycleAware.h"


namespace Disruptor
{
namespace Tests
{

    template <class T>
    class LifecycleAwareEventHandlerMock : public IEventHandler< T >, public ILifecycleAware
    {
    public:
        MOCK_METHOD3_T(onEvent, void(T& data, std::int64_t sequence, bool endOfBatch));
        MOCK_METHOD0(onStart, void());
        MOCK_METHOD0(onShutdown, void());
    };

} // namespace Tests
} // namespace Disruptor
