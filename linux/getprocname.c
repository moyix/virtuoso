/* ©2011 Massachusetts Institute of Technology */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <proc/readproc.h>
#include "vmnotify.h"

int main(int argc, char **argv) {
    int i,j;
    proc_t **procs;
    char *name = malloc(256);
    pid_t mypid = atoi(argv[1]);

    printf("Searching for pid: %d\n", mypid);

    vm_mark_buf_in(&mypid, sizeof(pid_t));
    vm_mark_buf_in(&name, sizeof(char *));
    procs = readproctab(PROC_FILLCOM);
    for (i = 0; procs[i]; i++) {
        if (procs[i]->tid == mypid) {
            if(!procs[i]->cmdline) {
                name[0] = '\0';
            }
            else {
                strncpy(name, procs[i]->cmdline[0], 256);
            }
        }
    }
    vm_mark_buf_out(name, 256);

    printf("%s\n", name);
    
    return 0;
}
