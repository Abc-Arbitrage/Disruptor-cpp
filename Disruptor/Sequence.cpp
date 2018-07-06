#include "stdafx.h"
#include "Sequence.h"

#include <ostream>


namespace Disruptor
{

    const std::int64_t Sequence::InitialCursorValue = -1;
    
    Sequence::Sequence(std::int64_t initialValue)
        : m_fieldsValue(initialValue)
    {
    }

    std::int64_t Sequence::value() const
    {
        return std::atomic_load_explicit(&m_fieldsValue, std::memory_order_acquire);
    }

    void Sequence::setValue(std::int64_t value)
    {
        std::atomic_store_explicit(&m_fieldsValue, value, std::memory_order_release);
    }

    bool Sequence::compareAndSet(std::int64_t expectedSequence, std::int64_t nextSequence)
    {
        return std::atomic_compare_exchange_strong(&m_fieldsValue, &expectedSequence, nextSequence);
    }

    std::int64_t Sequence::incrementAndGet()
    {
        return std::atomic_fetch_add(&m_fieldsValue, std::int64_t(1)) + 1;
    }

    std::int64_t Sequence::addAndGet(std::int64_t value)
    {
        return std::atomic_fetch_add(&m_fieldsValue, value) + value;
    }

    void Sequence::writeDescriptionTo(std::ostream& stream) const
    {
        stream << m_fieldsValue.load();
    }

} // namespace Disruptor
