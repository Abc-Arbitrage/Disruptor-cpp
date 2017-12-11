#include "stdafx.h"
#include "StubExecutor.h"


namespace Disruptor
{
namespace Tests
{

    std::future< void > StubExecutor::execute(const std::function< void() >& command)
    {
        ++m_executionCount;

        std::future< void > result;

        if (!m_ignoreExecutions)
        {
            std::packaged_task< void() > task(command);
            result = task.get_future();

            std::lock_guard< decltype(m_mutex) > lock(m_mutex);
            m_threads.push_back(boost::thread(std::move(task)));
        }

        return result;
    }

    void StubExecutor::joinAllThreads()
    {
        std::lock_guard< decltype(m_mutex) > lock(m_mutex);

        while (!m_threads.empty())
        {
            boost::thread thread(std::move(m_threads.front()));
            m_threads.pop_front();

            if (thread.joinable())
            {
                try
                {
                    thread.interrupt();
                    thread.timed_join(boost::posix_time::milliseconds(5000));
                }
                catch (std::exception& ex)
                {
                    std::cout << ex.what() << std::endl;
                }
            }

            BOOST_CHECK_MESSAGE(thread.joinable() == false, "Failed to stop thread: " << thread.get_id());
        }
    }

    void StubExecutor::ignoreExecutions()
    {
        m_ignoreExecutions = true;
    }

    std::int32_t StubExecutor::getExecutionCount() const
    {
        return m_executionCount;
    }

} // namespace Tests
} // namespace Disruptor
