#include "stdafx.h"
#include "ThreadHelper.h"

#include "BuildConfig.h"

#ifdef DISRUPTOR_OS_FAMILY_MACOS

#include <string>

#include <pthread.h>
#include <unistd.h>
#include <sched.h>
#include <cpuid.h>
#include <sys/sysctl.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <mach/mach_types.h>
#include <mach/thread_act.h>

#define SYSCTL_CORE_COUNT   "machdep.cpu.core_count"

#define CPUID(INFO, LEAF, SUBLEAF) __cpuid_count(LEAF, SUBLEAF, INFO[0], INFO[1], INFO[2], INFO[3])

#define GETCPU(CPU) {                              \
        uint32_t CPUInfo[4];                           \
        CPUID(CPUInfo, 1, 0);                          \
        /* CPUInfo[1] is EBX, bits 24-31 are APIC ID */ \
        if ( (CPUInfo[3] & (1 << 9)) == 0) {           \
          (CPU) = -1;  /* no APIC on chip */             \
        }                                              \
        else {                                         \
          (CPU) = (unsigned)CPUInfo[1] >> 24;                    \
        }                                              \
        if ((CPU) < 0) (CPU) = 0;                          \
      }

namespace Disruptor
{
namespace ThreadHelper
{
    typedef struct cpu_set {
        uint32_t    count;
    } cpu_set_t;

    static inline void CPU_ZERO(cpu_set_t *cs) { cs->count = 0; }

    static inline void CPU_SET(int num, cpu_set_t *cs) { cs->count |= (1 << num); }

    static inline int CPU_ISSET(int num, cpu_set_t *cs) { return (cs->count & (1 << num)); }

    int sched_getaffinity(pid_t pid, size_t cpu_size, cpu_set_t *cpu_set)
    {
        (void) pid;
        (void) cpu_size;

        int32_t core_count = 0;
        size_t  len = sizeof(core_count);
        int ret = sysctlbyname(SYSCTL_CORE_COUNT, &core_count, &len, 0, 0);
        if (ret) {
            return -1;
        }
        cpu_set->count = 0;
        for (int i = 0; i < core_count; i++) {
            cpu_set->count |= (1 << i);
        }

        return 0;
    }

    int pthread_setaffinity_np(pthread_t thread, size_t cpu_size,
                               cpu_set_t *cpu_set)
    {
        thread_port_t mach_thread;
        int core = 0;

        for (core = 0; core < 8 * (int)cpu_size; core++) {
            if (CPU_ISSET(core, cpu_set)) break;
        }

        thread_affinity_policy_data_t policy = { core };
        mach_thread = pthread_mach_thread_np(thread);
        thread_policy_set(mach_thread, THREAD_AFFINITY_POLICY,
                          (thread_policy_t)&policy, 1);
        return 0;
    }


    size_t getProcessorCount()
    {
        return static_cast<size_t>(sysconf(_SC_NPROCESSORS_CONF));
    }

    uint32_t getCurrentProcessor()
    {
        uint32_t cpu_id;

        GETCPU(cpu_id);

        return cpu_id;
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
        if (sched_getaffinity(0, sizeof(cpuSet), &cpuSet) == 0)
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
        uint64_t result;
        pthread_threadid_np(NULL, &result);
        return (uint32_t) result;
    }

    void setThreadName(const std::string& name)
    {
        pthread_setname_np(name.c_str());
    }

} // namespace ThreadHelper
} // namespace Disruptor

#endif // DISRUPTOR_OS_FAMILY_UNIX
