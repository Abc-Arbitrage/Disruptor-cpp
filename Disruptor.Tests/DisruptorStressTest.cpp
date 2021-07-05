#include "stdafx.h"

#include "Disruptor/BusySpinWaitStrategy.h"
#include "Disruptor/Disruptor.h"
#include "Disruptor/RoundRobinThreadAffinedTaskScheduler.h"

#include "Disruptor.TestTools/CountdownEvent.h"


using namespace Disruptor;
using namespace ::Disruptor::Tests;


BOOST_AUTO_TEST_SUITE(DisruptorStressTest)

struct MyTestEvent
{
    std::int64_t sequence;
    std::int64_t a;
    std::int64_t b;
    std::string s;
};

class TestEventHandler : public IEventHandler< MyTestEvent >
{
public:
    void onEvent(MyTestEvent& event, std::int64_t sequence, bool) override
    {
        if (event.sequence != sequence || event.a != sequence + 13 || event.b != sequence - 7)
        {
            failureCount++;
        }

        messagesSeen++;
    }

    std::int32_t failureCount;
    std::int32_t messagesSeen;
};

class Publisher
{
public:
    Publisher(const std::shared_ptr < RingBuffer< MyTestEvent > >& ringBuffer,
              std::int32_t iterations,
              const std::shared_ptr< CountdownEvent >& start,
              const std::shared_ptr< CountdownEvent >& end)
        : m_ringBuffer(ringBuffer)
        , m_end(end)
        , m_start(start)
        , m_iterations(iterations)
    {}

    void run()
    {
        try
        {
            m_start->signal();
            m_start->wait();

            auto i = m_iterations;
            while (--i != -1)
            {
                auto next = m_ringBuffer->next();
                auto& testEvent = (*m_ringBuffer)[next];
                testEvent.sequence = next;
                testEvent.a = next + 13;
                testEvent.b = next - 7;

                m_ringBuffer->publish(next);
            }
        }
        catch (...)
        {
            failed = true;
        }

        m_end->signal();
    }

    bool failed = false;

private:
    std::shared_ptr< RingBuffer< MyTestEvent > > m_ringBuffer;
    std::shared_ptr< CountdownEvent > m_end;
    std::shared_ptr< CountdownEvent > m_start;
    std::int32_t m_iterations;
};

std::vector< std::shared_ptr< Publisher > > initialize(size_t size,
                                                       const std::shared_ptr< RingBuffer< MyTestEvent > >& buffer,
                                                       int messageCount,
                                                       const std::shared_ptr< CountdownEvent >& start,
                                                       const std::shared_ptr< CountdownEvent >& end)
{
    std::vector< std::shared_ptr< Publisher > > result;

    for (auto i = 0u; i < size; i++)
    {
        result.push_back(std::make_shared< Publisher >(buffer, messageCount, start, end));
    }

    return result;
}

std::vector< std::shared_ptr< TestEventHandler > > initialize(const std::shared_ptr< disruptor< MyTestEvent > >& disruptor, size_t size)
{
    std::vector< std::shared_ptr< TestEventHandler > > result;

    for (auto i = 0u; i < size; i++)
    {
        auto handler = std::make_shared< TestEventHandler >();
        disruptor->handleEventsWith(handler);
        result.push_back(handler);
    }

    return result;
}

BOOST_AUTO_TEST_CASE(ShouldHandleLotsOfThreads)
{
    auto processorsCount = std::thread::hardware_concurrency();
    processorsCount = std::max(processorsCount / 2, 1u);

    auto scheduler = std::make_shared< RoundRobinThreadAffinedTaskScheduler >();
    scheduler->start(processorsCount);

    auto waitStrategy = std::make_shared< BusySpinWaitStrategy >();
    auto testDisruptor = std::make_shared< disruptor< MyTestEvent > >([] { return MyTestEvent(); }, 1 << 16, scheduler, ProducerType::Multi, waitStrategy);

    auto ringBuffer = testDisruptor->ringBuffer();
    testDisruptor->setDefaultExceptionHandler(std::make_shared< FatalExceptionHandler< MyTestEvent > >());

#ifdef _DEBUG
    const auto iterations = 200000;
#else
    const auto iterations = 20000000;
#endif

    auto publisherCount = processorsCount;
    auto handlerCount = processorsCount;

    auto end = std::make_shared< CountdownEvent >(publisherCount);
    auto start = std::make_shared< CountdownEvent >(publisherCount);

    auto handlers = initialize(testDisruptor, handlerCount);
    auto publishers = initialize(publisherCount, ringBuffer, iterations, start, end);

    testDisruptor->start();

    for (auto&& publisher : publishers)
    {
        std::thread([publisher] { publisher->run(); }).detach();
    }

    end->wait();
    while (ringBuffer->cursor() < (iterations - 1))
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(0));
    }

    testDisruptor->shutdown();

    for (auto&& publisher : publishers)
    {
        BOOST_CHECK_EQUAL(publisher->failed, false);
    }

    for (auto&& handler : handlers)
    {
        BOOST_CHECK_NE(handler->messagesSeen, 0);
        BOOST_CHECK_EQUAL(handler->failureCount, 0);
    }

    scheduler->stop();
}

BOOST_AUTO_TEST_SUITE_END()
