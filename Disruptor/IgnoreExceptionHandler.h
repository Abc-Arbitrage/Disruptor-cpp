#pragma once

#include <iostream>
#include <sstream>

#include "Disruptor/IExceptionHandler.h"
#include "Disruptor/TypeInfo.h"


namespace Disruptor
{

    /**
     * Convenience implementation of an exception handler that using Console.WriteLine to log the exception
     */ 
    template <class T>
    class IgnoreExceptionHandler : public IExceptionHandler< T >
    {
    public:
        /**
         * Strategy for handling uncaught exceptions when processing an event.
         * 
         * \param ex exception that propagated from the IEventHandler<T>
         * \param sequence sequence of the event which cause the exception.
         * \param evt event being processed when the exception occurred.
         */ 
        void handleEventException(const std::exception& ex, std::int64_t sequence, T& /*evt*/) override
        {
            std::stringstream stream;
            stream << "Exception processing sequence " << sequence << " for event " << Utils::getMetaTypeInfo< T >().fullyQualifiedName() << ": " << ex.what();

            std::cerr << stream.str() << std::endl;
        }

        /**
         * Callback to notify of an exception during ILifecycleAware.onStart()
         *
         * \param ex ex throw during the starting process.
         */ 
        void handleOnStartException(const std::exception& ex) override
        {
            std::stringstream stream;
            stream << "Exception during OnStart(): " << ex.what();

            std::cerr << stream.str() << std::endl;
        }

        /**
         * Callback to notify of an exception during ILifecycleAware.onShutdown()
         *
         * \param ex ex throw during the shutdown process.
         */
        void handleOnShutdownException(const std::exception& ex) override
        {
            std::stringstream stream;
            stream << "Exception during OnShutdown(): " << ex.what();

            std::cerr << stream.str() << std::endl;
        }

        /**
         * Callback to notify of an exception during ITimeoutHandler.onTimeout()
         *
         * \param ex ex throw during the starting process.
         * \param sequence sequence of the event which cause the exception.
         */ 
        void handleOnTimeoutException(const std::exception& ex, std::int64_t sequence) override
        {
            std::stringstream stream;
            stream << "Exception during OnTimeout() processing sequence " << sequence << " for event " << Utils::getMetaTypeInfo< T >().fullyQualifiedName() << ": " << ex.what();

            std::cerr << stream.str() << std::endl;
        }
    };

} // namespace Disruptor
