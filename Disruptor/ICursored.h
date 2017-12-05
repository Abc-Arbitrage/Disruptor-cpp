#pragma once

#include <cstdint>


namespace Disruptor
{

    /// <summary>
    /// Implementors of this interface must provide a single long value
    /// that represents their current cursor value.Used during dynamic
    /// add/remove of Sequences from a
    /// <see cref="SequenceGroups.AddSequences"/>.
    /// </summary>
    class ICursored
    {
    public:
        virtual ~ICursored() = default;

        /// <summary>
        /// Get the current cursor value.
        /// </summary>
        virtual std::int64_t cursor() const = 0;
    };

} // namespace Disruptor
