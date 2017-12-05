#pragma once

#include <memory>
#include <boost/thread/barrier.hpp>

#include "Disruptor/ISequenceBarrier.h"
#include "Disruptor/RingBuffer.h"

#include "Disruptor.Tests/StubEvent.h"


namespace Disruptor
{
namespace Tests
{

    class TestWaiter
    {
    public:
        TestWaiter(const std::shared_ptr< boost::barrier >& barrier,
                   const std::shared_ptr< ISequenceBarrier >& sequenceBarrier,
                   const std::shared_ptr< RingBuffer< StubEvent > >& ringBuffer,
                   std::int64_t initialSequence,
                   std::int64_t toWaitForSequence);

        std::vector< StubEvent > call() const;

    private:
        std::shared_ptr< boost::barrier > m_barrier;
        std::shared_ptr< ISequenceBarrier > m_sequenceBarrier;
        std::shared_ptr< RingBuffer< StubEvent > > m_ringBuffer;
        std::int64_t m_initialSequence;
        std::int64_t m_toWaitForSequence;
    };

} // namespace Tests
} // namespace Disruptor
