#pragma once

#include "Disruptor/BlockingWaitStrategy.h"
#include "Disruptor/MultiProducerSequencer.h"
#include "Disruptor/Sequence.h"
#include "Disruptor/SingleProducerSequencer.h"


namespace Disruptor
{
namespace Tests
{

    template <class T>
    struct SequencerTestFixture
    {
        SequencerTestFixture()
            : m_waitStrategy(std::make_shared< BlockingWaitStrategy >())
            , m_sequencer(std::make_shared< T >(m_bufferSize, m_waitStrategy))
            , m_gatingSequence(std::make_shared< Sequence >())
        {}
    
        const std::int32_t m_bufferSize = 16;
        std::shared_ptr< IWaitStrategy > m_waitStrategy;
        std::shared_ptr< T > m_sequencer;
        std::shared_ptr< Sequence > m_gatingSequence;
    };
    
} // namespace Tests
} // namespace Disruptor
