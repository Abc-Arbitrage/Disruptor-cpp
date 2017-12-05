#include "stdafx.h"
#include "AggregateEventHandlerTestsFixture.h"


namespace Disruptor
{
namespace Tests
{

    AggregateEventHandlerTestsFixture::AggregateEventHandlerTestsFixture()
    {
        m_eventHandlerMock1 = std::make_shared< testing::NiceMock< LifecycleAwareEventHandlerMock< std::int32_t > > >();
        m_eventHandlerMock2 = std::make_shared< testing::NiceMock< LifecycleAwareEventHandlerMock< std::int32_t > > >();
        m_eventHandlerMock3 = std::make_shared< testing::NiceMock< LifecycleAwareEventHandlerMock< std::int32_t > > >();

        std::vector< std::shared_ptr< IEventHandler< std::int32_t > > > v = {m_eventHandlerMock1, m_eventHandlerMock2, m_eventHandlerMock3};
        m_aggregateEventHandler = std::make_shared< AggregateEventHandler< std::int32_t > >(v);
    }

} // namespace Tests
} // namespace Disruptor
