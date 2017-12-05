#pragma once

#include <atomic>

#include "Disruptor/ISequence.h"


namespace Disruptor
{

    /// <summary>
    /// <p>Concurrent sequence class used for tracking the progress of
    /// the ring buffer and event processors.Support a number
    /// of concurrent operations including CAS and order writes.</p>
    ///
    /// <p>Also attempts to be more efficient with regards to false
    /// sharing by adding padding around the volatile field.</p>
    /// </summary>
    class Sequence : public ISequence
    {
    public:
        /// <summary>
        /// Construct a new sequence counter that can be tracked across threads.
        /// </summary>
        /// <param name="initialValue">initial value for the counter</param>
        explicit Sequence(std::int64_t initialValue = InitialCursorValue);

        /// <summary>
        /// Current sequence number
        /// </summary>
        std::int64_t value() const override;

        /// <summary>
        /// Perform an ordered write of this sequence.  The intent is
        /// a Store/Store barrier between this write and any previous
        /// store.
        /// </summary>
        /// <param name="value">The new value for the sequence.</param>
        void setValue(std::int64_t value) override;

        /// <summary>
        /// Performs a volatile write of this sequence.  The intent is a Store/Store barrier between this write and any previous
        /// write and a Store/Load barrier between this write and any subsequent volatile read. 
        /// </summary>
        /// <param name="value"></param>
        void setValueVolatile(std::int64_t value) override;

        /// <summary>
        /// Atomically set the value to the given updated value if the current value == the expected value.
        /// </summary>
        /// <param name="expectedSequence">the expected value for the sequence</param>
        /// <param name="nextSequence">the new value for the sequence</param>
        /// <returns>true if successful. False return indicates that the actual value was not equal to the expected value.</returns>
        bool compareAndSet(std::int64_t expectedSequence, std::int64_t nextSequence) override;

        ///<summary>
        /// Increments the sequence and stores the result, as an atomic operation.
        ///</summary>
        ///<returns>incremented sequence</returns>
        std::int64_t incrementAndGet() override;

        ///<summary>
        /// Increments the sequence and stores the result, as an atomic operation.
        ///</summary>
        ///<returns>incremented sequence</returns>
        std::int64_t addAndGet(std::int64_t value) override;

        void writeDescriptionTo(std::ostream& stream) const override;

        /// <summary>
        /// Set to -1 as sequence starting point
        /// </summary>
        static const std::int64_t InitialCursorValue;

    private:
        char padding0[56];
        std::atomic< std::int64_t > m_fieldsValue;
        char padding1[56];
    };

} // namespace Disruptor
