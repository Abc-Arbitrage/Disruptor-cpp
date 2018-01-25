#pragma once

#include <memory>

#include "Disruptor/InsufficientCapacityException.h"
#include "Disruptor/Sequencer.h"
#include "Disruptor/SpinWait.h"
#include "Disruptor/Util.h"


namespace Disruptor
{

    /// <summary>
    /// <para>Coordinator for claiming sequences for access to a data structure while tracking dependent <see cref="Sequence"/>s.
    /// Suitable for use for sequencing across multiple publisher threads.</para>
    /// <para/>
    /// <para/>Note on <see cref="Sequencer.Cursor"/>:  With this sequencer the cursor value is updated after the call
    /// to <see cref="Sequencer.Next()"/>, to determine the highest available sequence that can be read, then
    /// <see cref="GetHighestPublishedSequence"/> should be used. 
    /// </summary>
    template <class T>
    class MultiProducerSequencer : public Sequencer< T >
    {
    public:
        MultiProducerSequencer(std::int32_t bufferSize, const std::shared_ptr< IWaitStrategy >& waitStrategy)
            : Sequencer< T >(bufferSize, waitStrategy)
        {
            m_availableBuffer = std::unique_ptr< int[] >(new int [bufferSize]);
            m_indexMask = bufferSize - 1;
            m_indexShift = Util::log2(bufferSize);
            initializeAvailableBuffer();
        }

        /// <summary>
        /// Has the buffer got capacity to allocate another sequence.  This is a concurrent
        /// method so the response should only be taken as an indication of available capacity.
        /// </summary>
        /// <param name="requiredCapacity">requiredCapacity in the buffer</param>
        /// <returns>true if the buffer has the capacity to allocate the next sequence otherwise false.</returns>
        bool hasAvailableCapacity(std::int32_t requiredCapacity) override
        {
            return hasAvailableCapacity(this->m_gatingSequences, requiredCapacity, this->m_cursor->value());
        }

        /// <summary>
        /// Claim a specific sequence when only one publisher is involved.
        /// </summary>
        /// <param name="sequence">sequence to be claimed.</param>
        void claim(std::int64_t sequence) override
        {
            this->m_cursor->setValue(sequence);
        }

        /// <summary>
        /// Claim the next event in sequence for publishing.
        /// </summary>
        /// <returns></returns>
        std::int64_t next() override
        {
            return next(1);
        }

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
        std::int64_t next(std::int32_t n) override
        {
            if (n < 1)
            {
                DISRUPTOR_THROW_ARGUMENT_EXCEPTION("n must be > 0");
            }

            std::int64_t current;
            std::int64_t next;

            SpinWait spinWait;
            do
            {
                current = this->m_cursor->value();
                next = current + n;

                std::int64_t wrapPoint = next - this->m_bufferSize;
                std::int64_t cachedGatingSequence = m_gatingSequenceCache->value();

                if (wrapPoint > cachedGatingSequence || cachedGatingSequence > current)
                {
                    std::int64_t gatingSequence = Util::getMinimumSequence(this->m_gatingSequences, current);

                    if (wrapPoint > gatingSequence)
                    {
                        this->m_waitStrategy->signalAllWhenBlocking();
                        spinWait.spinOnce();
                        continue;
                    }

                    m_gatingSequenceCache->setValue(gatingSequence);
                }
                else if (this->m_cursor->compareAndSet(current, next))
                {
                    break;
                }
            }
            while (true);

            return next;
        }

        /// <summary>
        /// Attempt to claim the next event in sequence for publishing.  Will return the number of the slot if there is at least<code>requiredCapacity</code> slots available.
        /// </summary>
        /// <returns>the claimed sequence value</returns>
        std::int64_t tryNext() override
        {
            return tryNext(1);
        }

        /// <summary>
        /// Attempt to claim the next event in sequence for publishing.  Will return the
        /// number of the slot if there is at least <param name="n"></param> slots
        /// available. 
        /// </summary>
        /// <param name="n">the number of sequences to claim</param>
        /// <returns>the claimed sequence value</returns>
        std::int64_t tryNext(std::int32_t n) override
        {
            if (n < 1)
            {
                DISRUPTOR_THROW_ARGUMENT_EXCEPTION("n must be > 0");
            }

            std::int64_t current;
            std::int64_t next;

            do
            {
                current = this->m_cursor->value();
                next = current + n;

                if (!hasAvailableCapacity(this->m_gatingSequences, n, current))
                {
                    DISRUPTOR_THROW_INSUFFICIENT_CAPACITY_EXCEPTION();
                }
            }
            while (!this->m_cursor->compareAndSet(current, next));

            return next;
        }

