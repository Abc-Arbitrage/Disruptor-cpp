#include "stdafx.h"

#include "Disruptor/NoOpEventProcessor.h"
#include "Disruptor/RingBuffer.h"

#include "LongEvent.h"


namespace Disruptor
{
namespace Tests
{

    struct EventPublisherFixture
    {
        struct Translator : public IEventTranslator< LongEvent >
        {
            explicit Translator(EventPublisherFixture&)
            {
            }

            void translateTo(LongEvent& eventData, std::int64_t sequence) override
            {
                eventData.value = sequence + 29;
            }

        };

        EventPublisherFixture()
        {
            m_ringBuffer = RingBuffer< LongEvent >::createMultiProducer([]() { return LongEvent(); }, m_bufferSize);
            m_translator = std::make_shared< Translator >(*this);
        }

        const std::int32_t m_bufferSize = 32;
        const std::int64_t m_valueAdd = 29L;
        std::shared_ptr< RingBuffer< LongEvent > > m_ringBuffer;
        std::shared_ptr< Translator > m_translator;
    };


} // namespace Tests
} // namespace Disruptor

using namespace Disruptor;
using namespace ::Disruptor::Tests;


BOOST_FIXTURE_TEST_SUITE(EventPublisherTests, EventPublisherFixture)

BOOST_AUTO_TEST_CASE(ShouldPublishEvent)
{
    m_ringBuffer->addGatingSequences({ std::make_shared< NoOpEventProcessor< LongEvent > >(m_ringBuffer)->sequence() });

    m_ringBuffer->publishEvent(m_translator);
    m_ringBuffer->publishEvent(m_translator);

    BOOST_CHECK_EQUAL(0L + m_valueAdd, (*m_ringBuffer)[0].value);
    BOOST_CHECK_EQUAL(1L + m_valueAdd, (*m_ringBuffer)[1].value);
}

BOOST_AUTO_TEST_CASE(ShouldTryPublishEvent)
{
    m_ringBuffer->addGatingSequences({ std::make_shared< Sequence >() });

    for (auto i = 0; i < m_bufferSize; ++i)
    {
        BOOST_CHECK_EQUAL(m_ringBuffer->tryPublishEvent(m_translator), true);
    }

    for (auto i = 0; i < m_bufferSize; ++i)
    {
        BOOST_CHECK_EQUAL((*m_ringBuffer)[i].value, i + m_valueAdd);
    }

    BOOST_CHECK_EQUAL(m_ringBuffer->tryPublishEvent(m_translator), false);
}

BOOST_AUTO_TEST_SUITE_END()
