#define _GNU_SOURCE 1

#include <asm/unistd.h>
#include <fcntl.h>
#include <linux/perf_event.h>
#include <linux/hw_breakpoint.h>
#include <sys/ioctl.h>
#include <asm/unistd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/syscall.h>

using namespace std;

long perf_event_open(struct perf_event_attr* event_attr, pid_t pid, int cpu, int group_fd, unsigned long flags)
{
    return syscall(__NR_perf_event_open, event_attr, pid, cpu, group_fd, flags);
}

static void perf_event_handler(int signum, siginfo_t* info, void* ucontext) {
    if(info->si_code != POLL_HUP) {
        // Only POLL_HUP should happen.
        exit(EXIT_FAILURE);
    }
    
    ioctl(info->si_fd, PERF_EVENT_IOC_REFRESH, 1);
}

int main(int argc, char** argv)
{
    // Configure signal handler
    struct sigaction sa;
    memset(&sa, 0, sizeof(struct sigaction));
    sa.sa_sigaction = perf_event_handler;
    sa.sa_flags = SA_SIGINFO;
    
    // Setup signal handler
    if (sigaction(SIGIO, &sa, NULL) < 0) {
        fprintf(stderr,"Error setting up signal handler\n");
        perror("sigaction");
        exit(EXIT_FAILURE);
    }
    
    // Configure perf_event_attr struct
    struct perf_event_attr pe;
    memset(&pe, 0, sizeof(struct perf_event_attr));
    pe.type = PERF_TYPE_HARDWARE;
    pe.size = sizeof(struct perf_event_attr);
    pe.config = PERF_COUNT_HW_INSTRUCTIONS;     // Count retired hardware instructions
    pe.disabled = 1;        // Event is initially disabled
    pe.sample_type = PERF_SAMPLE_IP;
    pe.sample_period = 1000;
    pe.exclude_kernel = 1;      // excluding events that happen in the kernel-space
    pe.exclude_hv = 1;          // excluding events that happen in the hypervisor
    
    pid_t pid = 0;  // measure the current process/thread
    int cpu = -1;   // measure on any cpu
    int group_fd = -1;
    unsigned long flags = 0;
    
    int fd = perf_event_open(&pe, pid, cpu, group_fd, flags);
    if (fd == -1) {
        fprintf(stderr, "Error opening leader %llx\n", pe.config);
        perror("perf_event_open");
        exit(EXIT_FAILURE);
    }
    
    // Setup event handler for overflow signals
    fcntl(fd, F_SETFL, O_NONBLOCK|O_ASYNC);
    fcntl(fd, F_SETSIG, SIGIO);
    fcntl(fd, F_SETOWN, getpid());
    
    ioctl(fd, PERF_EVENT_IOC_RESET, 0);     // Reset event counter to 0
    ioctl(fd, PERF_EVENT_IOC_REFRESH, 1);   // 
    
    // Start monitoring
    
    long loopCount = 1000000;
    long c = 0;
    long i = 0;
    
    // Some sample payload.
    for(i = 0; i < loopCount; i++) {
        c += 1;
    }
    
    // End monitoring
    
    ioctl(fd, PERF_EVENT_IOC_DISABLE, 0);   // Disable event
    
    long long counter;
    read(fd, &counter, sizeof(long long));  // Read event counter value
    
    printf("Used %lld instructions\n", counter);
    
    close(fd);
}