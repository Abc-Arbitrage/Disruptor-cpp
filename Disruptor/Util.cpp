#include "stdafx.h"
#include "Util.h"

#include "Disruptor/IEventProcessor.h"
#include "Disruptor/ISequence.h"


namespace Disruptor
{
namespace Util
{

    std::int32_t ceilingNextPowerOfTwo(std::int32_t x)
    {
        std::int32_t result = 2;

        while (result < x)
        {
            result <<= 1;
        }

        return result;
    }

    bool isPowerOf2(std::int32_t x)
    {
        return x > 0 && (x & (x - 1)) == 0;
    }

    std::int32_t log2(std::int32_t i)
    {
        std::int32_t r = 0;
        while ((i >>= 1) != 0)
        {
            ++r;
        }
        return r;
    }

    std::int64_t getMinimumSequence(const std::vector< std::shared_ptr< ISequence > >& sequences, std::int64_t minimum)
    {
        for (auto i = 0u; i < sequences.size(); ++i)
        {
            auto sequence = sequences[i]->value();
            if (sequence < minimum)
                minimum = sequence;
        }
        return minimum;
    }

    std::vector< std::shared_ptr< ISequence > > getSequencesFor(const std::vector< std::shared_ptr< IEventProcessor > >& processors)
    {
        std::vector< std::shared_ptr< ISequence > > sequences(processors.size());

        for (auto i = 0u; i < sequences.size(); ++i)
        {
            sequences[i] = processors[i]->sequence();
        }

        return sequences;
    }

} // namespace Util
} // namespace Disruptor
