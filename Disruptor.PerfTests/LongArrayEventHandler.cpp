#include "stdafx.h"
#include "LongArrayEventHandler.h"


namespace Disruptor
{
namespace PerfTests
{

    std::int64_t LongArrayEventHandler::count() const
    {
        return m_count;
    }

    const std::shared_ptr< Tests::ManualResetEvent >& LongArrayEventHandler::signal() const
    {
        return m_signal;
    }

    std::int64_t LongArrayEventHandler::value() const
    {
        return m_value->value;
    }

    void LongArrayEventHandler::reset(const std::shared_ptr< Tests::ManualResetEvent >& signal, std::int64_t expectedCount)
    {
        m_value->value = 0L;
        m_signal = signal;
        m_count = expectedCount;
    }

    void LongArrayEventHandler::onEvent(std::vector< std::int64_t >& data, std::int64_t /*sequence*/, bool /*endOfBatch*/)
    {
        for (auto i = 0u; i < data.size(); ++i)
        {
            m_value->value = m_value->value + data[i];
        }

        if (--m_count == 0)
        {
            if (m_signal != nullptr)
                m_signal->set();
        }
    }

} // namespace PerfTests
} // namespace Disruptor
