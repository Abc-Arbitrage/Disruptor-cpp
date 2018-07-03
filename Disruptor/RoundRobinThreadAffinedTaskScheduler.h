#pragma once

#include <atomic>
#include <cstdint>
#include <future>
#include <vector>

#include <boost/thread.hpp>

#include "Disruptor/BlockingQueue.h"
#include "Disruptor/ITaskScheduler.h"


namespace Disruptor
{

    /**
     * An implementation of TaskScheduler which creates an underlying thread pool and set processor affinity to each thread.
     */ 
    class RoundRobinThreadAffinedTaskScheduler : public ITaskScheduler
    {
    public:
        void start(std::int32_t numberOfThreads) override;
        void stop() override;

        std::future< void > scheduleAndStart(std::packaged_task< void() >&& task) override;

    private:
        void createThreads(std::int32_t numberOfThreads);
        void workingLoop(std::int32_t threadId);
        void tryExecuteTask(std::packaged_task< void() >& task);

        BlockingQueue< std::packaged_task< void() > > m_tasks;
        std::atomic< bool > m_started {false};
        std::vector< boost::thread > m_threads;
    };

} // namespace Disruptor
