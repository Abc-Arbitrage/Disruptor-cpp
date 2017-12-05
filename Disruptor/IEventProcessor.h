#pragma once

#include <memory>


namespace Disruptor
{

    class ISequence;


    class IEventProcessor
    {
    public:
        virtual ~IEventProcessor() = default;

        /// <summary>
        /// Return a reference to the <see cref="ISequence"/> being used by this <see cref="IEventProcessor"/>
        /// </summary>
        virtual std::shared_ptr< ISequence > sequence() const = 0;

        /// <summary>
        /// Signal that this <see cref="IEventProcessor"/> should stop when it has finished consuming at the next clean break.
        /// It will call <see cref="ISequenceBarrier.Alert"/> to notify the thread to check status.
        /// </summary>
        virtual void halt() = 0;

        /// <summary>
        /// Starts this instance 
        /// </summary>
        virtual void run() = 0;

        /// <summary>
        /// Gets if the processor is running
        /// </summary>
        virtual bool isRunning() const = 0;
    };

} // namespace Disruptor
