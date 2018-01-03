#pragma once

#include <iostream>
#include <sstream>

#include "Disruptor/IExceptionHandler.h"
#include "Disruptor/TypeInfo.h"


namespace Disruptor
{

    /// <summary>
    /// Convenience implementation of an exception handler that using Console.WriteLine to log the exception
    /// </summary>
    template <class T>
    class IgnoreExceptionHandler : public IExceptionHandler< T >
    {
    public:
        /// <summary>
        /// Strategy for handling uncaught exceptions when processing an event.
        /// </summary>
        /// <param name="ex">exception that propagated from the <see cref="IEventHandler{T}"/>.</param>
        /// <param name="sequence">sequence of the event which cause the exception.</param>
        /// <param name="evt">event being processed when the exception occurred.</param>
        void handleEventException(const std::exception& ex, std::int64_t sequence, T& /*evt*/) override
        {
            std::stringstream stream;
            stream << "Exception processing sequence " << sequence << " for event " << Utils::getMetaTypeInfo< T >().fullyQualifiedName() << ": " << ex.what();

            std::cerr << stream.str() << std::endl;
        }

        /// <summary>
        /// Callback to notify of an exception during <see cref="ILifecycleAware.OnStart"/>
        /// </summary>
        /// <param name="ex">ex throw during the starting process.</param>
        void handleOnStartException(const std::exception& ex) override
        {
            std::stringstream stream;
            stream << "Exception during OnStart(): " << ex.what();

            std::cerr << stream.str() << std::endl;
        }

        /// <summary>
        /// Callback to notify of an exception during <see cref="ILifecycleAware.OnShutdown"/>
        /// </summary>
        /// <param name="ex">ex throw during the shutdown process.</param>
        void handleOnShutdownException(const std::exception& ex) override
        {
            std::stringstream stream;
            stream << "Exception during OnShutdown(): " << ex.what();

            std::cerr << stream.str() << std::endl;
        }

        /// <summary>
        /// Callback to notify of an exception during <see cref="ITimeoutHandler.OnTimeout"/>
        /// </summary>
        /// <param name="ex">ex throw during the starting process.</param>
        /// <param name="sequence">sequence of the event which cause the exception.</param>
        void handleOnTimeoutException(const std::exception& ex, std::int64_t sequence) override
        {
            std::stringstream stream;
            stream << "Exception during OnTimeout() processing sequence " << sequence << " for event " << Utils::getMetaTypeInfo< T >().fullyQualifiedName() << ": " << ex.what();

            std::cerr << stream.str() << std::endl;
        }
    };

} // namespace Disruptor
