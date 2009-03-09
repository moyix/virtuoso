/*
  maintains a mapping from (pid,fd) to file info,
  assuming we saw the open of that file for that process 
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "iferret_log.h"
#include "int_string_hashtable.h"
#include "int_int_hashtable.h"
#include "int_set.h"
#include "iferret.h"
#include "iferret_open_fd.h"


// key is pid/fd.  val is filename/flags/mode.  add key->val mapping.
void iferret_open_fd_add(iferret_t *iferret, int pid, int fd, char *filename,
			 int flags, int mode) {
  iferret_open_fd_t *ofd_info;
  int_int_hashtable_t *ofd_table;
  ofd_info = (iferret_open_fd_t *) malloc (sizeof (iferret_open_fd_t));
  ofd_info->filename = strdup(filename);
  ofd_info->flags = flags;
  ofd_info->mode = mode;
  if (int_int_hashtable_mem(iferret->open_fd_table, pid)) {
    // okay we have already have a table for this pid
    ofd_table = (int_int_hashtable_t *) int_int_hashtable_find(iferret->open_fd_table, pid);
  }
  else {
    // no table yet for that pid.  create it first.  
    ofd_table = int_int_hashtable_new();
    int_int_hashtable_add(iferret->open_fd_table, pid, (uint64_t) ofd_table);
  }
  if (int_int_hashtable_mem(ofd_table, fd)) {
    // Hey, we already have an entry for this pid/fd. wtf? shouldnt happen
    iferret_open_fd_t *ofd_info2;
    ofd_info2 = (iferret_open_fd_t *) int_int_hashtable_find(ofd_table, fd);
    printf ("We appear to be opening the same fd twice?\n");
    printf ("pid=%d fd=%d\n", pid, fd);
    printf ("old info: filename=%s flags=%d mode=%d\n", 
	    ofd_info2->filename, ofd_info2->flags, ofd_info2->mode);
    printf ("new info: filename=%s flags=%d mode=%d\n", 
	      ofd_info->filename, ofd_info->flags, ofd_info->mode);
    exit(1);
  }
  // finally add ptr to open fd data to pid/fd table 
  int_int_hashtable_add(ofd_table, fd, (uint64_t) ofd_info);
}


// key is pid/fd.  look for key mapping.
uint8_t iferret_open_fd_mem(iferret_t *iferret, int pid, int fd) {
  int_int_hashtable_t *ofd_table;
  if (int_int_hashtable_mem(iferret->open_fd_table, pid)) {
    // found the pid table.
    ofd_table = (int_int_hashtable_t *) int_int_hashtable_find(iferret->open_fd_table, pid);
    return (int_int_hashtable_mem(ofd_table, fd));
  }
  // no pid table
  return (0);
}


// key is pid/fd.  find val it maps to and return it.  or fail. 
iferret_open_fd_t *iferret_open_fd_find(iferret_t *iferret, int pid, int fd) {
  iferret_open_fd_t *ofd_info;
  int_int_hashtable_t *ofd_table;
  if (int_int_hashtable_mem(iferret->open_fd_table, pid)) {
    // okay we have already have a table for this pid
    ofd_table = (int_int_hashtable_t *) int_int_hashtable_find(iferret->open_fd_table, pid);
    if (int_int_hashtable_mem(ofd_table, fd)) {
      // both 1st and 2nd level keys present.  return pointer. 
      return  (iferret_open_fd_t *) (int_int_hashtable_find(ofd_table, fd));
    }
    else {
      printf ("iferret_open_fd_find: pid=%d is there but fd=%d isnt.\n", pid, fd);
      exit (1);
    }
  }
  else {
    printf ("iferret_open_fd_find: pid=%d isnt there.\n", pid);
    exit (1);
  }
  return (NULL);
}


// remove key (and val it maps to) from table.  Also free whatever
void iferret_open_fd_remove(iferret_t *iferret, int pid, int fd) {
  iferret_open_fd_t *ofd_info;
  int_int_hashtable_t *ofd_table;
  if (int_int_hashtable_mem(iferret->open_fd_table, pid)) {
    // we have a table for this pid
    ofd_table = (int_int_hashtable_t *) int_int_hashtable_find(iferret->open_fd_table, pid);
  }
  else {
    // no table yet for that pid.  return
    return;
  }
  if (int_int_hashtable_mem(ofd_table, fd)) {
    // Also there is an entry for the fd in that pid's table. 
    // free the value data
    ofd_info = (iferret_open_fd_t *) int_int_hashtable_find(ofd_table, fd);
    free(ofd_info->filename);
    free(ofd_info);
    // remove the key/val at 2nd level
    int_int_hashtable_remove(ofd_table, fd);
  }
  // remove key/value at 1st level.
  int_int_hashtable_remove(iferret->open_fd_table, pid);
}

  


  
