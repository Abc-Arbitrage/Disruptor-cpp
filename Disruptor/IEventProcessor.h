#pragma once

#include <memory>


namespace Disruptor
{

    class ISequence;

    class IEventProcessor
    {
    public:
        virtual ~IEventProcessor() = default;

        /**
         * Return a reference to the ISequence being used by this IEventProcessor
         */ 
        virtual std::shared_ptr< ISequence > sequence() const = 0;

        /**
         * Signal that this IEventProcessor should stop when it has finished consuming at the next clean break.
         * It will call ISequenceBarrier.alert() to notify the thread to check status.
         */ 
        virtual void halt() = 0;

        /**
         * Starts this instance
         */ 
        virtual void run() = 0;

        /**
         * Gets if the processor is running
         */ 
        virtual bool isRunning() const = 0;
    };

} // namespace Disruptor
