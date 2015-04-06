/* ©2011 Massachusetts Institute of Technology */
#include <stdio.h>
#include "vmnotify.h"

// FWDREF
typedef struct _link link;

typedef struct _link {
    int i;
    link *next;
} link;

int do_stuff() {
    int *p;
    link *head;
    link *cur;
    link *next;
    int i;
    int s;

    s = 0;
    head = (link *) malloc(sizeof(link));
    cur = head;
    cur->i = 0;
    cur->next = NULL;
    for (i = 1; i < 256; i++) {
        next = (link *) malloc(sizeof(link));
        cur->next = next;
        cur = next;
        cur->i = i;
        cur->next = NULL;
    }

    for (cur = head; cur != NULL; cur = cur->next) {
        s += cur->i;
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
