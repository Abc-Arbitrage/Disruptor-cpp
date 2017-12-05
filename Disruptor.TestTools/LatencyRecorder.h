#pragma once

#include <cstdint>

#include "Disruptor/Pragmas.h"

DISRUPTOR_PRAGMA_PUSH
DISRUPTOR_PRAGMA_IGNORE_ALL
DISRUPTOR_PRAGMA_IGNORE_UNREACHABLE_CODE
DISRUPTOR_PRAGMA_IGNORE_DEPRECATED_DECLARATIONS
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics.hpp>
DISRUPTOR_PRAGMA_POP


namespace Disruptor
{
namespace Tests
{

    class LatencyRecorder
    {
        using Accumulator = boost::accumulators::accumulator_set
        <
            double,
            boost::accumulators::stats
            <
                boost::accumulators::tag::mean,
                boost::accumulators::tag::max,
                boost::accumulators::tag::min,
                boost::accumulators::tag::variance,
                boost::accumulators::tag::tail_quantile< boost::accumulators::right >
            >
        >;

    public:
        explicit LatencyRecorder(std::int64_t sampleSize);
        
        void record(std::int64_t value);

        void writeReport(std::ostream& stream) const;

    private:
        Accumulator m_accumulator;
    };

} // namespace Tests
} // namespace Disruptor
