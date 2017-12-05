#pragma once

#include <cstdint>
#include <functional>


namespace Disruptor
{

    class SpinWait
    {
    public:
        SpinWait();

        std::int32_t count() const;
        bool nextSpinWillYield() const;
        
        void spinOnce();

        void reset();

        static void spinUntil(const std::function< bool() >& condition);
        static bool spinUntil(const std::function< bool() >& condition, std::int64_t  millisecondsTimeout);

        static std::int64_t getTickCount();

    private:
        static void spinWaitInternal(std::int32_t iterationCount);
        static void yieldProcessor();

    private:
        std::int32_t m_count;

        static const std::int32_t YIELD_THRESHOLD;
        static const std::int32_t SLEEP_0_EVERY_HOW_MANY_TIMES;
        static const std::int32_t SLEEP_1_EVERY_HOW_MANY_TIMES;
    };

} // namespace Disruptor
