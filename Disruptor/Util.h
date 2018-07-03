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

    /**
     * Calculate the next power of 2, greater than or equal to x.
     * 
     * \param x Value to round up
     * \returns The next power of 2 from x inclusive
     */ 
    std::int32_t ceilingNextPowerOfTwo(std::int32_t x);

    /**
     * Test whether a given integer is a power of 2
     * 
     * \param x 
     */ 
    bool isPowerOf2(std::int32_t x);

    /**
     * Calculate the log base 2 of the supplied integer, essentially reports the location of the highest bit.
     *
     * \param i Value to calculate log2 for.
     * \returns The log2 value
     */ 
    std::int32_t log2(std::int32_t i);

    /**
     * Get the minimum sequence from an array of Sequences.
     *
     * \param sequences sequences to compare.
     * \param minimum an initial default minimum.  If the array is empty this value will returned.
     * \returns the minimum sequence found or lon.MaxValue if the array is empty.
     */ 
    std::int64_t getMinimumSequence(const std::vector< std::shared_ptr< ISequence > >& sequences, std::int64_t minimum = std::numeric_limits< std::int64_t >::max());

    /**
     * Get an array of Sequences for the passed IEventProcessors
     *
     * \param processors processors for which to get the sequences
     * \returns the array of\returns <see cref="Sequence"/>\returns s
     */
    std::vector< std::shared_ptr< ISequence > > getSequencesFor(const std::vector< std::shared_ptr< IEventProcessor > >& processors);

} // namespace Util
} // namespace Disruptor
