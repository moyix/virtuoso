/* ©2011 Massachusetts Institute of Technology */
#include <stdio.h>
#include <time.h>
#include "vmnotify.h"

int main(int argc, char **argv) {
    time_t the_time;
    vm_mark_buf_in(NULL, 0);
    the_time = time(NULL);
    vm_mark_buf_out(&the_time, sizeof(time_t));

    printf("%d\n", the_time);

    return 0;
}
