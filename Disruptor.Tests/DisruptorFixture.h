#pragma once

#include <cstdint>
#include <memory>
#include <vector>

#include "Disruptor/ClockConfig.h"
#include "Disruptor/Disruptor.h"

#include "Disruptor.TestTools/CountdownEvent.h"

#include "Disruptor.Tests/AtomicReference.h"
#include "Disruptor.Tests/DelayedEventHandler.h"
#include "Disruptor.Tests/StubExecutor.h"
#include "Disruptor.Tests/StubPublisher.h"
#include "Disruptor.Tests/TestEvent.h"
#include "Disruptor.Tests/TestWorkHandler.h"


namespace Disruptor
{
namespace Tests
{

    struct DisruptorFixture
    {
        class EventProcessorFactory;
        class EventTranslator;
        class TempEventHandler;

        DisruptorFixture();
        ~DisruptorFixture();

        std::shared_ptr< TestWorkHandler > createTestWorkHandler();

        void ensureTwoEventsProcessedAccordingToDependencies(const std::shared_ptr< CountdownEvent >& countDownLatch,
                                                             const std::initializer_list< std::shared_ptr< DelayedEventHandler > >& dependencies = {});

        static void assertProducerReaches(const std::shared_ptr< StubPublisher >& stubPublisher, std::int32_t expectedPublicationCount, bool strict);

        TestEvent& publishEvent();

        static std::exception waitFor(const std::shared_ptr< AtomicReference< std::exception > >& reference);

        std::shared_ptr< DelayedEventHandler > createDelayedEventHandler();

        void assertThatCountDownLatchEquals(const std::shared_ptr< CountdownEvent >& countDownLatch, std::int64_t expectedCountDownValue) const;
        void assertThatCountDownLatchIsZero(const std::shared_ptr< CountdownEvent >& countDownLatch) const;

        std::int32_t m_timeoutInSeconds = 2;
        std::shared_ptr< disruptor< TestEvent > > m_disruptor;
        std::shared_ptr< StubExecutor > m_executor;
        std::vector< std::shared_ptr< DelayedEventHandler > > m_delayedEventHandlers;
        std::vector< std::shared_ptr< TestWorkHandler > > m_testWorkHandlers;
        std::shared_ptr< RingBuffer< TestEvent > > m_ringBuffer;
        TestEvent* m_lastPublishedEvent;
    };


    class DisruptorFixture::EventProcessorFactory : public IEventProcessorFactory< TestEvent >
    {
    public:
        EventProcessorFactory(const std::shared_ptr< disruptor< TestEvent > >& disruptor,
                              const std::shared_ptr< IEventHandler< TestEvent > >& eventHandler,
                              std::int32_t sequenceLength);

        std::shared_ptr< IEventProcessor > createEventProcessor(const std::shared_ptr< RingBuffer< TestEvent > >& ringBuffer,
                                                                const std::vector< std::shared_ptr< ISequence > >& barrierSequences) override;

    private:
        std::shared_ptr< disruptor< TestEvent > > m_disruptor;
        std::shared_ptr< IEventHandler< TestEvent > > m_eventHandler;
        std::int32_t m_sequenceLength;
    };


    class DisruptorFixture::EventTranslator : public IEventTranslator< TestEvent >
    {
    public:
        explicit EventTranslator(DisruptorFixture& disruptorTests);

        void translateTo(TestEvent& eventData, std::int64_t sequence) override;

    private:
        DisruptorFixture& m_disruptorTests;
    };


    class DisruptorFixture::TempEventHandler : public IEventHandler< TestEvent >
    {
    public:
        TempEventHandler(const std::shared_ptr< disruptor< TestEvent > >& disruptor, const std::vector< std::shared_ptr< std::int64_t > >& remainingCapacity);

        void onEvent(TestEvent& data, std::int64_t sequence, bool endOfBatch) override;

    private:
        std::shared_ptr< disruptor< TestEvent > > m_disruptor;
        std::vector< std::shared_ptr< std::int64_t > > m_remainingCapacity;
    };

} // namespace Tests
} // namespace Disruptor
