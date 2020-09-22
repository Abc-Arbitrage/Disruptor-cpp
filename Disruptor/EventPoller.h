#pragma once

#include <memory>

#include "Disruptor/FixedSequenceGroup.h"
#include "Disruptor/IDataProvider.h"
#include "Disruptor/ISequencer.h"
#include "Disruptor/ISequence.h"
#include "Disruptor/ProducerType.h"

namespace Disruptor
{

    enum class PollState
    {
        Processing,
        Gating,
        Idle
    };


    template <class T>
    class EventPoller
    {
    public:
        EventPoller(const std::shared_ptr< IDataProvider< T > >& dataProvider,
                    const std::shared_ptr< ISequencer< T > >& sequencer,
                    const std::shared_ptr< ISequence >& sequence,
                    const std::shared_ptr< ISequence >& gatingSequence)
             : m_dataProvider(dataProvider)
             , m_sequencer(sequencer)
             , m_sequence(sequence)
             , m_gatingSequence(gatingSequence)
        {}

        template <class TEventHandler>
        PollState poll(TEventHandler&& eventHandler)
        {
#if DISRUPTOR_CPP_17
            static_assert(std::is_invocable_r<bool, TEventHandler, T&, std::int64_t, bool>::value,
                          "eventHandler should have the following signature: bool(T&, std::int64_t, bool)");
#endif

            auto currentSequence = m_sequence->value();
            auto nextSequence = currentSequence + 1;
            auto availableSequence = m_sequencer->getHighestPublishedSequence(nextSequence, m_gatingSequence->value());

            if (nextSequence <= availableSequence)
            {
                bool processNextEvent;
                auto processedSequence = currentSequence;

                try
                {
                    do
                    {
                        auto& event = (*m_dataProvider)[nextSequence];
                        processNextEvent = eventHandler(event, nextSequence, nextSequence == availableSequence);
                        processedSequence = nextSequence;
                        nextSequence++;
                    }
                    while (nextSequence <= availableSequence && processNextEvent);
                }
                catch (...)
                {
                    m_sequence->setValue(processedSequence);
                    throw;
                }

                m_sequence->setValue(processedSequence);

                return PollState::Processing;
            }

            if (m_sequencer->cursor() >= nextSequence)
            {
                return PollState::Gating;
            }

            return PollState::Idle;
        }

        static std::shared_ptr< EventPoller< T > > newInstance(const std::shared_ptr< IDataProvider< T > >& dataProvider,
                                                               const std::shared_ptr< ISequencer< T > >& sequencer,
                                                               const std::shared_ptr< ISequence >& sequence,
                                                               const std::shared_ptr< ISequence >& cursorSequence,
                                                               const std::vector< std::shared_ptr< ISequence > >& gatingSequences)
        {
            std::shared_ptr< ISequence > gatingSequence;

            if (gatingSequences.empty())
            {
                gatingSequence = cursorSequence;
            }
            else if (gatingSequences.size() == 1)
            {
                gatingSequence = *gatingSequences.begin();
            }
            else
            {
                gatingSequence = std::make_shared< FixedSequenceGroup >(gatingSequences);
            }

            return std::make_shared< EventPoller < T > >(dataProvider, sequencer, sequence, gatingSequence);
        }

        std::shared_ptr< ISequence > sequence() const
        {
            return m_sequence;
        };

    private:
        std::shared_ptr< IDataProvider< T > > m_dataProvider;
        std::shared_ptr< ISequencer< T > > m_sequencer;
        std::shared_ptr< ISequence > m_sequence;
        std::shared_ptr< ISequence > m_gatingSequence;
    };

} // namespace Disruptor


#include <ostream>


namespace std
{

    inline ostream& operator<<(ostream& stream, const Disruptor::PollState& value)
    {
        switch(value)
        {
        case Disruptor::PollState::Processing:
            return stream << "Processing";
        case Disruptor::PollState::Gating:
            return stream << "Gating";
        case Disruptor::PollState::Idle:
            return stream << "Idle";
        default:
            return stream << static_cast< int >(value);
        }
    }

} // namespace std
