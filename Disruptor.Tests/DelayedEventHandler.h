#pragma once

#include <atomic>
#include <cstdint>
#include <memory>

#include <boost/thread/barrier.hpp>

#include "Disruptor/IEventHandler.h"
#include "Disruptor/ILifecycleAware.h"

#include "Disruptor.Tests/TestEvent.h"


namespace Disruptor
{
namespace Tests
{

    class DelayedEventHandler : public IEventHandler< TestEvent >, public ILifecycleAware
    {
    public:
        DelayedEventHandler();

        void onEvent(TestEvent& data, std::int64_t sequence, bool endOfBatch) override;

        void processEvent();

        void stopWaiting();

        void onStart() override;

        void onShutdown() override;

        void awaitStart();

    private:
        explicit DelayedEventHandler(const std::shared_ptr< boost::barrier >& barrier);

        void waitForAndSetFlag(std::int32_t newValue);

        std::atomic< std::int32_t > m_readyToProcessEvent;
        std::atomic< bool > m_stopped;
        std::shared_ptr< boost::barrier > m_barrier;
    };

} // namespace Tests
} // namespace Disruptor