        /// <summary>
        /// Get the remaining capacity for this sequencer. return The number of slots remaining.
        /// </summary>
        std::int64_t getRemainingCapacity() override
        {
            auto consumed = Util::getMinimumSequence(this->m_gatingSequences, this->m_cursorRef.value());
            auto produced = this->m_cursorRef.value();

            return this->bufferSize() - (produced - consumed);
        }

        /// <summary>
        /// Publish an event and make it visible to <see cref="IEventProcessor"/>s
        /// </summary>
        /// <param name="sequence">sequence to be published</param>
        void publish(std::int64_t sequence) override
        {
            setAvailable(sequence);
            this->m_waitStrategyRef.signalAllWhenBlocking();
        }

        /// <summary>
        /// Publish an event and make it visible to <see cref="IEventProcessor"/>s
        /// </summary>
        void publish(std::int64_t lo, std::int64_t hi) override
        {
            for (std::int64_t l = lo; l <= hi; l++)
            {
                setAvailable(l);
            }
            this->m_waitStrategyRef.signalAllWhenBlocking();
        }

        /// <summary>
        /// Confirms if a sequence is published and the event is available for use; non-blocking.
        /// </summary>
        /// <param name="sequence">sequence of the buffer to check</param>
        /// <returns>true if the sequence is available for use, false if not</returns>
        bool isAvailable(std::int64_t sequence) override
        {
            auto index = calculateIndex(sequence);
            auto flag = calculateAvailabilityFlag(sequence);

            return m_availableBuffer[index] == flag;
        }

        /// <summary>
        /// Get the highest sequence number that can be safely read from the ring buffer.  Depending
        /// on the implementation of the Sequencer this call may need to scan a number of values
        /// in the Sequencer.  The scan will range from nextSequence to availableSequence.  If
        /// there are no available values <code>&amp;gt;= nextSequence</code> the return value will be
        /// <code>nextSequence - 1</code>.  To work correctly a consumer should pass a value that
        /// it 1 higher than the last sequence that was successfully processed.
        /// </summary>
        /// <param name="nextSequence">The sequence to start scanning from.</param>
        /// <param name="availableSequence">The sequence to scan to.</param>
        /// <returns>The highest value that can be safely read, will be at least <code>nextSequence - 1</code>.</returns>
        std::int64_t getHighestPublishedSequence(std::int64_t lowerBound, std::int64_t availableSequence) override
        {
            for (std::int64_t sequence = lowerBound; sequence <= availableSequence; sequence++)
            {
                if (!isAvailable(sequence))
                {
                    return sequence - 1;
                }
            }

            return availableSequence;
        }

    private:
        bool hasAvailableCapacity(const std::vector< std::shared_ptr< ISequence > >& gatingSequences, std::int32_t requiredCapacity, std::int64_t cursorValue)
        {
            auto wrapPoint = (cursorValue + requiredCapacity) - this->m_bufferSize;
            auto cachedGatingSequence = m_gatingSequenceCache->value();

            if (wrapPoint > cachedGatingSequence || cachedGatingSequence > cursorValue)
            {
                auto minSequence = Util::getMinimumSequence(gatingSequences, cursorValue);
                m_gatingSequenceCache->setValue(minSequence);

                if (wrapPoint > minSequence)
                {
                    return false;
                }
            }

            return true;
        }

        void initializeAvailableBuffer()
        {
            for (std::int32_t i = this->m_bufferSize - 1; i != 0; i--)
            {
                setAvailableBufferValue(i, -1);
            }

            setAvailableBufferValue(0, -1);
        }

        void setAvailable(std::int64_t sequence)
        {
            setAvailableBufferValue(calculateIndex(sequence), calculateAvailabilityFlag(sequence));
        }

        void setAvailableBufferValue(std::int32_t index, std::int32_t flag)
        {
            m_availableBuffer[index] = flag;
        }

        std::int32_t calculateAvailabilityFlag(std::int64_t sequence)
        {
            return static_cast< std::int32_t >(static_cast< std::uint64_t >(sequence) >> m_indexShift);
        }

        std::int32_t calculateIndex(std::int64_t sequence)
        {
            return static_cast< std::int32_t >(sequence) & m_indexMask;
        }

    private:
        std::shared_ptr< Sequence > m_gatingSequenceCache = std::make_shared< Sequence >();

        // availableBuffer tracks the state of each ringbuffer slot
        // see below for more details on the approach
        std::unique_ptr< std::int32_t[] > m_availableBuffer;
        std::int32_t m_indexMask;
        std::int32_t m_indexShift;
    };

} // namespace Disruptor
