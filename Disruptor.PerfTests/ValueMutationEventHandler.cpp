#include "stdafx.h"
#include "ValueMutationEventHandler.h"


namespace Disruptor
{
namespace PerfTests
{

    ValueMutationEventHandler::ValueMutationEventHandler(Operation operation)
        : m_operation(operation)
    {
    }

    std::int64_t ValueMutationEventHandler::value() const
    {
        return m_value.value;
    }

    void ValueMutationEventHandler::reset(const std::shared_ptr< boost::barrier >& latch, std::int64_t expectedCount)
    {
        m_value.value = 0L;
        m_latch = latch;
        m_iterations = expectedCount;
    }

    void ValueMutationEventHandler::onEvent(ValueEvent& data, std::int64_t sequence, bool /*endOfBatch*/)
    {
        m_value.value = OperationExtensions::Op(m_operation, m_value.value, data.value);

        if (sequence == m_iterations)
        {
            m_latch->wait();
        }
    }

} // namespace PerfTests
} // namespace Disruptor
