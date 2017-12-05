#include "stdafx.h"
#include "ValueAdditionEventHandler.h"


namespace Disruptor
{
namespace PerfTests
{

    void ValueAdditionEventHandler::reset(const std::shared_ptr< Tests::ManualResetEvent >& latch, std::int64_t expectedCount)
    {
        m_value.value = 0;
        m_latch = latch;
        m_count = expectedCount;
    }

    void ValueAdditionEventHandler::onEvent(ValueEvent& value, std::int64_t sequence, bool)
    {
        m_value.value = m_value.value + value.value;

        if (count() == sequence)
        {
            if (m_latch != nullptr)
                m_latch->set();
        }
    }

    std::int64_t ValueAdditionEventHandler::count() const
    {
        return m_count;
    }

    std::int64_t ValueAdditionEventHandler::value() const
    {
        return m_value.value;
    }

} // namespace PerfTests
} // namespace Disruptor
