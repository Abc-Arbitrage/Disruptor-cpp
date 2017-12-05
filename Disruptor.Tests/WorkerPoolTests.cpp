#include "stdafx.h"

#include "Disruptor/BasicExecutor.h"
#include "Disruptor/FatalExceptionHandler.h"
#include "Disruptor/RoundRobinThreadAffinedTaskScheduler.h"
#include "Disruptor/WorkerPool.h"


namespace Disruptor
{
namespace Tests
{
    
    class AtomicLong
    {
    public:
        AtomicLong() : m_value(0)
        {}

        AtomicLong(const AtomicLong& other)
            : m_value(other.value())
        {}

        void operator=(const AtomicLong& other)
        {
            m_value = other.value();
        }

        std::int64_t value() const
        {
            return m_value;
        }

        void increment()
        {
            ++m_value;
        }

    private:
        std::atomic< std::int64_t > m_value;
    };

    class AtomicLongWorkHandler : public IWorkHandler< AtomicLong >
    {
    public:
        void onEvent(AtomicLong& evt) override
        {
            evt.increment();
        }
    };

} // namespace Tests
} // namespace Disruptor


using namespace Disruptor;
using namespace Disruptor::Tests;


BOOST_AUTO_TEST_SUITE(WorkerPoolTests)

BOOST_AUTO_TEST_CASE(ShouldProcessEachMessageByOnlyOneWorker)
{
    auto taskScheduler = std::make_shared< RoundRobinThreadAffinedTaskScheduler >();
    taskScheduler->start(2);

    WorkerPool< AtomicLong > pool
    (
        [] { return AtomicLong(); },
        std::make_shared< FatalExceptionHandler< AtomicLong > >(),
        { std::make_shared< AtomicLongWorkHandler >(), std::make_shared< AtomicLongWorkHandler >() }
    );

    auto ringBuffer = pool.start(std::make_shared< BasicExecutor >(taskScheduler));

    ringBuffer->next();
    ringBuffer->next();
    ringBuffer->publish(0);
    ringBuffer->publish(1);

    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    BOOST_CHECK_EQUAL((*ringBuffer)[0].value(), 1L);
    BOOST_CHECK_EQUAL((*ringBuffer)[1].value(), 1L);
}

BOOST_AUTO_TEST_CASE(ShouldProcessOnlyOnceItHasBeenPublished)
{
    auto taskScheduler = std::make_shared< RoundRobinThreadAffinedTaskScheduler >();
    taskScheduler->start(2);

    WorkerPool< AtomicLong > pool
    (
        [] { return AtomicLong(); },
        std::make_shared< FatalExceptionHandler< AtomicLong > >(),
        { std::make_shared< AtomicLongWorkHandler >(), std::make_shared< AtomicLongWorkHandler >() }
    );

    auto ringBuffer = pool.start(std::make_shared< BasicExecutor >(taskScheduler));

    ringBuffer->next();
    ringBuffer->next();

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    BOOST_CHECK_EQUAL((*ringBuffer)[0].value(), 0L);
    BOOST_CHECK_EQUAL((*ringBuffer)[1].value(), 0L);
}

BOOST_AUTO_TEST_SUITE_END()
