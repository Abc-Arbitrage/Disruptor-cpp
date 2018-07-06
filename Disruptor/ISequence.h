#pragma once

#include <cstdint>
#include <iosfwd>


namespace Disruptor
{

    class ISequence
    {
    public:
        virtual ~ISequence() = default;

        /**
         * Current sequence number
         */ 
        virtual std::int64_t value() const = 0;

        /**
         * Perform an ordered write of this sequence.  The intent isa Store/Store barrier between this write and any previous store.
         * 
         * \param value The new value for the sequence.
         */ 
        virtual void setValue(std::int64_t value) = 0;

        /**
         * Atomically set the value to the given updated value if the current value == the expected value.
         * 
         * \param expectedSequence the expected value for the sequence
         * \param nextSequence the new value for the sequence
         * \returns true if successful. False return indicates that the actual value was not equal to the expected value.
         */ 
        virtual bool compareAndSet(std::int64_t expectedSequence, std::int64_t nextSequence) = 0;

        /**
         * Increments the sequence and stores the result, as an atomic operation.
         * 
         * \returns incremented sequence
         */ 
        virtual std::int64_t incrementAndGet() = 0;

        /**
         * Increments the sequence and stores the result, as an atomic operation.
         * 
         * \returns incremented sequence
         */ 
        virtual std::int64_t addAndGet(std::int64_t value) = 0;

        virtual void writeDescriptionTo(std::ostream& stream) const = 0;
    };

} // namespace Disruptor
