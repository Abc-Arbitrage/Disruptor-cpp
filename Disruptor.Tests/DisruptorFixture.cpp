#include "stdafx.h"
#include "DisruptorFixture.h"


namespace Disruptor
{
namespace Tests
{

    DisruptorFixture::DisruptorFixture()
    {
        m_lastPublishedEvent = nullptr;
        m_ringBuffer = nullptr;
        m_delayedEventHandlers.clear();
        m_testWorkHandlers.clear();
        m_executor = std::make_shared< StubExecutor >();
        m_disruptor = std::make_shared< disruptor< TestEvent > >([] { return TestEvent(); }, 4, m_executor);
    }

    DisruptorFixture::~DisruptorFixture()
    {
        for (auto&& delayedEventHandler : m_delayedEventHandlers)
        {
            delayedEventHandler->stopWaiting();
        }

        for (auto&& testWorkHandler : m_testWorkHandlers)
        {
            testWorkHandler->stopWaiting();
        }

        m_disruptor->halt();
        m_executor->joinAllThreads();
    }

    DisruptorFixture::TempEventHandler::TempEventHandler(const std::shared_ptr< disruptor< TestEvent > >& disruptor,
                                                         const std::vector< std::shared_ptr< std::int64_t > >& remainingCapacity)
        : m_disruptor(disruptor)
        , m_remainingCapacity(remainingCapacity)
    {
    }

    void DisruptorFixture::TempEventHandler::onEvent(TestEvent& /*data*/, std::int64_t /*sequence*/, bool /*endOfBatch*/)
    {
        *m_remainingCapacity[0] = m_disruptor->ringBuffer()->getRemainingCapacity();
    }

    DisruptorFixture::EventProcessorFactory::EventProcessorFactory(const std::shared_ptr< disruptor< TestEvent > >& disruptor,
                                                                   const std::shared_ptr< IEventHandler< TestEvent > >& eventHandler,
                                                                   std::int32_t sequenceLength)
        : m_disruptor(disruptor)
        , m_eventHandler(eventHandler)
        , m_sequenceLength(sequenceLength)
    {
    }

    std::shared_ptr< IEventProcessor > DisruptorFixture::EventProcessorFactory::createEventProcessor(const std::shared_ptr< RingBuffer< TestEvent > >& ringBuffer,
                                                                                                     const std::vector< std::shared_ptr< ISequence > >& barrierSequences)
    {
        BOOST_CHECK_MESSAGE((size_t)m_sequenceLength == barrierSequences.size(), "Should not have had any barrier sequences");
        return std::make_shared< BatchEventProcessor< TestEvent > >(m_disruptor->ringBuffer(), ringBuffer->newBarrier(barrierSequences), m_eventHandler);
    }

    std::shared_ptr< TestWorkHandler > DisruptorFixture::createTestWorkHandler()
    {
        auto testWorkHandler = std::make_shared< TestWorkHandler >();
        m_testWorkHandlers.push_back(testWorkHandler);
        return testWorkHandler;
    }

    void DisruptorFixture::ensureTwoEventsProcessedAccordingToDependencies(const std::shared_ptr< CountdownEvent >& countDownLatch,
                                                                           const std::initializer_list< std::shared_ptr< DelayedEventHandler > >& dependencies)
    {
        publishEvent();
        publishEvent();

        for (auto&& dependency : dependencies)
        {
            assertThatCountDownLatchEquals(countDownLatch, 2L);
            dependency->processEvent();
            dependency->processEvent();
        }

        assertThatCountDownLatchIsZero(countDownLatch);
    }

    void DisruptorFixture::assertProducerReaches(const std::shared_ptr< StubPublisher >& stubPublisher, std::int32_t expectedPublicationCount, bool strict)
    {
        auto loopStart = ClockConfig::Clock::now();
        while (stubPublisher->getPublicationCount() < expectedPublicationCount && ClockConfig::Clock::now() - loopStart < std::chrono::milliseconds(5))
        {
            std::this_thread::yield();
        }

        if (strict)
        {
            BOOST_CHECK_EQUAL(stubPublisher->getPublicationCount(), expectedPublicationCount);
        }
        else
        {
            auto actualPublicationCount = stubPublisher->getPublicationCount();
            BOOST_CHECK_MESSAGE(actualPublicationCount >= expectedPublicationCount,
                "Producer reached unexpected count. Expected at least " << expectedPublicationCount << " but only reached " << actualPublicationCount);
        }
    }

    DisruptorFixture::EventTranslator::EventTranslator(DisruptorFixture& disruptorTests)
        : m_disruptorTests(disruptorTests)
    {
    }

    void DisruptorFixture::EventTranslator::translateTo(TestEvent& eventData, std::int64_t /*sequence*/)
    {
        m_disruptorTests.m_lastPublishedEvent = &eventData;
    }

    TestEvent& DisruptorFixture::publishEvent()
    {
        if (m_ringBuffer == nullptr)
        {
            m_ringBuffer = m_disruptor->start();

            for (auto&& eventHandler : m_delayedEventHandlers)
            {
                eventHandler->awaitStart();
            }
        }

        m_disruptor->publishEvent(std::make_shared< EventTranslator >(*this));

        return *m_lastPublishedEvent;
    }

    std::exception DisruptorFixture::waitFor(const std::shared_ptr< AtomicReference< std::exception > >& reference)
    {
        while (!reference->read())
        {
            std::this_thread::yield();
        }

        return reference->read().get();
    }

    std::shared_ptr< DelayedEventHandler > DisruptorFixture::createDelayedEventHandler()
    {
        auto delayedEventHandler = std::make_shared< DelayedEventHandler >();
        m_delayedEventHandlers.push_back(delayedEventHandler);
        return delayedEventHandler;
    }

    void DisruptorFixture::assertThatCountDownLatchEquals(const std::shared_ptr< CountdownEvent >& countDownLatch, std::int64_t expectedCountDownValue) const
    {
        BOOST_CHECK_EQUAL(countDownLatch->currentCount(), expectedCountDownValue);
    }

    void DisruptorFixture::assertThatCountDownLatchIsZero(const std::shared_ptr< CountdownEvent >& countDownLatch) const
    {
        auto released = countDownLatch->wait(std::chrono::seconds(m_timeoutInSeconds));
        BOOST_CHECK_MESSAGE(released == true, "Batch handler did not receive entries: " << countDownLatch->currentCount());
    }

} // namespace Tests
} // namespace Disruptor
