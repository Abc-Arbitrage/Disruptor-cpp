#pragma once

#include <bitset>

namespace Disruptor
{
namespace ThreadHelper
{

    using AffinityMask = std::bitset<64>;

    std::uint32_t getCurrentThreadId();
    std::uint32_t getCurrentProcessor();

    std::size_t getProcessorCount();

    bool setThreadAffinity (const AffinityMask& mask);
    AffinityMask getThreadAffinity();

    void setThreadName(const std::string& name);
    void setThreadName(int threadId, const std::string& name);

} // namespace ThreadHelper
} // namespace Disruptor
