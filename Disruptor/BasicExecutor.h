#pragma once

#include <memory>

#include "Disruptor/IExecutor.h"


namespace Disruptor
{

    class ITaskScheduler;


    /**
     * TaskScheduler implementation for IExecutor
     */ 
    class BasicExecutor : public IExecutor
    {
    public:
        /**
         * Create a new BasicExecutor with a given TaskScheduler that will handle low-level queuing of commands execution.
         */ 
        explicit BasicExecutor(const std::shared_ptr< ITaskScheduler >& taskScheduler);

        /**
         * Start a new task executiong the given command in the current taskscheduler
         * \param command 
         */ 
        std::future< void > execute(const std::function< void() >& command) override;

    private:
        void executeCommand(const std::function< void() >& command);

    private:
        std::shared_ptr< ITaskScheduler > m_taskScheduler;
    };

} // namespace Disruptor
