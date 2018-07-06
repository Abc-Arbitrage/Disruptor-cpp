#pragma once

#include <memory>
#include <vector>

#include "Disruptor/ISequence.h"


namespace Disruptor
{

    /**
     * Hides a group of Sequences behind a single Sequence
     */ 
    class FixedSequenceGroup : public ISequence
    {
    public:
        /**
         * 
         * \param sequences sequences the list of sequences to be tracked under this sequence group
         */ 
        explicit FixedSequenceGroup(const std::vector< std::shared_ptr< ISequence > >& sequences);

        /**
         * Get the minimum sequence value for the group.
         */ 
        std::int64_t value() const override;

        /**
         * Not supported.
         */ 
        void setValue(std::int64_t value) override;

        /**
         * Not supported.
         */ 
        bool compareAndSet(std::int64_t expectedValue, std::int64_t newValue) override;

        /**
         * Not supported.
         */ 
        std::int64_t incrementAndGet() override;

        /**
         * Not supported.
         */ 
        std::int64_t addAndGet(std::int64_t increment) override;

        void writeDescriptionTo(std::ostream& stream) const override;

    private:
        std::vector< std::shared_ptr< ISequence > > m_sequences;
    };

} // namespace Disruptor
