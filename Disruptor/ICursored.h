#pragma once

#include <cstdint>


namespace Disruptor
{

    /**
     * Implementors of this interface must provide a single long value that represents their current cursor value.Used during dynamic add/remove of Sequences from a SequenceGroups.addSequences
     */ 
    class ICursored
    {
    public:
        virtual ~ICursored() = default;

        /**
         * Get the current cursor value.
         */ 
        virtual std::int64_t cursor() const = 0;
    };

} // namespace Disruptor
