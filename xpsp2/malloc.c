#include <stdio.h>
#include "vmnotify.h"

int do_stuff() {
    int *p;
    int i;
    int s;

    s = 0;
    p = (int *) malloc(sizeof(int)*256);
    for (i = 0; i < 256; i++) {
        p[i] = i;
    }
    for (i = 0; i < 256; i++) {
        if (i % 8)  s += p[i];
        else        s ^= p[i];
    }
    return s;
}

int main(int argc, char **argv) {
    int res;
    vm_mark_buf_in(NULL, 0);
    res = do_stuff();
    vm_mark_buf_out(&res, sizeof(int));
    printf("%d\n", res);
}
