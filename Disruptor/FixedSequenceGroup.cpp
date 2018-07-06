#include "stdafx.h"
#include "FixedSequenceGroup.h"

#include <ostream>

#include "NotSupportedException.h"
#include "Util.h"


namespace Disruptor
{

    FixedSequenceGroup::FixedSequenceGroup(const std::vector< std::shared_ptr< ISequence > >& sequences)
        : m_sequences(sequences)
    {
    }

    std::int64_t FixedSequenceGroup::value() const
    {
        return Util::getMinimumSequence(m_sequences);
    }

    void FixedSequenceGroup::setValue(std::int64_t /*value*/)
    {
        DISRUPTOR_THROW_NOT_SUPPORTED_EXCEPTION();
    }

    bool FixedSequenceGroup::compareAndSet(std::int64_t /*expectedValue*/, std::int64_t /*newValue*/)
    {
        DISRUPTOR_THROW_NOT_SUPPORTED_EXCEPTION();
    }

    std::int64_t FixedSequenceGroup::incrementAndGet()
    {
        DISRUPTOR_THROW_NOT_SUPPORTED_EXCEPTION();
    }

    std::int64_t FixedSequenceGroup::addAndGet(std::int64_t /*increment*/)
    {
        DISRUPTOR_THROW_NOT_SUPPORTED_EXCEPTION();
    }

    void FixedSequenceGroup::writeDescriptionTo(std::ostream& stream) const
    {
        auto firstItem = true;
        for (auto&& sequence : m_sequences)
        {
            if (firstItem)
                firstItem = false;
            else
                stream << ", ";
            stream << "{ ";
            sequence->writeDescriptionTo(stream);
            stream << " }";
        }
    }

} // namespace Disruptor
