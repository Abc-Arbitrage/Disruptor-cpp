#include "stdafx.h"
#include "ExceptionThrowingEventHandler.h"


namespace Disruptor
{
namespace Tests
{

    ExceptionThrowingEventHandler::ExceptionThrowingEventHandler(const std::exception& applicationException)
        : m_applicationException(applicationException)
    {
    }

    void ExceptionThrowingEventHandler::onEvent(TestEvent& /*data*/, std::int64_t /*sequence*/, bool /*endOfBatch*/)
    {
        throw m_applicationException;
    }

} // namespace Tests
} // namespace Disruptor
