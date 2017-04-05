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
        
        HardwareEvent(): m_count{0}
        {
            std::cout << "Constructor" << std::endl;
            std::memset(&m_hardwareEvent, 0, sizeof(struct perf_event_attr));
            m_hardwareEvent.type = PERF_TYPE_HARDWARE;
            m_hardwareEvent.size = sizeof(struct perf_event_attr);
            m_hardwareEvent.disabled = 1;
            m_hardwareEvent.exclude_kernel = 1;
            m_hardwareEvent.exclude_hv = 1;
        }

        ~HardwareEvent()
        {
            close(m_descriptor);
        }
        
        
        bool start(int64_t event)
        {
            if( HardwareEvent::isValidEvent(event) == false )
            {
                return false;
            }

            m_hardwareEvent.config = event;
            
            if ( m_descriptor = perf_event_open(0, -1, -1, 0) != 0 )
            {
                return false;
            }
            
            ioctl(m_descriptor, PERF_EVENT_IOC_RESET, 0);
            ioctl(m_descriptor, PERF_EVENT_IOC_ENABLE, 0);
            return true;
        }
        
        void stop()
        {
            ioctl(m_descriptor, PERF_EVENT_IOC_DISABLE, 0);
            read(m_descriptor, &m_count, sizeof(long long));
        }
        
        long long getCount() const { return m_count; }
        
    private :
        long long m_count;
        struct perf_event_attr m_hardwareEvent;
        int m_descriptor;

        long perf_event_open(pid_t pid, int cpu, int group_fd, unsigned long flags)
        {
            int ret;
            ret = syscall(__NR_perf_event_open, &m_hardwareEvent, pid, cpu, group_fd, flags);
            return ret;
        }
        
        static bool isValidEvent(int64_t event)
        {
            bool ret{false};
            switch(event)
            {
                case PERF_COUNT_HW_CPU_CYCLES:
                case PERF_COUNT_HW_INSTRUCTIONS:
                case PERF_COUNT_HW_CACHE_REFERENCES:
                case PERF_COUNT_HW_CACHE_MISSES:
                case PERF_COUNT_HW_BRANCH_INSTRUCTIONS:
                case PERF_COUNT_HW_BRANCH_MISSES:
                case PERF_COUNT_HW_BUS_CYCLES:
                case PERF_COUNT_HW_STALLED_CYCLES_FRONTEND:
                case PERF_COUNT_HW_STALLED_CYCLES_BACKEND:
                case PERF_COUNT_HW_REF_CPU_CYCLES:
                    ret = true;
                    break;
                default:
                    break;
            }
            return ret;
        }
};

#endif