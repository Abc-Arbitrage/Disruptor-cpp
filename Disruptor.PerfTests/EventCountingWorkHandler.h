#pragma once

#include <cstdint>
#include <memory>
#include <vector>

#include "Disruptor/IWorkHandler.h"

#include "Disruptor.PerfTests/PaddedLong.h"
#include "Disruptor.PerfTests/ValueEvent.h"


namespace Disruptor
{
namespace PerfTests
{

    class EventCountingWorkHandler : public IWorkHandler< ValueEvent >
    {
    public:
        EventCountingWorkHandler(const std::vector< std::shared_ptr< PaddedLong > >& counters, std::int32_t index);

        void onEvent(ValueEvent& evt) override;

    private:
        std::vector< std::shared_ptr< PaddedLong > > m_counters;
        std::int32_t m_index;
    };

} // namespace PerfTests
} // namespace Disruptor
