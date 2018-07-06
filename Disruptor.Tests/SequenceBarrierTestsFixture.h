#pragma once

#include <memory>

#include "Disruptor/NoOpEventProcessor.h"
#include "Disruptor/RingBuffer.h"

#include "Disruptor.TestTools/CountdownEvent.h"

#include "Disruptor.Tests/EventProcessorMock.h"
#include "Disruptor.Tests/StubEvent.h"


namespace Disruptor
{
namespace Tests
{

    struct SequenceBarrierTestsFixture
    {
        class CountDownEventSequence;
        class StubEventProcessor;

        SequenceBarrierTestsFixture();

        void fillRingBuffer(std::int64_t expectedNumberEvents);


        std::shared_ptr< RingBuffer< StubEvent > > m_ringBuffer;
        std::shared_ptr< EventProcessorMock > m_eventProcessorMock1;
        std::shared_ptr< EventProcessorMock > m_eventProcessorMock2;
        std::shared_ptr< EventProcessorMock > m_eventProcessorMock3;
    };


    class SequenceBarrierTestsFixture::StubEventProcessor : public IEventProcessor
    {
    public:
        explicit StubEventProcessor(std::int64_t sequence);

        void run() override;

        std::shared_ptr< ISequence > sequence() const override;

        void halt() override;

        bool isRunning() const override;

    private:
        std::atomic< std::int32_t > m_running;
        std::shared_ptr< Sequence > m_sequence = std::make_shared< Sequence >();
    };


    class SequenceBarrierTestsFixture::CountDownEventSequence : public ISequence
    {
    public:
        CountDownEventSequence(std::int64_t initialValue, const std::shared_ptr< CountdownEvent >& signal);

        std::int64_t value() const override;

        void setValue(std::int64_t value) override;

        bool compareAndSet(std::int64_t expectedSequence, std::int64_t nextSequence) override;

        std::int64_t incrementAndGet() override;

        std::int64_t addAndGet(std::int64_t value) override;

        void writeDescriptionTo(std::ostream& stream) const override;

    private:
        std::shared_ptr< CountdownEvent > m_signal;
        std::shared_ptr< ISequence > m_sequenceImplementation;
    };

} // namespace Tests
} // namespace Disruptor
