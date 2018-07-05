#include "stdafx.h"
#include "ProcessingSequenceBarrier.h"

#include "AlertException.h"
#include "FixedSequenceGroup.h"
#include "IHighestPublishedSequenceProvider.h"
#include "IWaitStrategy.h"
#include "Sequence.h"


namespace Disruptor
{

    ProcessingSequenceBarrier::ProcessingSequenceBarrier(const std::shared_ptr< IHighestPublishedSequenceProvider >& sequenceProvider,
                                                         const std::shared_ptr< IWaitStrategy >& waitStrategy,
                                                         const std::shared_ptr< Sequence >& cursorSequence,
                                                         const std::vector< std::shared_ptr< ISequence > >& dependentSequences)
        : m_waitStrategy(waitStrategy)
        , m_dependentSequence(getDependentSequence(cursorSequence, dependentSequences))
        , m_cursorSequence(cursorSequence)
        , m_sequenceProvider(sequenceProvider)
        , m_waitStrategyRef(*m_waitStrategy)
        , m_dependentSequenceRef(*m_dependentSequence)
        , m_cursorSequenceRef(*m_cursorSequence)
        , m_sequenceProviderRef(*m_sequenceProvider)
    {
        m_alerted = false;
    }

    std::int64_t ProcessingSequenceBarrier::waitFor(std::int64_t sequence)
    {
        checkAlert();

        auto availableSequence = m_waitStrategyRef.waitFor(sequence, *m_cursorSequence, *m_dependentSequence, *shared_from_this());

        if (availableSequence < sequence)
            return availableSequence;

        return m_sequenceProviderRef.getHighestPublishedSequence(sequence, availableSequence);
    }

    std::int64_t ProcessingSequenceBarrier::cursor()
    {
        return m_dependentSequenceRef.value();
    }

    bool ProcessingSequenceBarrier::isAlerted()
    {
        return m_alerted;
    }

    void ProcessingSequenceBarrier::alert()
    {
        m_alerted = true;
        m_waitStrategyRef.signalAllWhenBlocking();
    }

    void ProcessingSequenceBarrier::clearAlert()
    {
        m_alerted = false;
    }

    void ProcessingSequenceBarrier::checkAlert()
    {
        if (m_alerted)
        {
            DISRUPTOR_THROW_ALERT_EXCEPTION();
        }
    }

    std::shared_ptr< ISequence > ProcessingSequenceBarrier::getDependentSequence(const std::shared_ptr< Sequence >& cursorSequence,
                                                                                 const std::vector< std::shared_ptr< ISequence > >& dependentSequences)
    {
        if (dependentSequences.empty())
            return cursorSequence;

        return std::make_shared< FixedSequenceGroup >(dependentSequences);
    }

} // namespace Disruptor
