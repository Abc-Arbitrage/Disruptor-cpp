#pragma once

#include <functional>


namespace Disruptor
{
namespace TestTools
{

    class ScopeExitFunctor
    {
    public:
        explicit ScopeExitFunctor(const std::function< void() >& func);

        ~ScopeExitFunctor();

    private:
        std::function< void() > m_func;
    };
    
} // namespace TestTools
} // namespace Disruptor
