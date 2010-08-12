#include <kernel/OS.h>
#include <stdio.h>
#include "vmnotify.h"

int32 real_list_procs(int32 *pids) {
    team_info info;
    int32 i = 0;
    int32 cookie = 0;
    while (get_next_team_info(&cookie, &info) == B_OK) {
        pids[i] = info.team;
        i++;
    }
    return i;
}

int32 list_procs(int32 *pids) {
    int i;   
    vm_mark_buf_in(&pids, 4);
    i = real_list_procs(pids);
    vm_mark_buf_out(pids, 1024);
    return i;
}

int main(int argc, char **argv) {
    int32 count, i;
    thread_id foo;
    int32 *pids = (int32 *)malloc(1024);
    foo = spawn_thread(list_procs, "list_procs", B_REAL_TIME_PRIORITY, pids);
    resume_thread(foo);
    wait_for_thread(foo, &count);
    for(i = 0; i < count; i++) {
        printf("%d\n", pids[i]);
    }
    return 0;
}
