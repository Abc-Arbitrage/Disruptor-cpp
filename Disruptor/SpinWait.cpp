#include "stdafx.h"
#include "SpinWait.h"

#include <limits>
#include <stdexcept>
#include <thread>

#include "BuildConfig.h"
#include "ClockConfig.h"


namespace Disruptor
{

    const std::int32_t SpinWait::YIELD_THRESHOLD = 10;
    const std::int32_t SpinWait::SLEEP_0_EVERY_HOW_MANY_TIMES = 5;
    const std::int32_t SpinWait::SLEEP_1_EVERY_HOW_MANY_TIMES = 20;


    SpinWait::SpinWait()
        : m_count(0)
    {
    }

    std::int32_t SpinWait::count() const
    {
        return m_count;
    }

    bool SpinWait::nextSpinWillYield() const
    {
        return m_count > YIELD_THRESHOLD;
    }

    void SpinWait::spinOnce()
    {
        if (nextSpinWillYield())
        {
            auto num = m_count >= YIELD_THRESHOLD ? m_count - 10 : m_count;
            if (num % SLEEP_1_EVERY_HOW_MANY_TIMES == SLEEP_1_EVERY_HOW_MANY_TIMES - 1)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
            else
            {
                if (num % SLEEP_0_EVERY_HOW_MANY_TIMES == SLEEP_0_EVERY_HOW_MANY_TIMES - 1)
                {
                    std::this_thread::sleep_for(std::chrono::milliseconds(0));
                }
                else
                {
                    std::this_thread::yield();
                }
            }
        }
        else
        {
            spinWaitInternal(4 << m_count);
        }

        if (m_count == std::numeric_limits< std::int32_t >::max())
            m_count = YIELD_THRESHOLD;
        else 
            ++m_count;
    }

    void SpinWait::reset()
    {
        m_count = 0;
    }

    void SpinWait::spinUntil(const std::function< bool() >& condition)
    {
        spinUntil(condition, -1);
    }

    bool SpinWait::spinUntil(const std::function< bool() >& condition, std::int64_t millisecondsTimeout)
    {
        if (millisecondsTimeout < -1)
            std::out_of_range("Timeout value is out of range");

        std::int64_t num = 0;
        if (millisecondsTimeout != 0 && millisecondsTimeout != -1)
            num = getTickCount();

        SpinWait spinWait;
        while (!condition())
        {
            if (millisecondsTimeout == 0)
                return false;

            spinWait.spinOnce();

            if (millisecondsTimeout != 1 && spinWait.nextSpinWillYield() && millisecondsTimeout <= (getTickCount() - num))
                return false;
        }

        return true;
    }

    std::int64_t SpinWait::getTickCount()
    {
        // TODO: not really portable
        return ClockConfig::Clock::now().time_since_epoch().count() / 1000;
    }

    void SpinWait::spinWaitInternal(std::int32_t iterationCount)
    {
        for (auto i = 0; i < iterationCount; i++)
            yieldProcessor();
    }

    void SpinWait::yieldProcessor()
    {
#if defined(DISRUPTOR_VC_COMPILER)

        ::YieldProcessor();

#elif defined(DISRUPTOR_GNUC_COMPILER)

        asm volatile
            (
                "rep\n"
                "nop"
                );

#else

# error "Unsupported platform"

#endif
    }

} // namespace Disruptor
