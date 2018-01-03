#pragma once

#include "Disruptor/ITaskScheduler.h"


namespace Disruptor
{

    class ThreadPerTaskScheduler : public ITaskScheduler
    {
    public:
        void start(std::int32_t numberOfThreads = 0) override;
        void stop() override;

        std::future< void > scheduleAndStart(std::packaged_task< void() >&& task) override;

    private:
        std::atomic< bool > m_started{ false };
        std::vector< std::thread > m_threads;
    };

} // namespace Disruptor
