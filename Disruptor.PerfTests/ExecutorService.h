#pragma once

#include <future>
#include <memory>

#include "Disruptor/BatchEventProcessor.h"


namespace Disruptor
{
namespace PerfTests
{

    template <class T>
    class ExecutorService
    {
    public:
        std::future< void > submit(const std::shared_ptr< BatchEventProcessor< T > >& eventProcessor)
        {
            return std::async(std::launch::async, [eventProcessor] { eventProcessor->run(); });
        }
    };

} // namespace PerfTests
} // namespace Disruptor
