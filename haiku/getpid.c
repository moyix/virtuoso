#include <kernel/OS.h>
#include <stdio.h>
#include "vmnotify.h"

uint32 getps(int32 dummy) {
    team_info info;   
    uint32 i;
    vm_mark_buf_in(NULL, 0);
    get_team_info(B_CURRENT_TEAM, &info);
    i = info.team;
    vm_mark_buf_out(&i, 4);
    return i;
}

int main(int argc, char **argv) {
    uint32 pid;
    thread_id foo;
    foo = spawn_thread(getps, "getps", B_REAL_TIME_PRIORITY, 0);
    resume_thread(foo);
    wait_for_thread(foo, &pid);
    printf("%d\n", pid);
    return 0;
}
