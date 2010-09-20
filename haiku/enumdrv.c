#include <kernel/OS.h>
#include <kernel/image.h>
#include <stdio.h>
#include "vmnotify.h"

int32 real_list_drvs(uint32 *addrs) {
    image_info info;
    int32 i = 0;
    int32 cookie = 0;
    while (get_next_image_info(1, &cookie, &info) == B_OK) {
        addrs[i] = (uint32) info.text;
        i++;
    }
    return i;
}

int32 list_drvs(uint32 *addrs) {
    int i;   
    vm_mark_buf_in(&addrs, 4);
    i = real_list_drvs(addrs);
    vm_mark_buf_out(addrs, 1024);
    return i;
}

int main(int argc, char **argv) {
    int32 count, i;
    thread_id foo;
    uint32 *addrs = (uint32 *)malloc(1024);
    foo = spawn_thread(list_drvs, "list_drvs", B_REAL_TIME_PRIORITY, addrs);
    resume_thread(foo);
    wait_for_thread(foo, &count);
    for(i = 0; i < count; i++) {
        printf("%#x\n", addrs[i]);
    }
    return 0;
}
