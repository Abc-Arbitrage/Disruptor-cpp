#include "stdafx.h"

#include <boost/mpl/vector.hpp>

#include "Disruptor/Disruptor.h"
#include "Disruptor/RoundRobinThreadAffinedTaskScheduler.h"
#include "Disruptor/SleepingWaitStrategy.h"

#include "LongEvent.h"


namespace Disruptor
{
namespace Tests
{

    class ParallelEventHandler : public IEventHandler< LongEvent >
    {
    public:
        ParallelEventHandler(std::int64_t mask, std::int64_t ordinal)
            : processed(0)
        {
            m_mask = mask;
            m_ordinal = ordinal;
        }
    
        void onEvent(LongEvent& event, std::int64_t sequence, bool endOfBatch) override
        {
            if ((sequence & m_mask) == m_ordinal)
            {
                eventCount++;
                tempValue = event.value;
            }
            
            if (endOfBatch || ++batchCount >= m_batchSize)
            {
                publishedValue = tempValue;
                batchCount = 0;
            }
            else
            {
                std::this_thread::yield();
            }

            processed = sequence;
        }

        std::int64_t eventCount = 0;
        std::int64_t batchCount = 0;
        std::int64_t publishedValue = 0;
        std::int64_t tempValue = 0;
        std::atomic< std::int64_t > processed;

    private:
        std::int64_t m_mask = 0;
        std::int64_t m_ordinal = 0;
        static const std::int32_t m_batchSize = 10;
    };
    
    class EventTranslator : public IEventTranslator< LongEvent >
    {
    public:
        void translateTo(LongEvent& eventData, std::int64_t sequence) override
        {
            eventData.value = sequence;
        }
    };
    
} // namespace Tests
} // namespace Disruptor


using namespace Disruptor;
using namespace ::Disruptor::Tests;


BOOST_AUTO_TEST_SUITE(BatchingTests)

typedef boost::mpl::vector
<
    std::integral_constant< ProducerType, ProducerType::Single >,
    std::integral_constant< ProducerType, ProducerType::Multi >
>
ProducerTypes;

BOOST_AUTO_TEST_CASE_TEMPLATE(ShouldBatch, TProducerType, ProducerTypes)
{
    auto scheduler = std::make_shared< RoundRobinThreadAffinedTaskScheduler >();
    scheduler->start(std::thread::hardware_concurrency());

    auto d = std::make_shared< disruptor< LongEvent > >([] { return LongEvent(); }, 2048, scheduler, TProducerType::value, std::make_shared< SleepingWaitStrategy >());

    auto handler1 = std::make_shared< ParallelEventHandler >(1, 0);
    auto handler2 = std::make_shared< ParallelEventHandler >(1, 1);

    d->handleEventsWith({ handler1, handler2 });

    auto buffer = d->start();

    auto translator = std::make_shared< EventTranslator >();

    const std::int32_t eventCount = 10000;
    for (auto i = 0; i < eventCount; ++i)
    {
        buffer->publishEvent(translator);
    }

    while (handler1->processed != eventCount - 1 ||
           handler2->processed != eventCount - 1)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    BOOST_CHECK_EQUAL(handler1->publishedValue, static_cast< std::int64_t >(eventCount) - 2);
    BOOST_CHECK_EQUAL(handler1->eventCount, static_cast< std::int64_t >(eventCount) / 2);
    BOOST_CHECK_EQUAL(handler2->publishedValue, static_cast< std::int64_t >(eventCount) - 1);
    BOOST_CHECK_EQUAL(handler2->eventCount, static_cast< std::int64_t >(eventCount) / 2);

    d->shutdown();
    scheduler->stop();
}

BOOST_AUTO_TEST_SUITE_END()
