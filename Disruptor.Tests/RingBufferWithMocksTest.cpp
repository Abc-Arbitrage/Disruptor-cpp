#include "stdafx.h"

#include "Disruptor/RingBuffer.h"
#include "SequencerMock.h"
#include "StubEvent.h"


namespace Disruptor
{
namespace Tests
{

    struct RingBufferWithMocksFixture
    {
        RingBufferWithMocksFixture()
        {
            m_sequencerMock = std::make_shared< testing::NiceMock< SequencerMock< StubEvent > > >();
            m_sequencer = m_sequencerMock;

            ON_CALL(*m_sequencerMock, bufferSize()).WillByDefault(testing::Return(16));
            m_ringBuffer = std::make_shared< RingBuffer< StubEvent > >(StubEvent::eventFactory(), m_sequencer);
        }
        
        std::shared_ptr< RingBuffer< StubEvent > > m_ringBuffer;
        std::shared_ptr< ISequencer< StubEvent > > m_sequencer;
        std::shared_ptr< SequencerMock< StubEvent > > m_sequencerMock;
    };

} // namespace Tests
} // namespace Disruptor


using namespace Disruptor;
using namespace Disruptor::Tests;


BOOST_FIXTURE_TEST_SUITE(RingBufferWithMocksTest, RingBufferWithMocksFixture)

BOOST_AUTO_TEST_CASE(ShouldDelgateNextAndPublish)
{
    EXPECT_CALL(*m_sequencerMock, next()).WillOnce(testing::Return(34L));
    EXPECT_CALL(*m_sequencerMock, publish(34L)).Times(1);

    m_ringBuffer->publish(m_ringBuffer->next());
}

BOOST_AUTO_TEST_CASE(ShouldDelgateTryNextAndPublish)
{
    EXPECT_CALL(*m_sequencerMock, tryNext()).WillOnce(testing::Return(34L));
    EXPECT_CALL(*m_sequencerMock, publish(34L)).Times(1);

    m_ringBuffer->publish(m_ringBuffer->tryNext());
}

BOOST_AUTO_TEST_CASE(ShouldDelgateNextNAndPublish)
{
    EXPECT_CALL(*m_sequencerMock, next(10)).WillOnce(testing::Return(34L));
    EXPECT_CALL(*m_sequencerMock, publish(25L, 34L)).Times(1);

    auto hi = m_ringBuffer->next(10);
    m_ringBuffer->publish(hi - 9, hi);
}

BOOST_AUTO_TEST_CASE(ShouldDelgateTryNextNAndPublish)
{
    EXPECT_CALL(*m_sequencerMock, tryNext(10)).WillOnce(testing::Return(34L));
    EXPECT_CALL(*m_sequencerMock, publish(25L, 34L)).Times(1);

    auto hi = m_ringBuffer->tryNext(10);
    m_ringBuffer->publish(hi - 9, hi);
}

BOOST_AUTO_TEST_SUITE_END()
