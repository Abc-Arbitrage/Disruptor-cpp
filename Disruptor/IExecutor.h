#pragma once

#include <future>


namespace Disruptor
{

    /// <summary>
    /// Replace the Executor interface in java.util.concurrent
    /// </summary>
    class IExecutor
    {
    public:
        virtual ~IExecutor() = default;

        /// <summary>
        /// Execute the given command in an other thread
        /// </summary>
        /// <param name="command">The command to execute</param>
        virtual std::future< void > execute(const std::function< void() >& command) = 0;
    };

} // namespace Disruptor
