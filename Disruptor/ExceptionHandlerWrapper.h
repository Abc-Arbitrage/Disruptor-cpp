#pragma once

#include <memory>

#include "Disruptor/IExceptionHandler.h"
#include "Disruptor/FatalExceptionHandler.h"


namespace Disruptor
{

    template <class T>
    class ExceptionHandlerWrapper : public IExceptionHandler< T >
    {
    public:
        void switchTo(const std::shared_ptr< IExceptionHandler< T > >& exceptionHandler)
        {
            m_handler = exceptionHandler;
        }

        void handleEventException(const std::exception& ex, std::int64_t sequence, T& evt) override
        {
            m_handler->handleEventException(ex, sequence, evt);
        }
        
        void handleOnStartException(const std::exception& ex) override
        {
            m_handler->handleOnStartException(ex);
        }
        
        void handleOnShutdownException(const std::exception& ex) override
        {
            m_handler->handleOnShutdownException(ex);
        }

        void handleOnTimeoutException(const std::exception& ex, std::int64_t sequence) override
        {
            m_handler->handleOnTimeoutException(ex, sequence);
        }

    private:
        std::shared_ptr< IExceptionHandler< T > > m_handler = std::make_shared< FatalExceptionHandler< T > >();
    };

} // namespace Disruptor
