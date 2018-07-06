#include "stdafx.h"
#include "SequenceBarrierTestsFixture.h"


namespace Disruptor
{
namespace Tests
{

    SequenceBarrierTestsFixture::SequenceBarrierTestsFixture()
    {
        m_ringBuffer = RingBuffer< StubEvent >::createMultiProducer([] { return StubEvent(-1); }, 64);

        m_eventProcessorMock1 = std::make_shared< testing::NiceMock< EventProcessorMock > >();
        m_eventProcessorMock2 = std::make_shared< testing::NiceMock< EventProcessorMock > >();
        m_eventProcessorMock3 = std::make_shared< testing::NiceMock< EventProcessorMock > >();

        m_ringBuffer->addGatingSequences({std::make_shared< NoOpEventProcessor< StubEvent > >(m_ringBuffer)->sequence()});
    }

    void SequenceBarrierTestsFixture::fillRingBuffer(std::int64_t expectedNumberEvents)
    {
        for (auto i = 0; i < expectedNumberEvents; i++)
        {
            auto sequence = m_ringBuffer->next();
            auto& event = (*m_ringBuffer)[sequence];
            event.value(i);
            m_ringBuffer->publish(sequence);
        }
    }

    SequenceBarrierTestsFixture::StubEventProcessor::StubEventProcessor(std::int64_t sequence)
    {
        m_sequence->setValue(sequence);
    }

    void SequenceBarrierTestsFixture::StubEventProcessor::run()
    {
        if (std::atomic_exchange(&m_running, 1) != 0)
            DISRUPTOR_THROW_INVALID_OPERATION_EXCEPTION("Already running");
    }

    std::shared_ptr< ISequence > SequenceBarrierTestsFixture::StubEventProcessor::sequence() const
    {
        return m_sequence;
    }

    void SequenceBarrierTestsFixture::StubEventProcessor::halt()
    {
        m_running = 0;
    }

    bool SequenceBarrierTestsFixture::StubEventProcessor::isRunning() const
    {
        return m_running == 1;
    }

    SequenceBarrierTestsFixture::CountDownEventSequence::CountDownEventSequence(std::int64_t initialValue, const std::shared_ptr< CountdownEvent >& signal)
        : m_signal(signal)
        , m_sequenceImplementation(std::make_shared< Sequence >(initialValue))
    {
    }

    std::int64_t SequenceBarrierTestsFixture::CountDownEventSequence::value() const
    {
        if (m_signal->currentCount() > 0)
            m_signal->signal();

        return m_sequenceImplementation->value();
    }

    void SequenceBarrierTestsFixture::CountDownEventSequence::setValue(std::int64_t value)
    {
        m_sequenceImplementation->setValue(value);
    }

    bool SequenceBarrierTestsFixture::CountDownEventSequence::compareAndSet(std::int64_t expectedSequence, std::int64_t nextSequence)
    {
        return m_sequenceImplementation->compareAndSet(expectedSequence, nextSequence);
    }

    std::int64_t SequenceBarrierTestsFixture::CountDownEventSequence::incrementAndGet()
    {
        return m_sequenceImplementation->incrementAndGet();
    }

    std::int64_t SequenceBarrierTestsFixture::CountDownEventSequence::addAndGet(std::int64_t value)
    {
        return m_sequenceImplementation->addAndGet(value);
    }

    void SequenceBarrierTestsFixture::CountDownEventSequence::writeDescriptionTo(std::ostream& /*stream*/) const
    {
    }

} // namespace Tests
} // namespace Disruptor
