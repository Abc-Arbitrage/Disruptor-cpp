#pragma once

#include <atomic>
#include <cstdint>
#include <memory>

#include "Disruptor/RingBuffer.h"

#include "Disruptor.Tests/TestEvent.h"


namespace Disruptor
{
namespace Tests
{

    class StubPublisher
    {
    public:
        explicit StubPublisher(const std::shared_ptr< RingBuffer< TestEvent > >& ringBuffer);

        void run();

        std::int32_t getPublicationCount() const;

        void halt();

    private:
        std::atomic< bool > m_running;
        std::atomic< std::int32_t > m_publicationCount;

        std::shared_ptr< RingBuffer< TestEvent > > m_ringBuffer;
    };

} // namespace Tests
} // namespace Disruptor
