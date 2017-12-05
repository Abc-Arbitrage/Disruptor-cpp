#pragma once

#include <cstdint>


namespace Disruptor
{

    /// <summary>
    /// Coordination barrier for tracking the cursor for producers and sequence of
    /// dependent <see cref="IEventProcessor"/>s for a <see cref="RingBuffer{T}"/>
    /// </summary>
    class ISequenceBarrier
    {
    public:
        virtual ~ISequenceBarrier() = default;

        /// <summary>
        /// Wait for the given sequence to be available for consumption.
        /// </summary>
        /// <param name="sequence">sequence to wait for</param>
        /// <returns>the sequence up to which is available</returns>
        /// <exception cref="AlertException">if a status change has occurred for the Disruptor</exception>
        virtual std::int64_t waitFor(std::int64_t sequence) = 0;

        /// <summary>
        /// Delegate a call to the <see cref="Sequencer.Cursor"/>
        /// Returns the value of the cursor for events that have been published.
        /// </summary>
        virtual std::int64_t cursor() = 0;

        /// <summary>
        /// The current alert status for the barrier.
        /// Returns true if in alert otherwise false.
        /// </summary>
        virtual bool isAlerted() = 0;

        /// <summary>
        ///  Alert the <see cref="IEventProcessor"/> of a status change and stay in this status until cleared.
        /// </summary>
        virtual void alert() = 0;

        /// <summary>
        /// Clear the current alert status.
        /// </summary>
        virtual void clearAlert() = 0;

        /// <summary>
        /// Check if an alert has been raised and throw an <see cref="AlertException"/> if it has.
        /// </summary>
        /// <exception cref="AlertException">AlertException if alert has been raised.</exception>
        virtual void checkAlert() = 0;
    };

} // namespace Disruptor
