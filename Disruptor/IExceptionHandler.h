#pragma once

#include <cstdint>
#include <exception>


namespace Disruptor
{

    /// <summary>
    /// Callback handler for uncaught exceptions in the event processing cycle of the <see cref="BatchEventProcessor{T}"/>
    /// </summary>
    template <class T>
    class IExceptionHandler
    {
    public:
        virtual ~IExceptionHandler() = default;

        /// <summary>
        /// Strategy for handling uncaught exceptions when processing an event.
        /// 
        /// If the strategy wishes to terminate further processing by the <see cref="BatchEventProcessor{T}"/>
        /// then it should throw a <see cref="ApplicationException"/>
        /// </summary>
        /// <param name="ex">exception that propagated from the <see cref="IEventHandler{T}"/>.</param>
        /// <param name="sequence">sequence of the event which cause the exception.</param>
        /// <param name="evt">event being processed when the exception occurred. This can be null</param>
        virtual void handleEventException(const std::exception& ex, std::int64_t sequence, T& evt) = 0;

        /// <summary>
        /// Callback to notify of an exception during <see cref="ILifecycleAware.OnStart"/>
        /// </summary>
        /// <param name="ex">ex throw during the starting process.</param>
        virtual void handleOnStartException(const std::exception& ex) = 0;

        /// <summary>
        /// Callback to notify of an exception during <see cref="ILifecycleAware.OnShutdown"/>
        /// </summary>
        /// <param name="ex">ex throw during the shutdown process.</param>
        virtual void handleOnShutdownException(const std::exception& ex) = 0;

        /// <summary>
        /// Callback to notify of an exception during <see cref="ITimeoutHandler.OnTimeout"/>
        /// </summary>
        /// <param name="ex">ex throw during the starting process.</param>
        /// <param name="sequence">sequence of the event which cause the exception.</param>
        virtual void handleOnTimeoutException(const std::exception& ex, std::int64_t sequence) = 0;
    };

} // namespace Disruptor
