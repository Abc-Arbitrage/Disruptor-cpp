#pragma once

#include <cstdint>
#include <memory>


namespace Disruptor
{

    class IWaitStrategy;


namespace Tests
{

    void assertWaitForWithDelayOf(std::int64_t sleepTimeMillis, const std::shared_ptr< IWaitStrategy >& waitStrategy);

} // namespace Tests
} // namespace Disruptor
