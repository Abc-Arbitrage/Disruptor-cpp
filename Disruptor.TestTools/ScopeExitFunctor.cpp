#include "stdafx.h"
#include "ScopeExitFunctor.h"


namespace Disruptor
{
namespace TestTools
{

    ScopeExitFunctor::ScopeExitFunctor(const std::function< void() >& func)
        : m_func(func)
    {
    }

    ScopeExitFunctor::~ScopeExitFunctor()
    {
        if (m_func)
            m_func();
    }

} // namespace TestTools
} // namespace Disruptor
