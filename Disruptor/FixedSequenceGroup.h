#pragma once

#include <memory>
#include <vector>

#include "Disruptor/ISequence.h"


namespace Disruptor
{

    /// <summary>
    /// Hides a group of Sequences behind a single Sequence
    /// </summary>
    class FixedSequenceGroup : public ISequence
    {
    public:
        /// <summary> </summary>
        /// <param name="sequences">sequences the list of sequences to be tracked under this sequence group</param>
        explicit FixedSequenceGroup(const std::vector< std::shared_ptr< ISequence > >& sequences);

        /// <summary>
        /// Get the minimum sequence value for the group.
        /// </summary>
        std::int64_t value() const override;

        /// <summary>
        /// Not supported.
        /// </summary>
        void setValue(std::int64_t value) override;

        /// <summary>
        /// Not supported
        /// </summary>
        /// <param name="value"></param>
        void setValueVolatile(std::int64_t value) override;

        /// <summary>
        /// Not supported.
        /// </summary>
        bool compareAndSet(std::int64_t expectedValue, std::int64_t newValue) override;

        /// <summary>
        /// Not supported.
        /// </summary>
        std::int64_t incrementAndGet() override;

        /// <summary>
        /// Not supported.
        /// </summary>
        std::int64_t addAndGet(std::int64_t increment) override;

        void writeDescriptionTo(std::ostream& stream) const override;

    private:
        std::vector< std::shared_ptr< ISequence > > m_sequences;
    };

} // namespace Disruptor
