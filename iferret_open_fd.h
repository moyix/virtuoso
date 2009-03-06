#ifndef __IFERRET_OPEN_FD_H_
#define __IFERRET_OPEN_FD_H_


typedef struct iferret_open_fd_struct_t {
  char *filename;
  int flags;
  int mode;
} iferret_open_fd_t;


void iferret_open_fd_add(iferret_t *iferret, int pid, int fd, char *filename,
			 int flags, int mode);

uint8_t iferret_open_fd_mem(iferret_t *iferret, int pid, int fd);

iferret_open_fd_t *iferret_open_fd_find(iferret_t *iferret, int pid, int fd);

void iferret_open_fd_remove(iferret_t *iferret, int pid, int fd);



#endif
