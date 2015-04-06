/* ©2011 Massachusetts Institute of Technology */
#include <stdio.h>
#include "vmnotify.h"

int main(int argc, char **argv) {
    pid_t pid;
    vm_mark_buf_in(NULL, 0);
    pid = getpid();
    vm_mark_buf_out(&pid, sizeof(pid_t));

    return 0;
}
