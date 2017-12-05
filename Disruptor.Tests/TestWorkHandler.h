#pragma once

#include <atomic>
#include <cstdint>

#include "Disruptor/IWorkHandler.h"

#include "Disruptor.Tests/TestEvent.h"


namespace Disruptor
{
namespace Tests
{

    class TestWorkHandler : public IWorkHandler< TestEvent >
    {
    public:
        TestWorkHandler() 
            : m_readyToProcessEvent(0)
            , m_stopped(false)
        {}

        void onEvent(TestEvent& evt) override;

        void processEvent();

        void stopWaiting();

    private:
        void waitForAndSetFlag(std::int32_t newValue);

        std::atomic< std::int32_t > m_readyToProcessEvent;
        std::atomic< bool > m_stopped;
    };

} // namespace Tests
} // namespace Disruptor
