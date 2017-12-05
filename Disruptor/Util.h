#pragma once

#include <cstdint>
#include <limits>
#include <memory>
#include <vector>

#include "Disruptor/IEventProcessor.h"
#include "Disruptor/ISequence.h"


namespace Disruptor
{

    class IEventProcessor;
    class ISequence;


namespace Util
{

    /// <summary>
    /// Calculate the next power of 2, greater than or equal to x.
    /// </summary>
    /// <param name="x">Value to round up</param>
    /// <returns>The next power of 2 from x inclusive</returns>
    std::int32_t ceilingNextPowerOfTwo(std::int32_t x);

    /// <summary>
    /// Test whether a given integer is a power of 2 
    /// </summary>
    /// <param name="x"></param>
    /// <returns></returns>
    bool isPowerOf2(std::int32_t x);

    /// <summary>
    /// Calculate the log base 2 of the supplied integer, essentially reports the location
    /// of the highest bit.
    /// </summary>
    /// <param name="i">Value to calculate log2 for.</param>
    /// <returns>The log2 value</returns>
    std::int32_t log2(std::int32_t i);

    /// <summary>
    /// Get the minimum sequence from an array of <see cref="Sequence"/>s.
    /// </summary>
    /// <param name="sequences">sequences to compare.</param>
    /// <param name="minimum">an initial default minimum.  If the array is empty this value will returned.</param>
    /// <returns>the minimum sequence found or lon.MaxValue if the array is empty.</returns>
    std::int64_t getMinimumSequence(const std::vector< std::shared_ptr< ISequence > >& sequences, std::int64_t minimum = std::numeric_limits< std::int64_t >::max());

    /// <summary>
    /// Get an array of <see cref="Sequence"/>s for the passed <see cref="IEventProcessor"/>s
    /// </summary>
    /// <param name="processors">processors for which to get the sequences</param>
    /// <returns>the array of <see cref="Sequence"/>s</returns>
    std::vector< std::shared_ptr< ISequence > > getSequencesFor(const std::vector< std::shared_ptr< IEventProcessor > >& processors);

} // namespace Util
} // namespace Disruptor
