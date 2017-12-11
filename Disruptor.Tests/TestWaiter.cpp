#include "stdafx.h"
#include "TestWaiter.h"


namespace Disruptor
{
namespace Tests
{

    TestWaiter::TestWaiter(const std::shared_ptr< boost::barrier >& barrier,
                           const std::shared_ptr< ISequenceBarrier >& sequenceBarrier,
                           const std::shared_ptr< RingBuffer< StubEvent > >& ringBuffer,
                           std::int64_t initialSequence,
                           std::int64_t toWaitForSequence)
        : m_barrier(barrier)
        , m_sequenceBarrier(sequenceBarrier)
        , m_ringBuffer(ringBuffer)
        , m_initialSequence(initialSequence)
        , m_toWaitForSequence(toWaitForSequence)
    {
    }

    std::vector< StubEvent > TestWaiter::call() const
    {
        m_barrier->wait();
        m_sequenceBarrier->waitFor(m_toWaitForSequence);

        std::vector< StubEvent > events;
        for (auto l = m_initialSequence; l <= m_toWaitForSequence; ++l)
        {
            events.push_back((*m_ringBuffer)[l]);
        }

        return events;
    }

} // namespace Tests
} // namespace Disruptor
