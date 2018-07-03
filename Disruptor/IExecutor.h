#pragma once

#include <future>


namespace Disruptor
{

    /**
     * Replace the Executor interface in java.util.concurrent
     */ 
    class IExecutor
    {
    public:
        virtual ~IExecutor() = default;

        /**
         * Execute the given command in an other thread
         *
         * \param command The command to execute
         */ 
        virtual std::future< void > execute(const std::function< void() >& command) = 0;
    };

} // namespace Disruptor
