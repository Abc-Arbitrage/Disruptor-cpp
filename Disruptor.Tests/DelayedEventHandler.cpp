#include "stdafx.h"
#include "DelayedEventHandler.h"


namespace Disruptor
{
namespace Tests
{

    DelayedEventHandler::DelayedEventHandler() 
       : DelayedEventHandler(std::make_shared< boost::barrier >(2))
    {}

    void DelayedEventHandler::onEvent(TestEvent& /*data*/, std::int64_t /*sequence*/, bool /*endOfBatch*/)
    {
        waitForAndSetFlag(0);
    }

    void DelayedEventHandler::processEvent()
    {
        waitForAndSetFlag(1);
    }

    void DelayedEventHandler::stopWaiting()
    {
        m_stopped = true;
    }

    void DelayedEventHandler::onStart()
    {
        try
        {
            m_barrier->wait();
        }
        catch (std::exception& ex)
        {
            throw std::runtime_error(ex.what());
        }
    }

    void DelayedEventHandler::onShutdown()
    {
    }

    void DelayedEventHandler::awaitStart()
    {
        m_barrier->wait();
    }

    DelayedEventHandler::DelayedEventHandler(const std::shared_ptr< boost::barrier >& barrier)
        : m_readyToProcessEvent(0)
        , m_stopped(false)
        , m_barrier(barrier)
    {
    }

    void DelayedEventHandler::waitForAndSetFlag(std::int32_t newValue)
    {
        //    while (!_stopped && Thread.CurrentThread.IsAlive && Interlocked.Exchange(ref _readyToProcessEvent, newValue) == newValue)
        while (!m_stopped && std::atomic_exchange(&m_readyToProcessEvent, newValue) == newValue)
        {
            std::this_thread::yield();
        }
    }

} // namespace Tests
} // namespace Disruptor

