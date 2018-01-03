#include "stdafx.h"
#include "ThreadHelper.h"

#include "BuildConfig.h"

#ifdef DISRUPTOR_OS_FAMILY_WINDOWS

namespace Disruptor
{
namespace ThreadHelper
{

    std::uint32_t getCurrentThreadId()
    {
        return static_cast< std::uint32_t >(::GetCurrentThreadId());
    }

    std::uint32_t getCurrentProcessor()
    {
        return ::GetCurrentProcessorNumber();
    }

    std::size_t getProcessorCount()
    {
        SYSTEM_INFO systemInfo;
        GetSystemInfo(&systemInfo);
        return systemInfo.dwNumberOfProcessors;
    }

    bool setThreadAffinity(const AffinityMask& mask)
    {
        return ::SetThreadAffinityMask(::GetCurrentThread(), mask.to_ullong()) != 0;
    }

    AffinityMask getThreadAffinity()
    {
        DWORD_PTR temp = 1;
        DWORD_PTR current = ::SetThreadAffinityMask(::GetCurrentThread(), temp);
        if (current)
            ::SetThreadAffinityMask(::GetCurrentThread(), current);
        return current;
    }

#if _DEBUG
    #define MS_VC_EXCEPTION 0x406D1388
 
    #pragma pack(push,8)
    typedef struct tagTHREADNAME_INFO
    {
        DWORD dwType; // Must be 0x1000.
        LPCSTR szName; // Pointer to name (in user addr space).
        DWORD dwThreadID; // Thread ID (-1=caller thread).
        DWORD dwFlags; // Reserved for future use, must be zero.
    } THREADNAME_INFO;
    #pragma pack(pop)

    void setThreadName(const std::string& name)
    {
        setThreadName(-1, name);
    }

    void setThreadName(int threadId, const std::string& name)
    {
        std::this_thread::sleep_for(std::chrono::microseconds(10));
        THREADNAME_INFO info;
        info.dwType = 0x1000;
        info.szName = name.c_str();
        info.dwThreadID = threadId;
        info.dwFlags = 0;
 
        __try
        {
            RaiseException(MS_VC_EXCEPTION, 0, sizeof(info) / sizeof(ULONG_PTR), reinterpret_cast< ULONG_PTR* >(&info));
        }
        __except(EXCEPTION_EXECUTE_HANDLER)
        {
        }
    }
#else // _DEBUG
    void setThreadName(const std::string& /*name*/) { }
    void setThreadName(int /*threadId*/, const std::string& /*name*/) { }
#endif // _DEBUG

} // namespace ThreadHelper
} // namespace Disruptor

#endif // ABC_OS_FAMILY_WINDOWS
