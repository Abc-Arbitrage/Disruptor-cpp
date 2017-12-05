#include "stdafx.h"
#include "RoundRobinThreadAffinedTaskScheduler.h"

#include <boost/date_time.hpp>

#include "ArgumentOutOfRangeException.h"
#include "ThreadHelper.h"


namespace Disruptor
{

    void RoundRobinThreadAffinedTaskScheduler::start(std::int32_t numberOfThreads)
    {
        if (m_started)
            return;

        m_started = true;

        if (numberOfThreads < 1)
            DISRUPTOR_THROW_ARGUMENT_OUT_OF_RANGE_EXCEPTION(numberOfThreads);

        createThreads(numberOfThreads);
    }

    void RoundRobinThreadAffinedTaskScheduler::stop()
    {
        if (!m_started)
            return;

        m_started = false;

        for (auto&& thread : m_threads)
        {
            if (thread.joinable())
                thread.timed_join(boost::posix_time::seconds(10));
        }
    }

    std::future< void > RoundRobinThreadAffinedTaskScheduler::scheduleAndStart(std::packaged_task< void() >&& task)
    {
        auto future = task.get_future();
        m_tasks.push(std::move(task));

        return future;
    }

    void RoundRobinThreadAffinedTaskScheduler::createThreads(std::int32_t numberOfThreads)
    {
        for (auto i = 0; i < numberOfThreads; ++i)
            m_threads.emplace_back([this, i]() { workingLoop(i); });
    }

    void RoundRobinThreadAffinedTaskScheduler::workingLoop(std::int32_t threadId)
    {
        static const auto processorCount = std::thread::hardware_concurrency();

        const auto processorIndex = threadId % processorCount;

        const auto affinityMask = ThreadHelper::AffinityMask(1ull << processorIndex);

        ThreadHelper::setThreadAffinity(affinityMask);

        while (m_started)
        {
            std::packaged_task< void() > task;
            while (m_tasks.timedWaitAndPop(task, std::chrono::milliseconds(100)))
            {
                tryExecuteTask(task);
            }
        }
    }

    void RoundRobinThreadAffinedTaskScheduler::tryExecuteTask(std::packaged_task< void() >& task)
    {
        task();
    }

} // namespace Disruptor
