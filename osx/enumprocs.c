#include <stdlib.h>
#include <stdio.h>
#include <sys/sysctl.h>
#include <mach/mach_init.h>
#include <mach/thread_policy.h>
#include <sched.h>
#include "vmnotify.h"
 
int set_realtime(int period, int computation, int constraint) {
    struct thread_time_constraint_policy ttcpolicy;
    int ret;
 
    ttcpolicy.period = period; // HZ/160
    ttcpolicy.computation = computation; // HZ/3300;
    ttcpolicy.constraint = constraint; // HZ/2200;
    ttcpolicy.preemptible = 0;
 
    if ((ret=thread_policy_set(mach_thread_self(),
        THREAD_TIME_CONSTRAINT_POLICY, (thread_policy_t)&ttcpolicy,
        THREAD_TIME_CONSTRAINT_POLICY_COUNT)) != KERN_SUCCESS) {
            fprintf(stderr, "set_realtime() failed.\n");
            return 0;
    }
    return 1;
}
 
int main (int argc, const char * argv[])
{
    int mib[4];
    int i;
    size_t buffer_size, num_procs;
    struct kinfo_proc *pproc, *proc_list;    
    pid_t *pids;

    pids = malloc(1024);

    set_realtime(500000,1000000,1000000);

    vm_mark_buf_in(&pids, 4);
    mib[0] = CTL_KERN;
    mib[1] = KERN_PROC;
    mib[2] = KERN_PROC_ALL;
    mib[3] = 0;
    sysctl(mib, 4, NULL, &buffer_size, NULL, 0);
    proc_list = malloc(buffer_size);
    sysctl(mib, 4, proc_list, &buffer_size, NULL, 0);
    num_procs = buffer_size / sizeof(struct kinfo_proc);
    for (i = 0; i < num_procs; i++) {
        pids[i] = (proc_list+i)->kp_proc.p_pid;
    }
    vm_mark_buf_out(pids, 1024);
 
    for (i = 0; i < num_procs; i++) {
        printf("%d\n", pids[i]);
    }
    free(proc_list);
    free(pids);
    printf("Note: malloc at %p\n", malloc);
    return 0;
}
