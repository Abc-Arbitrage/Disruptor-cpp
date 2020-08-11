#pragma once

#include <atomic>

#include "Disruptor/InsufficientCapacityException.h"
#include "Disruptor/IWaitStrategy.h"
#include "Disruptor/Sequencer.h"
#include "Disruptor/SpinWait.h"
#include "Disruptor/Util.h"


namespace Disruptor
{

    template <class T>
    class SingleProducerSequencer : public Sequencer< T >
    {
        struct Fields
        {
            char padding0[56];
            std::int64_t nextValue;
            std::int64_t cachedValue;
            char padding1[56];

            Fields(std::int64_t nextValue, std::int64_t cachedValue)
                : nextValue(nextValue)
                , cachedValue(cachedValue)
            {}
        };

    public:
        SingleProducerSequencer(std::int32_t bufferSize, const std::shared_ptr< IWaitStrategy >& waitStrategy)
            : Sequencer< T >(bufferSize, waitStrategy)
            , m_fields(Sequence::InitialCursorValue, Sequence::InitialCursorValue)
        {}

        /**
         * Has the buffer got capacity to allocate another sequence.  This is a concurrent method so the response should only be taken as an indication of available capacity.
         * 
         * \param requiredCapacity requiredCapacity in the buffer
         * \returns true if the buffer has the capacity to allocate the next sequence otherwise false.
         */ 
        bool hasAvailableCapacity(int requiredCapacity) override
        {
            std::int64_t nextValue = m_fields.nextValue;

            std::int64_t wrapPoint = (nextValue + requiredCapacity) - this->m_bufferSize;
            std::int64_t cachedGatingSequence = m_fields.cachedValue;

            if (wrapPoint > cachedGatingSequence || cachedGatingSequence > nextValue)
            {
                auto minSequence = Util::getMinimumSequence(this->m_gatingSequences, nextValue);
                m_fields.cachedValue = minSequence;

                if (wrapPoint > minSequence)
                {
                    return false;
                }
            }

            return true;
        }

        /**
         * Claim the next event in sequence for publishing.
         */ 
        std::int64_t next() override
        {
            return next(1);
        }

        /**
         * Claim the next n events in sequence for publishing. This is for batch event producing. Using batch producing requires a little care and some math.
         * <code>
         *   int n = 10;
         *   long hi = sequencer.next(n);
         *   long lo = hi - (n - 1);
         *   for (long sequence = lo; sequence &lt;= hi; sequence++) {
         *   // Do work.
         *    }
         *   sequencer.publish(lo, hi);
         * </code>
         * 
         * \param n the number of sequences to claim
         * \returns the highest claimed sequence value
         */ 
        std::int64_t next(std::int32_t n) override
        {
            if (n < 1 || n > this->m_bufferSize)
            {
                DISRUPTOR_THROW_ARGUMENT_EXCEPTION("n must be > 0 and < bufferSize");
            }

            auto nextValue = m_fields.nextValue;

            auto nextSequence = nextValue + n;
            auto wrapPoint = nextSequence - this->m_bufferSize;
            auto cachedGatingSequence = m_fields.cachedValue;

            if (wrapPoint > cachedGatingSequence || cachedGatingSequence > nextValue)
            {
                this->m_cursor->setValue(nextValue);

                SpinWait spinWait;
                std::int64_t minSequence;
                while (wrapPoint > (minSequence = Util::getMinimumSequence(this->m_gatingSequences, nextValue)))
                {
                    this->m_waitStrategyRef.signalAllWhenBlocking();
                    spinWait.spinOnce();
                }

                m_fields.cachedValue = minSequence;
            }

            m_fields.nextValue = nextSequence;

            return nextSequence;
        }
        
        /**
         * Attempt to claim the next event in sequence for publishing. Will return the number of the slot if there is at least requiredCapacity slots available.
         *
         * \returns the claimed sequence value
         */
        std::int64_t tryNext() override
        {
            return tryNext(1);
        }

        /**
         * Attempt to claim the next event in sequence for publishing. Will return the number of the slot if there is at least availableCapacity slots available.
         * 
         * \param n the number of sequences to claim
         * \returns the claimed sequence value
         */ 
        std::int64_t tryNext(std::int32_t n) override
        {
            if (n < 1)
            {
                DISRUPTOR_THROW_ARGUMENT_EXCEPTION("n must be > 0");
            }

            if (!hasAvailableCapacity(n))
            {
                DISRUPTOR_THROW_INSUFFICIENT_CAPACITY_EXCEPTION();
            }

            auto nextSequence = m_fields.nextValue + n;
            m_fields.nextValue = nextSequence;

            return nextSequence;
        }

        /**
         * Get the remaining capacity for this sequencer. return The number of slots remaining.
         */ 
        std::int64_t getRemainingCapacity() override
        {
            auto nextValue = m_fields.nextValue;

            auto consumed = Util::getMinimumSequence(this->m_gatingSequences, nextValue);
            auto produced = nextValue;

            return this->bufferSize() - (produced - consumed);
        }

        /**
         * Claim a specific sequence when only one publisher is involved.
         * 
         * \param sequence sequence to be claimed.
         */ 
        void claim(std::int64_t sequence) override
        {
            m_fields.nextValue = sequence;
        }

        /**
         * Publish an event and make it visible to IEventProcessors
         * 
         * \param sequence sequence to be published
         */ 
        void publish(std::int64_t sequence) override
        {
            this->m_cursorRef.setValue(sequence);
            this->m_waitStrategyRef.signalAllWhenBlocking();
        }

        /**
         * Batch publish sequences.  Called when all of the events have been filled.
         *
         * \param lo first sequence number to publish
         * \param hi last sequence number to publish
         */ 
        void publish(std::int64_t /*lo*/, std::int64_t hi) override
        {
            publish(hi);
        }

        /**
         * Confirms if a sequence is published and the event is available for use; non-blocking.
         * 
         * \param sequence sequence of the buffer to check
         * \returns true if the sequence is available for use, false if not
         */ 
        bool isAvailable(std::int64_t sequence) override
        {
            return sequence <= this->m_cursorRef.value();
        }

        /**
         * Get the highest sequence number that can be safely read from the ring buffer. Depending on the implementation of the Sequencer this call may need to scan a number of values
         * in the Sequencer.  The scan will range from nextSequence to availableSequence. If there are no available values > nextSequence the return value will be nextSequence - 1.
         * To work correctly a consumer should pass a value that is 1 higher than the last sequence that was successfully processed.
         * 
         * \param nextSequence The sequence to start scanning from.
         * \param availableSequence The sequence to scan to.
         * \returns The highest value that can be safely read, will be at least nextSequence - 1</code>.
         */
        std::int64_t getHighestPublishedSequence(std::int64_t /*nextSequence*/, std::int64_t availableSequence) override
        {
            return availableSequence;
        }

    protected:
        Fields m_fields;
    };

} // namespace Disruptor
