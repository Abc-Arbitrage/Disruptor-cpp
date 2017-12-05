#pragma once

#include "Disruptor/IExceptionHandler.h"

#include "Disruptor.Tests/AtomicReference.h"


namespace Disruptor
{
namespace Tests
{

    template <class T>
    class StubExceptionHandler : public IExceptionHandler< T >
    {
    public:
        explicit StubExceptionHandler(const std::shared_ptr< AtomicReference< std::exception > >& exceptionHandled)
            : m_exceptionHandled(exceptionHandled)
        {
        }

        void handleEventException(const std::exception& ex, std::int64_t /*sequence*/, T& /*evt*/) override
        {
            m_exceptionHandled->write(ex);
        }

        void handleOnStartException(const std::exception& ex) override
        {
            m_exceptionHandled->write(ex);
        }

        void handleOnShutdownException(const std::exception& ex) override
        {
            m_exceptionHandled->write(ex);
        }

        void handleOnTimeoutException(const std::exception& ex, std::int64_t /*sequence*/) override
        {
            m_exceptionHandled->write(ex);
        }

    private:
        std::shared_ptr< AtomicReference< std::exception > > m_exceptionHandled;
    };

} // namespace Tests
} // namespace Disruptor
