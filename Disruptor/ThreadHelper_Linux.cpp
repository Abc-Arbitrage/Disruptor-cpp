#include "stdafx.h"
#include "ThreadHelper.h"

#include "BuildConfig.h"

#ifdef DISRUPTOR_OS_FAMILY_LINUX

#include <string>

#include <pthread.h>
#include <unistd.h>
#include <sched.h>
#include <sys/prctl.h>
#include <sys/syscall.h>
#include <sys/types.h>

namespace Disruptor
{
namespace ThreadHelper
{

    size_t getProcessorCount()
    {
        return sysconf(_SC_NPROCESSORS_CONF);
    }

    uint32_t getCurrentProcessor()
    {
        return sched_getcpu();
    }

    bool setThreadAffinity(const AffinityMask& mask)
    {
        cpu_set_t cpuSet;
        CPU_ZERO(&cpuSet);

        for (size_t i = 0; i < mask.size(); ++i)
        {
            if (mask.test(i))
                CPU_SET(i, &cpuSet);
        }

        return pthread_setaffinity_np(pthread_self(), sizeof(cpuSet), &cpuSet) == 0;
    }

    AffinityMask getThreadAffinity()
    {
        AffinityMask mask;

        cpu_set_t cpuSet;
        CPU_ZERO(&cpuSet);
        if (pthread_getaffinity_np(pthread_self(), sizeof(cpuSet), &cpuSet) == 0)
        {
            int processorCount = getProcessorCount();
            int maskSize = (int) mask.size();
            for (int i = 0; i < processorCount && i < maskSize; ++i)
            {
                if (CPU_ISSET(i, &cpuSet))
                    mask.set(i);
            }
        }

        return mask;
    }

    uint32_t getCurrentThreadId()
    {
        return (uint32_t) syscall(SYS_gettid);
    }

    void setThreadName(const std::string& name)
    {
        prctl(PR_SET_NAME, name.c_str(), 0, 0, 0);
    }

} // namespace ThreadHelper
} // namespace Disruptor

#endif // DISRUPTOR_OS_FAMILY_UNIX
