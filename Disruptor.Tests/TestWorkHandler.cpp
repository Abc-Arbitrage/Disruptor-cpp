#include "stdafx.h"
#include "TestWorkHandler.h"


namespace Disruptor
{
namespace Tests
{

    void TestWorkHandler::onEvent(TestEvent& /*evt*/)
    {
        waitForAndSetFlag(0);
    }

    void TestWorkHandler::processEvent()
    {
        waitForAndSetFlag(1);
    }

    void TestWorkHandler::stopWaiting()
    {
        m_stopped = true;
    }

    void TestWorkHandler::waitForAndSetFlag(std::int32_t newValue)
    {
        while (!m_stopped && std::atomic_exchange(&m_readyToProcessEvent, newValue) == newValue)
        {
            std::this_thread::yield();
        }
    }

} // namespace Tests
} // namespace Disruptor
