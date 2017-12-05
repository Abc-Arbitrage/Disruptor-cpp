#include "stdafx.h"
#include "BasicExecutor.h"

#include "ITaskScheduler.h"


namespace Disruptor
{

    BasicExecutor::BasicExecutor(const std::shared_ptr< ITaskScheduler >& taskScheduler)
        : m_taskScheduler(taskScheduler)
    {
    }

    std::future< void > BasicExecutor::execute(const std::function< void() >& command)
    {
        return m_taskScheduler->scheduleAndStart(std::packaged_task< void() >([this, command] { executeCommand(command); }));
    }

    void BasicExecutor::executeCommand(const std::function< void() >& command)
    {
        try
        {
            command();
        }
        catch (...)
        { }
    }

} // namespace Disruptor
