#pragma once

#include <cstdint>


namespace Disruptor
{

    /**
     * Coordination barrier for tracking the cursor for producers and sequence of dependent IEventProcessor's for a RingBuffer<T>
     */ 
    class ISequenceBarrier
    {
    public:
        virtual ~ISequenceBarrier() = default;

        /**
         * Wait for the given sequence to be available for consumption.
         * 
         * \param sequence sequence to wait for
         * \returns the sequence up to which is available
         */ 
        virtual std::int64_t waitFor(std::int64_t sequence) = 0;

        /**
         * Delegate a call to the Sequencer.cursor()
         * Returns the value of the cursor for events that have been published.
         */ 
        virtual std::int64_t cursor() = 0;

        /**
         * The current alert status for the barrier. Returns true if in alert otherwise false.
         */ 
        virtual bool isAlerted() = 0;

        /**
         * Alert the IEventProcessor of a status change and stay in this status until cleared.
         */ 
        virtual void alert() = 0;

        /**
         * Clear the current alert status.
         */ 
        virtual void clearAlert() = 0;

        /**
         * Check if an alert has been raised and throw an AlertException if it has.
         */ 
        virtual void checkAlert() = 0;
    };

} // namespace Disruptor
