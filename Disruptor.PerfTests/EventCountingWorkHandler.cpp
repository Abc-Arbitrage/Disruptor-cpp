#include "stdafx.h"
#include "EventCountingWorkHandler.h"


namespace Disruptor
{
namespace PerfTests
{

    EventCountingWorkHandler::EventCountingWorkHandler(const std::vector< std::shared_ptr< PaddedLong > >& counters, std::int32_t index)
        : m_counters(counters)
        , m_index(index)
    {
    }

    void EventCountingWorkHandler::onEvent(ValueEvent& /*evt*/)
    {
        m_counters[m_index]->value = m_counters[m_index]->value + 1L;
    }

} // namespace PerfTests
} // namespace Disruptor
