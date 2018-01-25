#include "stdafx.h"

#include "ThreadPerTaskScheduler.h"


namespace Disruptor
{

    void ThreadPerTaskScheduler::start(std::int32_t)
    {
        if (m_started)
            return;

        m_started = true;
    }

    void ThreadPerTaskScheduler::stop()
    {
        if (!m_started)
            return;

        m_started = false;

        for (auto&& thread : m_threads)
        {
            if (thread.joinable())
                thread.join();
        }
    }

    std::future<void> ThreadPerTaskScheduler::scheduleAndStart(std::packaged_task<void()>&& task)
    {
        auto result = task.get_future();

        m_threads.emplace_back([this, task{ move(task) }] () mutable
        {
            while(m_started)
            {
                try
                {
                    task();
                }
                catch (...)
                {
                }
            }
        });

        return result;
    }

} // namespace Disruptor
