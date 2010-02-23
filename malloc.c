#include <stdio.h>
#include "vmnotify.h"

int main(int argc, char **argv) {
    int *p;
    int i;
    int s = 0;

    vm_mark_buf_in(NULL, 0);
    p = (int *) malloc(sizeof(int)*256);
    for (i = 0; i < 256; i++) {
        p[i] = i;
    }
    for (i = 0; i < 256; i++) {
        if (i % 8)  s += p[i];
        else        s ^= p[i];
    }
    vm_mark_buf_out(&s, sizeof(int));
    printf("%d\n", s);
}
