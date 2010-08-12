#include <stdio.h>
#include <proc/readproc.h>
#include "vmnotify.h"

int main(int argc, char **argv) {
    int i,j;
    proc_t **procs;
    int *pids = malloc(1024);

    vm_mark_buf_in(&pids, 4);
    procs = readproctab(0);
    for (i = 0; procs[i]; i++) {
        pids[i] = procs[i]->tid;
    }
    vm_mark_buf_out(pids, 1024);

    for (i = 0; procs[i]; i++) {
        printf("%d\n", pids[i]);
    }
    
    return 0;
}
