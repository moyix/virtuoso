/* ©2011 Massachusetts Institute of Technology */
#include <kernel/OS.h>
#include <kernel/image.h>
#include <stdio.h>
#include "vmnotify.h"

typedef struct _data {
    uint32 base;
    char *name;
} data;

int32 real_find_drv(data *bar) {
    image_info info;
    int32 cookie = 0;
    while (get_next_image_info(1, &cookie, &info) == B_OK) {
        if ((uint32) info.text == bar->base) {
            strcpy(bar->name, info.name);
            return 1;
        }
    }
    return 0;
}

int32 find_drv(data *bar) {
    int i;   
    vm_mark_buf_in(&bar->base, 4);
    vm_mark_buf_in(&bar->name, 4);
    i = real_find_drv(bar);
    vm_mark_buf_out(bar->name, 1024);
    return i;
}

int main(int argc, char **argv) {
    int32 status, i;
    thread_id foo;
    data bar;
    char *name;
    bar.base = strtoul(argv[1], NULL, 0);
    bar.name = (char *) malloc(1024);
    foo = spawn_thread(find_drv, "find_drv", B_REAL_TIME_PRIORITY, &bar);
    resume_thread(foo);
    wait_for_thread(foo, &status);
    if(status)
        printf("%s\n", bar.name);
    else
        printf("Failed.\n");
    return 0;
}
