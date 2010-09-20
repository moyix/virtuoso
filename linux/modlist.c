#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include "vmnotify.h"

int main(int argc, char **argv) {
    unsigned char *buf;
    unsigned char *buf2;
    int fd, n;
    int x, y, i;
    
    buf2 = malloc(1024);
    vm_mark_buf_in(&buf2, sizeof(unsigned char *));
    x = 0;
    y = 0;
    i = 0;
    buf = malloc(8192);
    fd = open("/proc/modules", O_RDONLY);

    while(n = read(fd, buf+i, 8192)) {
        i += n;
    }
    n = i;
    i = 0;

    while(1) {
        while(buf[i] != ' ' && i < n) i++;
        if (i >= n) break;
        buf[i] = '\0';
        memcpy(buf2+y, buf+x, strlen(buf+x)+1);
        y += strlen(buf+x)+1;
        while(buf[i] != '\n' && i < n) i++;
        x = i+1;
        if (i >= n) break;
    }
    vm_mark_buf_out(buf2, 1024);
    
    for (i = 0; i < y; i = i + (strlen(buf2+i)+1)) {
        printf("%s\n", buf2+i);
    }

    return 0;
}
