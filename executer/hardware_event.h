#ifndef _HARDWARE_EVENT_H_
#define _HARDWARE_EVENT_H_

#ifdef __linux__
#include <cstring>
#include <cstdint>
#include <sys/ioctl.h>
#include <asm/unistd.h>
#include <linux/perf_event.h>
#else
#error "hardware_event.h can only be used in Linux systems"
#endif

class HardwareEvent
{
    public :
        
        HardwareEvent(): m_cache_misses_count{0}
        {
            std::memset(&m_hardwareEvent, 0, sizeof(struct perf_event_attr));
            m_hardwareEvent.type = PERF_TYPE_HARDWARE;
            m_hardwareEvent.disabled = 1;
            m_hardwareEvent.exclude_kernel = 1;
            m_hardwareEvent.exclude_hv = 1;
        }

        ~HardwareEvent() { }
        
        
        bool start(int64_t event)
        {
            m_hardwareEvent.config = PERF_COUNT_HW_CACHE_MISSES;
            m_cache_misses = perf_event_open(0, -1, -1, 0);
            m_hardwareEvent.config = PERF_COUNT_HW_CACHE_REFERENCES;
            m_cache_references = perf_event_open(0, -1, m_cache_misses, 0);
            
            if (m_cache_misses == -1 || m_cache_references == -1)
            {
                return false;
            }
            
            ioctl(m_cache_misses, PERF_EVENT_IOC_RESET, 0);
            ioctl(m_cache_misses, PERF_EVENT_IOC_ENABLE, 0);
            return true;
        }
        
        void stop()
        {
            ioctl(m_cache_misses, PERF_EVENT_IOC_DISABLE, 0);
            read(m_cache_misses, &m_cache_misses_count, sizeof(long long));
            read(m_cache_references, &m_cache_references_count, sizeof(long long));
            close(m_cache_misses);
            close(m_cache_references);
        }
        
        long long getCacheMisses() const { return m_cache_misses_count; }

        long long getCacheReferences() const { return m_cache_references_count; }

        double getCacheMissRatio() const {
            return static_cast<double>(m_cache_misses_count)/static_cast<double>(m_cache_references_count);
        }
        
    private :
        long long m_cache_misses_count;
        long long m_cache_references_count;
        struct perf_event_attr m_hardwareEvent;
        int m_cache_misses;
        int m_cache_references;

        long perf_event_open(pid_t pid, int cpu, int group_fd, unsigned long flags)
        {
            //m_hardwareEvent.size = sizeof(struct perf_event_attr);
            int ret = syscall(__NR_perf_event_open, &m_hardwareEvent, pid, cpu, group_fd, flags);
            return ret;
        }
};

#endif