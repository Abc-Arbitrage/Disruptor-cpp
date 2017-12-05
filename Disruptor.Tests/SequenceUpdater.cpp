#include "stdafx.h"
#include "SequenceUpdater.h"


namespace Disruptor
{
namespace Tests
{

    SequenceUpdater::SequenceUpdater(std::int64_t sleepTime, const std::shared_ptr< IWaitStrategy >& waitStrategy)
        : m_barrier(2)
        , m_sleepTime(static_cast< std::int32_t >(sleepTime))
        , m_waitStrategy(waitStrategy)
    {
    }

    void SequenceUpdater::run()
    {
        try
        {
            m_barrier.signal();
            m_barrier.wait();
            if (0 != m_sleepTime)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(m_sleepTime));
            }
            sequence->incrementAndGet();
            m_waitStrategy->signalAllWhenBlocking();
        }
        catch (std::exception& ex)
        {
            std::cout << ex.what() << std::endl;
        }
    }

    void SequenceUpdater::waitForStartup()
    {
        m_barrier.signal();
        m_barrier.wait();
    }

} // namespace Tests
} // namespace Disruptor
