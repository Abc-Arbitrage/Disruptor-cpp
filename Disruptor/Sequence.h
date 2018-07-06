#pragma once

#include <atomic>

#include "Disruptor/ISequence.h"


namespace Disruptor
{

    /**
     * Concurrent sequence class used for tracking the progress of the ring buffer and event processors. Support a number of concurrent operations including CAS and order writes. 
     * Also attempts to be more efficient with regards to false sharing by adding padding around the volatile field.
     */ 
    class Sequence : public ISequence
    {
    public:
        /**
         * Construct a new sequence counter that can be tracked across threads.
         *
         * \param initialValue initial value for the counter
         */ 
        explicit Sequence(std::int64_t initialValue = InitialCursorValue);

        /**
         * Current sequence number
         */ 
        std::int64_t value() const override;

        /**
         * Perform an ordered write of this sequence.  The intent is a Store/Store barrier between this write and any previous store.
         * 
         * \param value The new value for the sequence.
         */
        void setValue(std::int64_t value) override;

        /**
         * Atomically set the value to the given updated value if the current value == the expected value.
         *
         * \param expectedSequence the expected value for the sequence
         * \param nextSequence the new value for the sequence
         * \returns true if successful. False return indicates that the actual value was not equal to the expected value.
         */ 
        bool compareAndSet(std::int64_t expectedSequence, std::int64_t nextSequence) override;

        /**
         * Increments the sequence and stores the result, as an atomic operation.
         *
         * \returns incremented sequence
         */
        std::int64_t incrementAndGet() override;

        /**
         * Increments the sequence and stores the result, as an atomic operation.
         *
         * \returns incremented sequence
         */ 
        std::int64_t addAndGet(std::int64_t value) override;

        void writeDescriptionTo(std::ostream& stream) const override;

        /**
         * Set to -1 as sequence starting point
         */ 
        static const std::int64_t InitialCursorValue;

    private:
        char m_padding0[56] = {};
        std::atomic< std::int64_t > m_fieldsValue;
        char m_padding1[56] = {};
    };

} // namespace Disruptor
