#pragma once

#include <memory>
#include <ostream>
#include <vector>

#include "Disruptor/ArgumentException.h"
#include "Disruptor/ISequencer.h"
#include "Disruptor/IWaitStrategy.h"
#include "Disruptor/ProcessingSequenceBarrier.h"
#include "Disruptor/Sequence.h"
#include "Disruptor/SequenceGroups.h"
#include "Disruptor/EventPoller.h"
#include "Disruptor/Util.h"


namespace Disruptor
{

    template <class T>
    class Sequencer : public ISequencer< T >, public std::enable_shared_from_this< Sequencer< T > >
    {
    public:
        /// <summary>
        /// Construct a Sequencer with the selected strategies.
        /// </summary>
        /// <param name="bufferSize"></param>
        /// <param name="waitStrategy">waitStrategy for those waiting on sequences.</param>
        Sequencer(std::int32_t bufferSize, const std::shared_ptr< IWaitStrategy >& waitStrategy)
            : m_bufferSize(bufferSize)
            , m_waitStrategy(waitStrategy)
            , m_cursor(std::make_shared< Sequence >())
            , m_waitStrategyRef(*m_waitStrategy)
            , m_cursorRef(*m_cursor)
        {
            if (bufferSize < 1)
            {
                DISRUPTOR_THROW_ARGUMENT_EXCEPTION("bufferSize must not be less than 1");
            }

            if (!Util::isPowerOf2(bufferSize))
            {
                DISRUPTOR_THROW_ARGUMENT_EXCEPTION("bufferSize must be a power of 2");
            }
        }

        /// <summary>
        /// Create a <see cref="ISequenceBarrier"/> that gates on the the cursor and a list of <see cref="Sequence"/>s
        /// </summary>
        /// <param name="sequencesToTrack"></param>
        /// <returns></returns>
        std::shared_ptr< ISequenceBarrier > newBarrier(const std::vector< std::shared_ptr< ISequence > >& sequencesToTrack) override
        {
            return std::make_shared< ProcessingSequenceBarrier >(this->shared_from_this(), m_waitStrategy, m_cursor, sequencesToTrack);
        }

        /// <summary>
        /// The capacity of the data structure to hold entries.
        /// </summary>
        std::int32_t bufferSize() override
        {
            return m_bufferSize;
        }

        /// <summary>
        /// Get the value of the cursor indicating the published sequence.
        /// </summary>
        std::int64_t cursor() const override
        {
            return m_cursorRef.value();
        }

        /// <summary>
        /// Add the specified gating sequences to this instance of the Disruptor.  They will
        /// safely and atomically added to the list of gating sequences. 
        /// </summary>
        /// <param name="gatingSequences">The sequences to add.</param>
        void addGatingSequences(const std::vector< std::shared_ptr< ISequence > >& gatingSequences) override
        {
            SequenceGroups::addSequences(m_gatingSequences, *this, gatingSequences);
        }

        /// <summary>
        /// Remove the specified sequence from this sequencer.
        /// </summary>
        /// <param name="sequence">to be removed.</param>
        /// <returns>true if this sequence was found, false otherwise.</returns>
        bool removeGatingSequence(const std::shared_ptr< ISequence >& sequence) override
        {
            return SequenceGroups::removeSequence(m_gatingSequences, sequence);
        }

        /// <summary>
        /// Get the minimum sequence value from all of the gating sequences
        /// added to this ringBuffer.
        /// </summary>
        /// <returns>The minimum gating sequence or the cursor sequence if no sequences have been added.</returns>
        std::int64_t getMinimumSequence() override
        {
            //auto& gatingSequences = this->m_gatingSequences;
            //auto gatingSequences = std::atomic_load_explicit(&this->m_gatingSequences, std::memory_order_acquire);

            return Util::getMinimumSequence(m_gatingSequences, m_cursorRef.value());
            //return Util::getMinimumSequence(*gatingSequences, m_cursor->value());
        }

        /// <summary>
        /// Creates an event poller for this sequence that will use the supplied data provider and
        /// gating sequences.
        /// </summary>
        /// <typeparam name="T"></typeparam>
        /// <param name="provider">The data source for users of this event poller</param>
        /// <param name="gatingSequences">Sequence to be gated on.</param>
        /// <returns>A poller that will gate on this ring buffer and the supplied sequences.</returns>
        std::shared_ptr< EventPoller< T > > newPoller(const std::shared_ptr< IDataProvider< T > > & provider, const std::vector< std::shared_ptr< ISequence > >& gatingSequences) override
        {
            return EventPoller< T >::newInstance(provider, this->shared_from_this(), std::make_shared< Sequence >(), m_cursor, gatingSequences);
        }

        void writeDescriptionTo(std::ostream& stream) const override
        {
            stream << "WaitStrategy: { ";
            m_waitStrategy->writeDescriptionTo(stream);
            stream << " }, Cursor: { ";
            m_cursor->writeDescriptionTo(stream);
            stream << " }, GatingSequences: [ ";

            auto firstItem = true;
            for (auto&& sequence : m_gatingSequences)
            {
                if (firstItem)
                    firstItem = false;
                else
                    stream << ", ";
                stream << "{ ";
                sequence->writeDescriptionTo(stream);
                stream << " }";
            }

            stream << " ]";
        }

    protected:
        /// <summary>Volatile in the Java version => always use Volatile.Read/Write or Interlocked methods to access this field.</summary>
        std::vector< std::shared_ptr< ISequence > > m_gatingSequences;

        std::int32_t m_bufferSize;
        std::shared_ptr< IWaitStrategy > m_waitStrategy;
        std::shared_ptr< Sequence > m_cursor;
        IWaitStrategy& m_waitStrategyRef;
        Sequence& m_cursorRef;
    };

} // namespace Disruptor
