#pragma once

#include <cstdint>
#include <exception>


namespace Disruptor
{

    /**
     * Callback handler for uncaught exceptions in the event processing cycle of the BatchEventProcessor<T>
     */
    template <class T>
    class IExceptionHandler
    {
    public:
        virtual ~IExceptionHandler() = default;

        /**
         * Strategy for handling uncaught exceptions when processing an event. If the strategy wishes to terminate further processing by the BatchEventProcessor<T>
         * then it should throw a ApplicationException
         *
         * \param ex exception that propagated from the IEventHandler<T>
         * \param sequence sequence of the event which cause the exception.
         * \param evt event being processed when the exception occurred
         */ 
        virtual void handleEventException(const std::exception& ex, std::int64_t sequence, T& evt) = 0;

        /**
         * Callback to notify of an exception during ILifecycleAware.onStart()
         *
         * \param ex ex throw during the starting process.
         */ 
        virtual void handleOnStartException(const std::exception& ex) = 0;

        /**
         * Callback to notify of an exception during ILifecycleAware.onShutdown()
         *
         * \param ex ex throw during the shutdown process.
         */
        virtual void handleOnShutdownException(const std::exception& ex) = 0;

        /**
         * Callback to notify of an exception during ITimeoutHandler.onTimeout()
         *
         * \param ex ex throw during the starting process.
         * \param sequence sequence of the event which cause the exception.
         */
        virtual void handleOnTimeoutException(const std::exception& ex, std::int64_t sequence) = 0;
    };

} // namespace Disruptor
