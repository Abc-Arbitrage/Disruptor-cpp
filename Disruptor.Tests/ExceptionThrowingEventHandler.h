#pragma once

#include <exception>

#include "Disruptor/IEventHandler.h"

#include "Disruptor.Tests/TestEvent.h"


namespace Disruptor
{
namespace Tests
{

    class ExceptionThrowingEventHandler : public IEventHandler< TestEvent >
    {
    public:
        explicit ExceptionThrowingEventHandler(const std::exception& applicationException);

        void onEvent(TestEvent& data, std::int64_t sequence, bool endOfBatch) override;

    private:
        std::exception m_applicationException;
    };

} // namespace Tests
} // namespace Disruptor
