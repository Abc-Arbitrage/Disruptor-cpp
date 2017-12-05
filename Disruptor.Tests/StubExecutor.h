#pragma once

#include <atomic>
#include <cstdint>
#include <deque>

#include <boost/thread.hpp>

#include "Disruptor/IExecutor.h"


namespace Disruptor
{
namespace Tests
{

    class StubExecutor : public IExecutor
    {
    public:
        StubExecutor() 
           : m_ignoreExecutions(false)
           , m_executionCount(0)
        {}

        std::future< void > execute(const std::function< void() >& command) override;

        void joinAllThreads();

        void ignoreExecutions();

        std::int32_t getExecutionCount() const;

    private:
        std::recursive_mutex m_mutex;
        std::deque< boost::thread > m_threads;
        std::atomic< bool > m_ignoreExecutions;
        std::atomic< std::int32_t > m_executionCount;
    };

} // namespace Tests
} // namespace Disruptor

