#pragma once

#include <atomic>

#include "Disruptor/ISequenceBarrier.h"
#include "Disruptor/IWaitStrategy.h"
#include "Disruptor/Sequence.h"


namespace Disruptor
{

    class IHighestPublishedSequenceProvider;
    class ISequence;
    class IWaitStrategy;
    class Sequence;


    /**
     * 
     * ISequenceBarrier handed out for gating IEventProcessor on a cursor sequence and optional dependent IEventProcessors, using the given WaitStrategy.
     */ 
    class ProcessingSequenceBarrier : public ISequenceBarrier, public std::enable_shared_from_this< ProcessingSequenceBarrier >
    {
    public:
        ProcessingSequenceBarrier(const std::shared_ptr< IHighestPublishedSequenceProvider >& sequenceProvider,
                                  const std::shared_ptr< IWaitStrategy >& waitStrategy,
                                  const std::shared_ptr< Sequence >& cursorSequence,
                                  const std::vector< std::shared_ptr< ISequence > >& dependentSequences);

        std::int64_t waitFor(std::int64_t sequence) override;

        std::int64_t cursor() override;

        bool isAlerted() override;

        void alert() override;

        void clearAlert() override;

        void checkAlert() override;

    private:
        static std::shared_ptr< ISequence > getDependentSequence(const std::shared_ptr< Sequence >& cursorSequence,
                                                                 const std::vector< std::shared_ptr< ISequence > >& dependentSequences);


        std::shared_ptr< IWaitStrategy > m_waitStrategy;
        std::shared_ptr< ISequence > m_dependentSequence;
        std::shared_ptr< Sequence > m_cursorSequence;
        std::shared_ptr< IHighestPublishedSequenceProvider > m_sequenceProvider;

        IWaitStrategy& m_waitStrategyRef;
        ISequence& m_dependentSequenceRef;
        Sequence& m_cursorSequenceRef;
        IHighestPublishedSequenceProvider& m_sequenceProviderRef;

        bool m_alerted;
    };

} // namespace Disruptor
