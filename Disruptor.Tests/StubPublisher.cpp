
#include "stdafx.h"
#include "StubPublisher.h"


namespace Disruptor
{
namespace Tests
{

    StubPublisher::StubPublisher(const std::shared_ptr< RingBuffer< TestEvent > >& ringBuffer)
        : m_running(true)
        , m_publicationCount(0)
    {
        m_ringBuffer = ringBuffer;
    }

    void StubPublisher::run()
    {
        while (m_running)
        {
            auto sequence = m_ringBuffer->next();
            m_ringBuffer->publish(sequence);
            ++m_publicationCount;
        }
    }

    std::int32_t StubPublisher::getPublicationCount() const
    {
        return m_publicationCount;
    }

    void StubPublisher::halt()
    {
        m_running = false;
    }

} // namespace Tests
} // namespace Disruptor

