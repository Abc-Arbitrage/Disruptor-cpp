#include "stdafx.h"
#include "LatencyRecorder.h"

#include "Disruptor.TestTools/DurationHumanizer.h"


namespace Disruptor
{
namespace Tests
{

    struct DurationPrinter
    {
        explicit DurationPrinter(std::int64_t nanoseconds)
            : value(nanoseconds)
        {
        }

        std::int64_t value;
    };

    std::ostream& operator<<(std::ostream& stream, const DurationPrinter& printer)
    {
        auto humanDuration = Tests::DurationHumanizer::deduceHumanDuration(std::chrono::nanoseconds(printer.value));

        return stream << humanDuration.value << " " << humanDuration.shortUnitName;
    }


    LatencyRecorder::LatencyRecorder(std::int64_t sampleSize)
        : m_accumulator(boost::accumulators::tag::tail< boost::accumulators::right >::cache_size = sampleSize)
    {
    }

    void LatencyRecorder::record(std::int64_t value)
    {
        m_accumulator(value);
    }

    void LatencyRecorder::writeReport(std::ostream& stream) const
    {
        stream
            << "min: " << DurationPrinter(static_cast< std::int64_t >(boost::accumulators::min(m_accumulator)))
            << ", mean: " << DurationPrinter(static_cast< std::int64_t >(boost::accumulators::mean(m_accumulator)))
            << ", max: " << DurationPrinter(static_cast< std::int64_t >(boost::accumulators::max(m_accumulator)))
            << ", Q99.99: " << DurationPrinter(static_cast< std::int64_t >(boost::accumulators::quantile(m_accumulator, boost::accumulators::quantile_probability = 0.9999)))
            << ", Q99.9: " << DurationPrinter(static_cast< std::int64_t >(boost::accumulators::quantile(m_accumulator, boost::accumulators::quantile_probability = 0.999)))
            << ", Q99: " << DurationPrinter(static_cast< std::int64_t >(boost::accumulators::quantile(m_accumulator, boost::accumulators::quantile_probability = 0.99)))
            << ", Q98: " << DurationPrinter(static_cast< std::int64_t >(boost::accumulators::quantile(m_accumulator, boost::accumulators::quantile_probability = 0.98)))
            << ", Q95: " << DurationPrinter(static_cast< std::int64_t >(boost::accumulators::quantile(m_accumulator, boost::accumulators::quantile_probability = 0.95)))
            << ", Q93: " << DurationPrinter(static_cast< std::int64_t >(boost::accumulators::quantile(m_accumulator, boost::accumulators::quantile_probability = 0.93)))
            << ", Q90: " << DurationPrinter(static_cast< std::int64_t >(boost::accumulators::quantile(m_accumulator, boost::accumulators::quantile_probability = 0.90)))
            << ", Q50: " << DurationPrinter(static_cast< std::int64_t >(boost::accumulators::quantile(m_accumulator, boost::accumulators::quantile_probability = 0.50)))
            ;
    }

} // namespace Tests
} // namespace Disruptor
