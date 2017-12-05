#pragma once

#include <memory>

#include "Disruptor/IExecutor.h"


namespace Disruptor
{

    class ITaskScheduler;


    /// <summary>
    /// TaskScheduler implementation for IExecutor
    /// </summary>
    class BasicExecutor : public IExecutor
    {
    public:
        /// <summary>
        /// Create a new <see cref="BasicExecutor"/> with a given <see cref="TaskScheduler"/>
        /// that will handle low-level queuing of commands execution.
        /// </summary>
        explicit BasicExecutor(const std::shared_ptr< ITaskScheduler >& taskScheduler);

        /// <summary>
        /// Start a new task executiong the given command in the current taskscheduler
        /// </summary>
        /// <param name="command"></param>
        std::future< void > execute(const std::function< void() >& command) override;

    private:
        void executeCommand(const std::function< void() >& command);

    private:
        std::shared_ptr< ITaskScheduler > m_taskScheduler;
    };

} // namespace Disruptor
