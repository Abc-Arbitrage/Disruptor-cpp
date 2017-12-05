#pragma once

#include <cstdint>
#include <vector>

#include "Disruptor/IEventHandler.h"

#include "Disruptor.TestTools/ManualResetEvent.h"

#include "Disruptor.PerfTests/PaddedLong.h"


namespace Disruptor
{
namespace PerfTests
{

    class LongArrayEventHandler : public IEventHandler< std::vector< std::int64_t > >
    {
    public:
        std::int64_t count() const;

        const std::shared_ptr< Tests::ManualResetEvent >& signal() const;

        std::int64_t value() const;

        void reset(const std::shared_ptr< Tests::ManualResetEvent >& signal, std::int64_t expectedCount);

        void onEvent(std::vector< std::int64_t >& data, std::int64_t sequence, bool endOfBatch) override;

    private:
        std::shared_ptr< PaddedLong > m_value = std::make_shared< PaddedLong >();
        std::int64_t m_count = 0;
        std::shared_ptr< Tests::ManualResetEvent > m_signal;
    };

} // namespace PerfTests
} // namespace Disruptor
