#pragma once

#include <cstdint>
#include <iosfwd>


namespace Disruptor
{

    class ISequence
    {
    public:
        virtual ~ISequence() = default;

        /// <summary>
        /// Current sequence number
        /// </summary>
        virtual std::int64_t value() const = 0;

        /// <summary>
        /// Perform an ordered write of this sequence.  The intent is
        /// a Store/Store barrier between this write and any previous
        /// store.
        /// </summary>
        /// <param name="value">The new value for the sequence.</param>
        virtual void setValue(std::int64_t value) = 0;

        /// <summary>
        /// Performs a volatile write of this sequence.  The intent is a Store/Store barrier between this write and any previous
        /// write and a Store/Load barrier between this write and any subsequent volatile read. 
        /// </summary>
        /// <param name="value"></param>
        virtual void setValueVolatile(std::int64_t value) = 0;

        /// <summary>
        /// Atomically set the value to the given updated value if the current value == the expected value.
        /// </summary>
        /// <param name="expectedSequence">the expected value for the sequence</param>
        /// <param name="nextSequence">the new value for the sequence</param>
        /// <returns>true if successful. False return indicates that the actual value was not equal to the expected value.</returns>
        virtual bool compareAndSet(std::int64_t expectedSequence, std::int64_t nextSequence) = 0;

        ///<summary>
        /// Increments the sequence and stores the result, as an atomic operation.
        ///</summary>
        ///<returns>incremented sequence</returns>
        virtual std::int64_t incrementAndGet() = 0;

        ///<summary>
        /// Increments the sequence and stores the result, as an atomic operation.
        ///</summary>
        ///<returns>incremented sequence</returns>
        virtual std::int64_t addAndGet(std::int64_t value) = 0;

        virtual void writeDescriptionTo(std::ostream& stream) const = 0;
    };

} // namespace Disruptor
