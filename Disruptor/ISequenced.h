#pragma once

#include <cstdint>


namespace Disruptor
{

    /// <summary>
    /// 
    /// </summary>
    class ISequenced
    {
    public:
        virtual ~ISequenced() = default;

        /// <summary>
        /// The capacity of the data structure to hold entries.
        /// </summary>
        virtual std::int32_t bufferSize() = 0;

        /// <summary>
        /// Has the buffer got capacity to allocate another sequence.  This is a concurrent
        /// method so the response should only be taken as an indication of available capacity.
        /// </summary>
        /// <param name="requiredCapacity">requiredCapacity in the buffer</param>
        /// <returns>true if the buffer has the capacity to allocate the next sequence otherwise false.</returns>
        virtual bool hasAvailableCapacity(std::int32_t requiredCapacity) = 0;

        /// <summary>
        /// Get the remaining capacity for this sequencer. return The number of slots remaining.
        /// </summary>
        virtual std::int64_t getRemainingCapacity() = 0;

        /// <summary>
        /// Claim the next event in sequence for publishing.
        /// </summary>
        /// <returns>the claimed sequence value</returns>
        virtual std::int64_t next() = 0;

        /// <summary>
        /// Claim the next n events in sequence for publishing.  This is for batch event producing.  Using batch producing requires a little care and some math.
        /// <code>
        ///     int n = 10;
        ///     long hi = sequencer.next(n);
        ///     long lo = hi - (n - 1);
        ///     for (long sequence = lo; sequence &lt;= hi; sequence++) {
        ///        // Do work.
        ///     }
        ///     sequencer.publish(lo, hi);
        /// </code>
        /// </summary>
        /// <param name="n">the number of sequences to claim</param>
        /// <returns>the highest claimed sequence value</returns>
        virtual std::int64_t next(std::int32_t n) = 0;

        /// <summary>
        /// Attempt to claim the next event in sequence for publishing.  Will return the number of the slot if there is at least<code>requiredCapacity</code> slots available.
        /// </summary>
        /// <returns>the claimed sequence value</returns>
        virtual std::int64_t tryNext() = 0;

        /// <summary>
        /// Attempt to claim the next n events in sequence for publishing.  Will return the highest numbered slot if there is at least &lt;code&gt;requiredCapacity&lt;/code&gt; slots
        /// available.  Have a look at <see cref="Next"/> for a description on how to use this method.
        ///  </summary>
        /// <param name="n">the number of sequences to claim</param>
        /// <returns>the claimed sequence value</returns>
        virtual std::int64_t tryNext(std::int32_t n) = 0;

        /// <summary>
        /// Publishes a sequence. Call when the event has been filled.
        /// </summary>
        /// <param name="sequence"></param>
        virtual void publish(std::int64_t sequence) = 0;

        /// <summary>
        /// Batch publish sequences.  Called when all of the events have been filled.
        /// </summary>
        /// <param name="lo">first sequence number to publish</param>
        /// <param name="hi">last sequence number to publish</param>
        virtual void publish(std::int64_t lo, std::int64_t hi) = 0;
    };

} // namespace Disruptor
