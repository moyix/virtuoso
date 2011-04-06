/* ©2011 Massachusetts Institute of Technology */
#include <kernel/OS.h>
#include <stdio.h>
#include "vmnotify.h"

uint32 gettime(int32 *dummy) {
    uint32 i;   
    vm_mark_buf_in(NULL, 0);
    i = real_time_clock();
    vm_mark_buf_out(&i, 4);
    return i;
}

int main(int argc, char **argv) {
    uint32 time;
    thread_id foo;
    foo = spawn_thread(gettime, "gettime", B_REAL_TIME_PRIORITY, 0);
    resume_thread(foo);
    wait_for_thread(foo, &time);
    printf("%d\n", time);
    return 0;
}
