
// Note: 
// system call prototypes
// from  /usr/src/linux-headers-2.6.24-19/include/linux/syscalls.h
// Look in /usr/include/asm/unistd_32.h for the syscall numbers

// Note, from http://www.win.tue.nl/~aeb/linux/lk/lk-4.html
// "The Linux Kernel: Linux System Calls"
// On i386, the parameters of a system call are transported via registers. 
// The system call number goes into %eax, 
// The parameters are (%ebx, %ecx, %edx, %esi, %edi, %ebp)
// More than 6 param?  Not sure what happens.  


#include <string.h>
#include <assert.h>
#include "exec.h"
#include "lookup_table.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/uio.h>
#include <poll.h>
#include <ctype.h>
#include <sys/types.h>
#include <unistd.h>

#include "linux_task_struct_offsets.h"

extern struct CPUX86State *env;
extern pid_t pid;
extern uid_t uid;

target_phys_addr_t cpu_get_phys_addr(CPUState *env, target_ulong addr);


pid_t current_pid, last_pid;
uid_t current_uid, last_uid;

uint8_t no_pid_flag = 1;
uint8_t no_uid_flag = 1;

/* Argument list sizes for sys_socketcall */
#define AL(x) ((x) * sizeof(unsigned long))
static unsigned char nargs[18]={AL(0),AL(3),AL(3),AL(3),AL(2),AL(3),
                                AL(3),AL(3),AL(4),AL(4),AL(4),AL(6),
                                AL(6),AL(2),AL(5),AL(5),AL(3),AL(3)};




// current_task is pointer to vm physical memory at which linux task structure is
// located. 
// slot_offset is where in task_struct to find this slot.
// slot_size is size of data for that slot, in bytes.
// dest is where to copy that data
static inline void copy_task_struct_slot(target_ulong current_task, uint32_t slot_offset,
                           uint32_t slot_size, char *dest) {
  target_phys_addr_t paddr;

  assert (slot_size > 0);
  bzero(dest,slot_size);
  paddr = cpu_get_phys_addr(env, current_task + slot_offset);
  if (paddr != -1) {
    cpu_physical_memory_read(paddr, dest, slot_size);
  }
}


// copy string of length at most len-1
// from physical adddress physaddr into buffer tempbuf
// null terminate
// NB: assumes tempbuf allocated!
static inline void copy_string_phys(char *tempbuf, target_phys_addr_t physaddr, uint32_t len) {
  assert (len > 0);
  bzero(tempbuf, len);
  cpu_physical_memory_read((unsigned long) physaddr, tempbuf, len-1);
}


// str assumed to be at least 120 bytes allocated.
// vaddr is virt addr of a string. 
// return 0 on failure, 1 on success.
int copy_string(char *str, uint32_t vaddr) { 
  target_phys_addr_t paddr;
  paddr = cpu_get_phys_addr(env,vaddr);
  if (paddr == -1) {
    return 0;
  }
  copy_string_phys(str,paddr,120);
  return (1);
}
  


static inline target_ulong get_task_struct_ptr (target_ulong current_esp) {
  target_phys_addr_t paddr;
  target_ulong current_task;
  // find current_task, the ptr to the currently executing process' task_struct
  paddr = cpu_get_phys_addr(env, (target_ulong) (current_esp & CURRENT_TASK_MASK)); 
  if (paddr!=-1) {
    cpu_physical_memory_read(paddr, (char *) &current_task, 4);
    return (current_task);
  }
  assert (paddr != -1);
  return (0);
}


void get_current_pid_uid() {
  target_ulong current_task; 
  current_task = get_task_struct_ptr(ESP);
  copy_task_struct_slot(current_task, PID_OFFSET, PID_SIZE, (char *) &current_pid);
  copy_task_struct_slot(current_task, UID_OFFSET, UID_SIZE, (char *) &current_uid);  
}


static inline uint32_t get_uint32_t_phys(uint32_t virt_addr) {
  target_phys_addr_t paddr;
  uint32_t retval;
  paddr = cpu_get_phys_addr(env, virt_addr);
  if (paddr!=-1) {
    cpu_physical_memory_read((unsigned long) paddr, (char*) &retval, sizeof(uint32_t));
  }    
  return(retval);
}




// write an entry to iferret log to capture
// context (eip & pid), number, and arguments of 
// current system call 
void iferret_log_syscall_enter (uint8_t is_sysenter, uint32_t eip_for_callsite) {
  
  target_phys_addr_t paddr; 
  char tempbuf[1204];
  target_ulong current_task;
  char command[COMM_SIZE];
  char str1[MAX_STRING_LEN], str2[MAX_STR_LEN];
  int pid, uid, len, i;

  // find current_task, the ptr to the currently executing process' task_struct
  current_task = get_task_struct_ptr(ESP);

  // grab process id, uid and command string. 
  copy_task_struct_slot(current_task, PID_OFFSET, PID_SIZE, (char *) &pid);
  copy_task_struct_slot(current_task, UID_OFFSET, UID_SIZE, (char *) &uid);
  copy_task_struct_slot(current_task, COMM_OFFSET, COMM_SIZE, command);
  
  init_table();

  
  if ((EAX==11) || (EAX==119)) {
    return;
  }

  {
    Info_flow_syscall_t sc, *scp;

    scp = &sc;
    scp->op_num = EAX + IFLO_SYS_CALLS_START + 1;
    if (EAX==102) {
      // sys_socketcall has 17 sub-possibilities.
      // EBX=1 (socket) ... EBX=17 (recvmsg)
      // each with its own arg format.
      scp->op_num += EBX;
    }
    if (EAX>102) {
      scp->op_num += 18;
    }
    
    scp->is_sysenter = is_sysenter;
    scp->pid = pid;
    scp->callsite_eip = eip_for_callsite;
    scp->command = command;

    // manage Ryan's stack
    add_element(pid,eip_for_callsite,EAX);    
  
    // fprintf(logfile, "PID: %d, stack size:%d\n",pid,get_stack_size(pid));

    /////////////////////////////////////
    // the syscalls, by the numbers
  
    switch (EAX) {	
    case 0 : 
      // long sys_restart_syscall(void);
      info_flow_log_syscall_write(scp);
      break;
    case 1 : 
      // long sys_exit(int error_code);
      info_flow_log_syscall_write(scp,EBX);
      break;
    case 2 :
      // sys_fork is missing from syscalls.h
      // Here is what man 2 fork gets:
      // pid_t fork(void);
      info_flow_log_syscall_write(scp);
      break;
    case 3 : 
      // ssize_t sys_read(unsigned int fd, char __user *buf,
      //	                size_t count);
      info_flow_log_syscall_write(scp,EBX);      
      break;
    case 4 :
      // ssize_t sys_write(unsigned int fd, const char __user *buf,
      //                   size_t count);
      info_flow_log_syscall_write(scp,EBX);
      break;
    case 5 :
      // long sys_open(const char __user *filename,
      //               int flags, int mode);
      if (copy_string(str1,EBX)) {
	info_flow_log_syscall_write(scp,str1,ECX,EDX);
      }
      break;
    case 6 :
      // long sys_close(unsigned int fd);
      info_flow_log_syscall_write(scp,EBX);            
      break;
    case 7 :
      // long sys_waitpid(pid_t pid, int __user *stat_addr, int options);
      info_flow_log_syscall_write(scp);      
      break;
    case 8 : 
      // long sys_creat(const char __user *pathname, int mode);    
      if (copy_string(str1,EBX)) {
	info_flow_log_syscall_write(scp,str1,ECX);      
      }
      break;
    case 9: 
      // long sys_link(const char __user *oldname,
      //	             const char __user *newname);
      if (copy_string(str1,EBX)) {
	if (copy_string(str2,ECX)) {
	  info_flow_log_syscall_write(scp,str1,str2);
	}
      }
      break;
    case 10 : 
      // long sys_unlink(const char __user *pathname);
      if (copy_string(str1,EBX)) {
	info_flow_log_syscall_write(scp,str1);      
      }
      break;
    case 11: 
      // sys_execve is missing from syscalls.h 
      // This one is there, though? 
      // long sys_execve(char *filename, char **argv, char **envp, struct pt_regs regs)
      // And here's what we get from man 2 execve:
      //       int execve(const char *filename, char *const argv[],
      //                  char *const envp[]);
      //
      // Hmm.  we don't really have a way to write an info-flow op 
      // with an arbitrary number of arguments.  
      // Punt for now and toss the args.  
      if (copy_string(str1,EBX)) {
	info_flow_log_syscall_write(scp,str1);      
      }
      /*
      {
        int i,n;
        char filename[120], char arg[120], **orig_argvp, **argvp;
	target_phys_addr_t paddr;

        // first, get the filename of the program to be executed
        paddr = cpu_get_phys_addr(env, EBX);
        if (paddr != -1) {
          copy_string_phys(filename, paddr, 120);	
          paddr = cpu_get_phys_addr(env, ECX);
          cpu_physical_memory_read(paddr, (char *) &argvp, 4);  // The first argument
          orig_argvp = argvp;      
          // next, figure out how many args
          n=0;
          while (argvp && (n<12)) {
            n++;
            paddr = cpu_get_phys_addr(env, ECX+n*4);
            cpu_physical_memory_read(paddr, (char *) &argvp, 4);  // next arg
          }
          i = 0;
          // write op plus num args 
	  IFLS_I(EXECVE,n);
	  argvp = orig_argvp;
	  for (i=0; i<n; i++) {
            paddr = cpu_get_phys_addr(env, ECX+i*4);	
            copy_string_phys(name, paddr, 120); // arg 
            IFLW_PUT_STRING(name);
          }
	  */
        }
      }
      break;
    case 12 :
      //  long sys_chdir(const char __user *filename);
      if (copy_string(str1,EBX)) {
	info_flow_log_syscall_write(scp,str1);
      }
      break;
    case 13 : 
      // long sys_time(time_t __user *tloc);
      info_flow_log_syscall_write(scp);
      break;
    case 14 :
      // long sys_mknod(const char __user *filename, int mode,
      //	              unsigned dev);
      if (copy_string(str1,EBX)) {
        info_flow_log_syscall_write(scp,str1,ECX,EDX);
      }
      break;
    case 15 :
      // long sys_chmod(const char __user *filename, mode_t mode);
      if (copy_string(str1,EBX)) {
        info_flow_log_syscall_write(scp,str1,ECX);
      }
      break;
    case 16 : 
      // long sys_lchown(const char __user *filename,
      //                 uid_t user, gid_t group);
      if (copy_string(str1,EBX)) {
        info_flow_log_syscall_write(scp,str1,ECX,EDX);
      }
      break;
    case 17 :
      // sys_break missing from syscalls.h
      info_flow_log_syscall_write(scp);
      break;
    case 18 :
      // sys_oldstat missing from syscalls.h
      // Xuxian seems to know ebx is a ptr to a string
      if (copy_string(str1,EBX)) {
        info_flow_log_syscall_write(IFLO_SYS_OLDSTAT,str1);      
      }
      break;
    case 19 :
      // off_t sys_lseek(unsigned int fd, off_t offset,
      //                 unsigned int origin);
      info_flow_log_syscall_write(scp,EBX,ECX,EDX);
      break;
    case 20 :
      // long sys_getpid(void);
      info_flow_log_syscall_write(scp);
      break;
    case 21: 
      // long sys_mount(char __user *dev_name, char __user *dir_name,
      //                char __user *type, unsigned long flags,
      //	              void __user *data);
      if (copy_string(str1,EBX)) {
        if (copy_string(str2,ECX)) {
          if (copy_string(str3,EDX)) {
            info_flow_log_syscall_write(scp,str1,str2,ESI);
          }
        }
      }
      break;
    case 22 :
      // long sys_umount(char __user *name, int flags);
      if (copy_string(str1,EBX)) {
        info_flow_log_syscall_write(IFLO_SYS_UMOUNT,str1,ECX);
      }
      break;
    case 23 : 
      // long sys_setuid(uid_t uid);
      info_flow_log_syscall_write(scp);
      break;
    case 24 : 
      // long sys_getuid(void);
      info_flow_log_syscall_write(scp);
      break;
    case 25 :
      // long sys_stime(time_t __user *tptr);
      info_flow_log_syscall_write(scp);
      break;
    case 26 :
      // long sys_ptrace(long request, long pid, long addr, long data);
      info_flow_log_syscall_write(scp,EBX,ECX,EDX);
      break;
    case 27 :
      // unsigned long sys_alarm(unsigned int seconds);
      info_flow_log_syscall_write(IFLO_SYS_ALARM,EBX);
      break;
    case 28 : 
      // sys_oldfstat not in syscalls.h
      // I think this is really oldfstat.  can't find a prototype
      info_flow_log_syscall_write(scp);
      break;
    case 29 : 
      //  long sys_pause(void);
      info_flow_log_syscall_write(scp);
      break;
    case 30 :     
      // long sys_utime(char __user *filename,
      //                struct utimbuf __user *times);
      if (copy_string(str1,EBX)) {
        info_flow_log_syscall_write(scp,str1);
      }
      break;
    case 31 : 
      // sys_stty not in syscalls.h
      info_flow_log_syscall_write(IFLO_SYS_STTY);
      break;
    case 32 : 
      // sys_gtty not in syscalls.h
      info_flow_log_syscall_write(scp);
      break;
    case 33 :
      // long sys_access(const char __user *filename, int mode);
      if (copy_string(str1,EBX)) {
        info_flow_log_syscall_write(scp,str1,ECX);
      break;
    case 34 :
      // long sys_nice(int increment);
      info_flow_log_syscall_write(scp,EBX);
      break;
    case 35 : 
      // sys_ftime not in syscalls.h
      info_flow_log_syscall_write(scp);
      break;
    case 36 : 
      // long sys_sync(void);
      info_flow_log_syscall_write(scp);
      break;
    case 37 :
      // long sys_kill(int pid, int sig);
      info_flow_log_syscall_write(scp,EBX,ECX);
      break;
    case 38: 
      //  long sys_rename(const char __user *oldname,
      //	                const char __user *newname);
      if (copy_string(str1,EBX)) {
        if (copy_string(str2,ECX)) {
          info_flow_log_syscall_write(scp,str1,str2);
        }
      }
      break;
    case 39: 
      // long sys_mkdir(const char __user *pathname, int mode);
      if (copy_string(str1,EBX)) {
        info_flow_log_syscall_write(scp,str1,EBX);
      }
      break;
    case 40: 
      //  long sys_rmdir(const char __user *pathname);
      if (copy_string(str1,EBX)) {
        info_flow_log_syscall_write(scp,str1);
      }
      break;
    case 41: 
      //  long sys_dup(unsigned int fildes);
      info_flow_log_syscall_write(scp,EBX);
      break;
    case 42:
      // sys_pipe is missing from syscalls.h. 
      //        int pipe(int pipefd[2]);
      info_flow_log_syscall_write(scp,EBX,ECX,EDX);
      //    paddr = cpu_get_phys_addr(env, EBX);
      //    if (paddr!=-1)	{      
      //      fprintf(logfile,"PID %3d (%16s)", pid, command);
      //      fprintf(logfile,"[sys_pipe     42]\n");
      //} 
      break;
    case 43 : 
      // long sys_times(struct tms __user *tbuf);
      info_flow_log_syscall_write(scp);
      break;
    case 44 : 
      // sys_prof missing from syscalls.h
      info_flow_log_syscall_write(Iscp);
      break;
    case 45 : 
      // unsigned long sys_brk(unsigned long brk);
      info_flow_log_syscall_write(scp,EBX);
      break;
    case 46 :
      // long sys_setgid(gid_t gid);
      info_flow_log_syscall_write(scp,EBX);
      break;
    case 47 : 
      // long sys_getgid(void);
      info_flow_log_syscall_write(scp);
      break;
    case 48 : 
      // unsigned long sys_signal(int sig, __sighandler_t handler);
      info_flow_log_syscall_write(scp,EBX,ECX);
      break;
    case 49 : 
      // long sys_geteuid(void);
      info_flow_log_syscall_write(scp);
      break;
    case 50 :
      // long sys_getegid(void);
      info_flow_log_syscall_write(scp);
      break;
    case 51 : 
      // long sys_acct(const char __user *name);
      if (copy_string(str1,EBX)) {
        info_flow_log_syscall_write(scp,str1);
      }
      break;
    case 52 : 
      // sys_umount2 is missing from syscalls.h
      // Xuxian used EBX as a ptr to a string tho...
      // man 2 umount 2 gets:
      //        int umount2(const char *target, int flags);
      if (copy_string(str1,EBX)) {
        info_flow_log_syscall_write(scp,str1);
      }
      break;
    case 53 : 
      // sys_lock is missing from syscalls.h
      info_flow_log_syscall_write(scp);
      break;
    case 54 : 
      // long sys_ioctl(unsigned int fd, unsigned int cmd,
      //                unsigned long arg);
      info_flow_log_syscall_write(scp,EBX,ECX,EDX);
      break;
    case 55 : 
      // long sys_fcntl(unsigned int fd, unsigned int cmd, unsigned long arg);
      info_flow_log_syscall_write(scp,EBX,ECX,EDX);
      break;
    case 56 :
      // sys_mpx is missing from syscalls.h
      info_flow_log_syscall_write(scp);
      break;
    case 57 : 
      // long sys_setpgid(pid_t pid, pid_t pgid);    
      info_flow_log_syscall_write(scp,EBX,ECX);
      break;
    case 58 : 
      // sys_ulimit is missing from syscalls.h
      info_flow_log_syscall_write(scp);
      break;
    case 59 :
      // sys_oldolduname is missing from syscalls.h
      info_flow_log_syscall_write(scp);
      break;
    case 60 :
      // long sys_umask(int mask);
      info_flow_log_syscall_write(scp);
      break;
    case 61 :
      // long sys_chroot(const char __user *filename);
      if (copy_string(str1,EBX)) {
        info_flow_log_syscall_write(scp,str1);
      }
      break;
    case 62 :
      // long sys_ustat(unsigned dev, struct ustat __user *ubuf);
      info_flow_log_syscall_write(scp);
      break;
    case 63 : 
      // long sys_dup2(unsigned int oldfd, unsigned int newfd);
      info_flow_log_syscall_write(scp,EBX,ECX);
      break;
    case 64 : 
      // long sys_getppid(void);
      info_flow_log_syscall_write(scp);
      break;
    case 65 : 
      // long sys_getpgrp(void);
      info_flow_log_syscall_write(scp);
      break;
    case 66 : 
      // long sys_setsid(void);
      info_flow_log_syscall_write(scp);
      break;
    case 67 :
      // sys_sigaction is missing from syscalls.h
      info_flow_log_syscall_write(scp,EBX);
      break;
    case 68 :
      // long sys_sgetmask(void);
      info_flow_log_syscall_write(scp);
      break;
    case 69 : 
      // long sys_ssetmask(int newmask);
      info_flow_log_syscall_write(scp,EBX);
      break;
    case 70 : 
      // long sys_setreuid(uid_t ruid, uid_t euid);
      info_flow_log_syscall_write(scp,EBX,ECX);
      break;
    case 71 :
      // long sys_setregid(gid_t rgid, gid_t egid);
      info_flow_log_syscall_write(scp,EBX,ECX);
      break;
    case 72 : 
      // sys_sigsuspend is missing from syscalls.h
      info_flow_log_syscall_write(scp);
      break;
    case 73 : 
      // long sys_sigpending(old_sigset_t __user *set);
      info_flow_log_syscall_write(scp);
      break;
    case 74 : 
      // long sys_sethostname(char __user *name, int len);
      if (copy_string(str1,EBX)) {
        info_flow_log_syscall_write(scp,str1,ECX);
      }
      break;
    case 75 : 
      // long sys_setrlimit(unsigned int resource,
      //                    struct rlimit __user *rlim);
      // might want that ptr?
      info_flow_log_syscall_write(scp,EBX,ECX);
      break;
    case 76 : 
      // long sys_getrlimit(unsigned int resource,
      //                    struct rlimit __user *rlim);
      info_flow_log_syscall_write(scp,EBX);
      break;
    case 77 : 
      //  long sys_getrusage(int who, struct rusage __user *ru);
      info_flow_log_syscall_write(scp,EBX);
      break;
    case 78 : 
      // long sys_gettimeofday(struct timeval __user *tv,
      //                       struct timezone __user *tz);
      info_flow_log_syscall_write(scp);
      break;
    case 79 : 
      // long sys_settimeofday(struct timeval __user *tv,
      //                       struct timezone __user *tz);
      info_flow_log_syscall_write(scp);
      break;
    case 80 :
      // long sys_getgroups(int gidsetsize, gid_t __user *grouplist);
      info_flow_log_syscall_write(scp);
      break;
    case 81 : 
      // long sys_setgroups(int gidsetsize, gid_t __user *grouplist);
      info_flow_log_syscall_write(scp);
      break;
    case 82 :
      // long sys_select(int n, fd_set __user *inp, fd_set __user *outp,
      // fd_set __user *exp, struct timeval __user *tvp);
      //      info_flow_log_syscall_write(IFLO_SYS_SELECT,EBX);
      // TRL 2009 02 06 probably not useful?
      info_flow_log_syscall_write(scp);
      /*
      {
        int fd, *ptr;
	fprintf(logfile,"PID %3d (%16s)[sys_select   82]: ", pid, command);
        if ( EBX > 0 ){
          if (!ECX) {
            paddr = cpu_get_phys_addr(env, ECX);
            //	  fprintf(logfile, "readfds");
            bzero(tempbuf, 120);
            cpu_physical_memory_read(paddr, tempbuf, EBX*4); //-> get the fdset 
            i=0; ptr = (int*)tempbuf; fd = *ptr++;
            while (fd ) {
              //	    fprintf(logfile, " %d", fd);
              IFLW_PUT_UINT32_T(fd);
              i++;	
              if ( i<EBX) fd = *ptr++;
              else fd = 0;
            }
          }
          if (!EDX) {
            paddr = cpu_get_phys_addr(env, EDX);
            //	  fprintf(logfile, "writefds");
            bzero(tempbuf, 120);
            cpu_physical_memory_read(paddr, tempbuf, EBX*4); //-> get the fdset 
            i=0; ptr = (int*)tempbuf; fd = *ptr++;
            while (fd ) {
              //	    fprintf(logfile, " %d", fd);
              IFLW_PUT_UINT32_T(fd);
              i++;	
              if ( i<EBX) fd = *ptr++;
              else fd = 0;
            }
          }
        }
        //      fprintf(logfile, "\n");
      }
      */
      break;
    case 83 :
      // long sys_symlink(const char __user *old, const char __user *new);
      if (copy_string(str1,EBX)) {
        if (copy_string(str2,ECX)) {
          info_flow_log_syscall_write(scp,str1,str2);
        }
      }
      break;
    case 84 :
      // sys_oldlstat is missing from syscalls.h
      // Xuxian takes EBX to be a ptr to a string.
      if (copy_string(str1,EBX)) {
        info_flow_log_syscall_write(scp,str1);
      }
      break;
    case 85 :
      // long sys_readlink(const char __user *path,
      //                   char __user *buf, int bufsiz);
      if (copy_string(str1,EBX)) {
        info_flow_log_syscall_write(scp,str1);
      }
      break;
    case 86 : 
      // long sys_uselib(const char __user *library);
      //                   char __user *buf, int bufsiz);
      if (copy_string(str1,EBX)) {
        info_flow_log_syscall_write(scp,str1);
      }
      break;
    case 87 : // sys_swapon
      // long sys_swapon(const char __user *specialfile, int swap_flags);
      if (copy_string(str1,EBX)) {
        info_flow_log_syscall_write(scp,str1);
      }
      break;
    case 88 :
      // long sys_reboot(int magic1, int magic2, unsigned int cmd,
      //                 void __user *arg);    
      info_flow_log_syscall_write(scp);
      break;
    case 89 : 
      // sys_readdir is missing from syscalls.h
      //
      // Here's what man 2 readdir gets:
      //        int readdir(unsigned int fd, struct dirent *dirp,
      //                    unsigned int count);
      //
      if (copy_string(str1,ECX)) {
        info_flow_log_syscall_write(scp,EBX,str1);
      } 
      break;
    case 90 :
      // sys_mmap is missing from syscalls.h
      //
      // Here's what man 2 mmap gets:
      //       void *mmap(void *addr, size_t length, int prot, int flags,
      //                  int fd, off_t offset);
      //
      // This is Xuxian's decoding
      {
        char buf[24];
        int addr, fd, *ptr;
        paddr = cpu_get_phys_addr(env, EBX);
        if (paddr!=-1)	{
          copy_string_phys(buf, paddr, 24);
          ptr = (int *)buf;
          addr = *ptr++;
          len = *ptr++;
          ptr++; ptr++;
          fd = *ptr++;
	  info_flow_log_syscall_write(scp,addr,len,fd);
        }
      }
      break;
    case 91 :
      //  long sys_munmap(unsigned long addr, size_t len);
      info_flow_log_syscall_write(scp,EBX,ECX);
      break;
    case 92 : 
      // long sys_truncate(const char __user *path,
      //                   unsigned long length);
      if (copy_string(str1,EBX)) {
        info_flow_log_syscall_write(scp,str1,ECX);
      }
      break;
    case 93 : 
      // long sys_ftruncate(unsigned int fd, unsigned long length);
      info_flow_log_syscall_write(scp,EBX,ECX);
      break;
    case 94 : 
      // long sys_fchmod(unsigned int fd, mode_t mode);
      info_flow_log_syscall_write(scp,EBX,ECX);
      break;
    case 95 : 
      // long sys_fchown(unsigned int fd, uid_t user, gid_t group);
      info_flow_log_syscall_write(scp,EBX,ECX,EDX);
      break;
    case 96 : 
      // long sys_getpriority(int which, int who);
      info_flow_log_syscall_write(scp,EBX,ECX);
      break;
    case 97 : 
      // long sys_setpriority(int which, int who, int niceval);
      info_flow_log_syscall_write(scp,EBX,ECX);
      break;
    case 98 :
      // sys_profil is missing from syscalls.h
      info_flow_log_syscall_write(scp);
      break;
    case 99 : 
      // long sys_statfs(const char __user * path,
      //                 struct statfs __user *buf);
      info_flow_log_syscall_write(scp,EBX);
      break;
    case 100 :
      // long sys_fstatfs(unsigned int fd, struct statfs __user *buf);
      info_flow_log_syscall_write(scp,EBX);
      break;
    case 101 : 
      // long sys_ioperm(unsigned long from, unsigned long num, int on);
      info_flow_log_syscall_write(scp,EBX,ECX,EDX);
      break;
    case 102 : 
      // long sys_socketcall(int call, unsigned long __user *args);
      //      IFLS_I(SOCKETCALL,EBX);
      switch (EBX) {
      case 1: // socket
        { 
          int family, type, protocol, *ptr;
          paddr = cpu_get_phys_addr(env, ECX);
          if (paddr!=-1)	{
            bzero(tempbuf, 120);
            cpu_physical_memory_read(paddr, tempbuf, nargs[EBX] ); //-> get the args
            ptr = (int*) tempbuf;
            family = *ptr++; type =*ptr++; protocol = *ptr++;
	    //            IFLW_PUT_UINT32_T(family);
	    //            IFLW_PUT_UINT32_T(type);
	    //            IFLW_PUT_UINT32_T(protocol);
	    info_flow_log_syscall_write(scp,EBX,family,type,protocol);
          }
        }
        break;
      case 2: // bind
        {
          int fd, len, group, *ptr; struct sockaddr_in *sap;
          unsigned char b0, b1, b2, b3, *bptr, b4, b5, b6, b7; 
          unsigned short *sptr;
          unsigned char pkttype, halen; // , sll_addr[8];
          paddr = cpu_get_phys_addr(env, ECX);
          if (paddr!=-1)	{
	    target_ulong sap_addr;
            bzero(tempbuf, 120);
            cpu_physical_memory_read(paddr, tempbuf, nargs[EBX] ); //-> get the args
            ptr = (int*) tempbuf;
            fd = *ptr++;
	    sap_addr = (target_ulong) *ptr++; 
	    len= *ptr++;
            paddr = cpu_get_phys_addr(env, sap_addr);
            bzero(tempbuf, 120);
            cpu_physical_memory_read(paddr, tempbuf, 120); //-> get the args
            //bptr = (unsigned char*)&sa.sin_addr.s_addr;
            bptr = tempbuf;
            b0=*bptr++; b1=*bptr++; b2=*bptr++; b3=*bptr++;
            b4=*bptr++; b5=*bptr++; b6=*bptr++; b7=*bptr++;
            bptr = tempbuf; bptr+=2;
            sap = (struct sockaddr_in*) tempbuf;
            ptr = (int*) tempbuf; ptr++; pid =*ptr++;  group = *ptr++;
            if (sap->sin_family == 1 ) { // PF_LOCAL
	      info_flow_log_syscall_write(scp,EBX,sap->sin_family,fd, bptr);
              //	    fprintf(logfile,"family  1; fd %d; file %s", fd, bptr);
            }
            else if (sap->sin_family == 2 ) { // PF_INET
	      char addrPort[1024];
	      sprintf(addrPort,"%d.%d.%d.%d:%d",b4,b5,b6,b7,b2*256+b3);
	      info_flow_log_syscall_write(scp,EBX,sap->sin_family,fd,addrPort);
              //	    fprintf(logfile,"family  2; fd %d; %d.%d.%d.%d:%d", 
              //		    fd, b4, b5, b6, b7, b2*256+b3);
            }
            else if (sap->sin_family == 16 ) {// PF_NETLINK
	      info_flow_log_syscall_write(scp,EBX,sap->sin_family,fd);				     
              //	    fprintf(logfile,"family 16; fd %d; pid %d; group %d", 
              //		    fd, pid, group);
            }
            else if (sap->sin_family == 17 )  {// PF_PACKET
              char addr[1024];
              bptr = tempbuf; bptr += 8; sptr = (unsigned short*)bptr;
              bptr +=2; pkttype =*bptr; halen =*bptr++;
	      //                fprintf(logfile,"family 17; fd %d; protocol 0x%x; ifindex 0x%x",
	      //                fd, b2*256+b3, pid);
	      //                fprintf(logfile,"family 17; fd %d; protocol %d; ifindex %d; hatype %d; pkttype %d; halen %d; addr %02x-%02x-%02x-%02x-%02x-%02x-%02x-%02x", 
	      //                sap->sin_family, fd, b2*256+b3, pid, *sptr, pkttype, halen, 
	      //                tempbuf[12], tempbuf[13], tempbuf[14], tempbuf[15],
	      //                tempbuf[16], tempbuf[17], tempbuf[18], tempbuf[19]);
              sprintf(addr,"%02x-%02x-%02x-%02x-%02x-%02x-%02x-%02x", 
                      tempbuf[12], tempbuf[13], tempbuf[14], tempbuf[15],
                      tempbuf[16], tempbuf[17], tempbuf[18], tempbuf[19]);
              info_flow_log_syscall_write(scp,EBX,sap->sin_family,fd,*sptr,pktype,halen,addr);
            } else {
              char godknowswhat[1024];
              // fprintf(logfile,"family %2d; fd %d; %d.%d.%d.%d-%d.%d.%d.%d(%c%c%c%c%c%c%c%c); len %d", 
              //		    sap->sin_family, fd, b0, b1, b2, b3,b4, b5, b6, 
              //		    b7, b0, b1, b2, b3,b4, b5, b6, b7, len);
              sprintf(godknowswhat,"%d.%d.%d.%d-%d.%d.%d.%d(%c%c%c%c%c%c%c%c", 
                      b0, b1, b2, b3,b4, b5, b6, 
                      b7, b0, b1, b2, b3,b4, b5, b6, b7);
              info_flow_log_syscall_write(scp,EBX,sap->sin_family,fd,godknowswhast);
            }
          }
        }
        break;
      case 3: // connect
        {
          int fd, len, *ptr; struct sockaddr_in *sap;
          unsigned char b0, b1, b2, b3, *bptr, b4, b5, b6, b7; 
          //	fprintf(logfile,"PID %3d (%16s)[sys_connect 102]%d: ", pid, command, EBX);
          paddr = cpu_get_phys_addr(env, ECX);
          if (paddr!=-1)	{
	    target_ulong sap_addr;
            bzero(tempbuf, 120);
            cpu_physical_memory_read(paddr, tempbuf, nargs[EBX] ); //-> get the args
            ptr = (int*) tempbuf;
            fd = *ptr++; 
	    sap_addr = *ptr++; 
	    len = *ptr++;
            paddr = cpu_get_phys_addr(env, (target_ulong) sap_addr);
            bzero(tempbuf, 120);
            cpu_physical_memory_read(paddr, tempbuf, 120); //-> get the args
            bptr = tempbuf;
            b0=*bptr++; b1=*bptr++; b2=*bptr++; b3=*bptr++;
            b4=*bptr++; b5=*bptr++; b6=*bptr++; b7=*bptr++;
            bptr = tempbuf; bptr+=2;
            sap = (struct sockaddr_in*) tempbuf;
            if (sap->sin_family == 1) {// PF_LOCAL
              //	    fprintf(logfile,"socket %d; family  1; file %s", fd, bptr);
              info_flow_log_syscall_write(scp,EBX,sap->sin_family,fd,bptr);
            }
            else if (sap->sin_family == 2) { // PF_INET
              char addrport[1024];
              //	    fprintf(logfile,"socket %d; family  2; %d.%d.%d.%d:%d", 
              //		    fd, b4, b5, b6, b7, b2*256+b3);
              sprintf(addrport,"%d.%d.%d.%d:%d", b4, b5, b6, b7, b2*256+b3);              
              info_flow_log_syscall_write(scp,EBX,sap->sin_family,fd,addrport);
            }
            else {
              //	    fprintf(logfile,"socket %d; family %2d;",
              //		    fd, sap->sin_family);
              info_flow_log_syscall_write(scp,EBX,sap->sin_family,fd);
            }
          }
          //	fprintf(logfile,"\n");
        }        
        break;
      case 4: // listen
        //      fprintf(logfile,"PID %3d (%16s)[sys_listen  102]%d: ", pid, command, EBX);
        paddr = cpu_get_phys_addr(env, ECX);
        if (paddr!=-1)	{
          bzero(tempbuf, 120);
          cpu_physical_memory_read(paddr, tempbuf, nargs[EBX] ); //-> get the args
          //	fprintf(logfile,"socket %d", *(int*) tempbuf);
          info_flow_log_syscall_write(scp,EBX, *((int*) tempbuf));
        }
        //      fprintf(logfile,"\n");
        break;
      case 5: // accept
        //      fprintf(logfile,"PID %3d (%16s)[sys_accept  102]%d: ", pid, command, EBX);
        paddr = cpu_get_phys_addr(env, ECX);
        if (paddr!=-1)	{
          bzero(tempbuf, 120);
          cpu_physical_memory_read(paddr, tempbuf, nargs[EBX] ); //-> get the args
          //	fprintf(logfile,"socket %d", *(int*) tempbuf);
          info_flow_log_syscall_write(scp,EBX,*(int*) tempbuf); 
        }
        //      fprintf(logfile,"\n");      
        break;
      case 6: // getsockname
        //      fprintf(logfile,"PID %3d (%16s)[sys_getskna 102]%d: ", pid, command, EBX);
        paddr = cpu_get_phys_addr(env, ECX);
        if (paddr!=-1)	{
          bzero(tempbuf, 120);
          cpu_physical_memory_read(paddr, tempbuf, nargs[EBX] ); //-> get the args
          //	fprintf(logfile,"socket %d", *(int*) tempbuf);
          info_flow_log_syscall_write(scp,EBX,*(int*) tempbuf); 
        }
        //      fprintf(logfile,"\n");      
        break;
      case 7: // getpeername
        //      fprintf(logfile,"PID %3d (%16s)[sys_getpeer 102]%d: ", pid, command, EBX);
        paddr = cpu_get_phys_addr(env, ECX);
        if (paddr!=-1)	{
          bzero(tempbuf, 120);
          cpu_physical_memory_read(paddr, tempbuf, nargs[EBX] ); //-> get the args
          //	fprintf(logfile,"socket %d", *(int*) tempbuf);
          info_flow_log_syscall_write(scp,EBX,*(int*) tempbuf); 
        }
        //      fprintf(logfile,"\n");     
        break;
      case 8: 
        {// socketpair
          int domain, type, protocol, *ptr;
	  // int * socket_vector;
          //      fprintf(logfile,"PID %3d (%16s)[sys_skpair  102]%d: ", pid, command, EBX);
          paddr = cpu_get_phys_addr(env, ECX);
          if (paddr!=-1)	{
            bzero(tempbuf, 120);
            cpu_physical_memory_read(paddr, tempbuf, nargs[EBX] ); //-> get the args
            ptr = (int *) tempbuf;
            domain =*ptr++; 
	    type=*ptr++; 
	    protocol=*ptr++; 
	    //	    socket_vector= (int *)*ptr++;
            //	fprintf(logfile,"domain %d; type %d; protocol %d\n", *(int*) tempbuf);
	    /*
            IFLW_PUT_UINT32_T(domain);
            IFLW_PUT_UINT32_T(type);
            IFLW_PUT_UINT32_T(protocol);
	    */
            info_flow_log_syscall_write(scp,EBX,domain,type,protocol,*(int*)tempbuf); 
          }
        }          
        break;    
      case 9: // send
        {
          int fd, msg, len, *ptr;
          //	fprintf(logfile,"PID %3d (%16s)[sys_send    102]%d: ", pid, command, EBX);
          paddr = cpu_get_phys_addr(env, ECX);
          if (paddr!=-1)	{
            bzero(tempbuf, 120);
            cpu_physical_memory_read(paddr, tempbuf, nargs[EBX] ); //-> get the args
            ptr = (int*) tempbuf;
            fd = *ptr++; msg=*ptr++; len= *ptr++;
            //	  fprintf(logfile,"socket %d ", fd);
            
            bzero(tempbuf, 120);
            paddr = cpu_get_phys_addr(env, msg);
            cpu_physical_memory_read(paddr, tempbuf, 30); 
            for (i=0; i<30; i++)
              if (iscntrl(tempbuf[i])) tempbuf[i]='.';
            //	  fprintf(logfile,"(%s)(%d)", tempbuf, len);
	    /*
            IFLW_PUT_UINT32_T(fd);
            IFLW_PUT_STRING(tempbuf);
            IFLW_PUT_UINT32_T(len);
	    */
            info_flow_log_syscall_write(scp,EBX,fd,tempbuf);
          }
          //	fprintf(logfile,"\n");	
        }
        break;
      case 10: // recv
        {
          int fd, len, *ptr;
          unsigned int msg;
          
          //	fprintf(logfile,"PID %3d (%16s)[sys_recv    102]%d: ", pid, command, EBX);
          paddr = cpu_get_phys_addr(env, ECX);
          if (paddr!=-1)	{
            bzero(tempbuf, 120);
            cpu_physical_memory_read(paddr, tempbuf, nargs[EBX] ); //-> get the args
            ptr = (int*) tempbuf;
            fd = *ptr++; msg=*ptr++; len= *ptr;
            //	  fprintf(logfile,"socket %d, msg 0x%08x\n", fd, msg);
            info_flow_log_syscall_write(scp,EBX,fd,msg);
          }
        }
        break;
      case 11: // sendto
        {
          int fd,msg, len, *ptr; struct sockaddr_in *sap;
          unsigned char b0, b1, b2, b3, *bptr, b4, b5, b6, b7;
	
          //	fprintf(logfile,"PID %3d (%16s)[sys_sendto  102]%d: ", pid, command, EBX);
          paddr = cpu_get_phys_addr(env, ECX);
          if (paddr!=-1)	{
	    target_ulong sap_addr;
            bzero(tempbuf, 120);
            cpu_physical_memory_read(paddr, tempbuf, nargs[EBX] ); //-> get the args
            ptr = (int*) tempbuf;
            fd =  *ptr++; 
	    msg = *ptr++;
	    len = *ptr++;
	    ptr++;
	    sap_addr = *ptr;
            //	  fprintf(logfile,"socket %d --> ", fd);
            bzero(tempbuf, 120);
            paddr = cpu_get_phys_addr(env, sap_addr);
            cpu_physical_memory_read(paddr, tempbuf, 120); 
            bptr = tempbuf;
            b0=*bptr++; b1=*bptr++; b2=*bptr++; b3=*bptr++;
            b4=*bptr++; b5=*bptr++; b6=*bptr++; b7=*bptr++;
            bptr = tempbuf; bptr+=2;	  
            sap = (struct sockaddr_in*) tempbuf;
            if (sap->sin_family == 1 ) {// PF_LOCAL
              //              IFLW_PUT_STRING(bptr);
              //	    fprintf(logfile,"[dest: family  1; file %s]", bptr);
              info_flow_log_syscall_write(scp,EBX,sap->sin_family,bptr);
            }
            else if (sap->sin_family == 2 ) {// PF_INET
              char addrport[1024];
              //	    fprintf(logfile,"[dest: family  2; %d.%d.%d.%d:%d]", 
              //		    b4, b5, b6, b7, b2*256+b3);
              sprintf(addrport,"%d.%d.%d.%d:%d", b4, b5, b6, b7, b2*256+b3);
              info_flow_log_syscall_write(scp,EBX,sap->sin_family,addrport);
            }
            else {
              //fprintf(logfile,"[dest: family %2d]", sap->sin_family);
              info_flow_log_syscall_write(scp,EBX,sap->sin_family);
            }
            bzero(tempbuf, 120);
            paddr = cpu_get_phys_addr(env, msg);
            cpu_physical_memory_read(paddr, tempbuf, 30); 
            for (i=0; i<30; i++)
              if (iscntrl(tempbuf[i])) tempbuf[i]='.';
            //	  fprintf(logfile," (%s)(%d)", tempbuf, len);
          }
          //	fprintf(logfile,"\n");	
        }
        break;
      case 12: // recvfrom
        {
          int fd, *ptr;
          //	fprintf(logfile,"PID %3d (%16s)[sys_recvfro 102]%d: ",
          //            pid, command, EBX);
          paddr = cpu_get_phys_addr(env, ECX);
          if (paddr!=-1)	{
            bzero(tempbuf, 120);
            cpu_physical_memory_read(paddr, tempbuf, nargs[EBX] ); //-> get the args
            ptr = (int*) tempbuf;
            fd = *ptr++; 
            //	  fprintf(logfile,"socket %d  ", fd);
            info_flow_log_syscall_write(scp,EBX,fd);
          }
          //	fprintf(logfile,"\n");	
        }
        break;
      case 13: // shutdown
        {
          int fd, *ptr;
	
          //	fprintf(logfile,"PID %3d (%16s)[sys_shutdow 102]%d: ",
          //            pid, command, EBX);
          paddr = cpu_get_phys_addr(env, ECX);
          if (paddr!=-1)	{
            bzero(tempbuf, 120);
            cpu_physical_memory_read(paddr, tempbuf, nargs[EBX] ); //-> get the args
            ptr = (int*) tempbuf;
            fd = *ptr++; 
            info_flow_log_syscall_write(scp,EBX,fd,*ptr);
            /*
              if (*ptr == 0 ) 
              fprintf(logfile,"socket %d (SHUT_RD)",   fd);
              else if (*ptr == 1 )
              fprintf(logfile,"socket %d (SHUT_WR)",   fd);
              else if (*ptr == 2 )
              fprintf(logfile,"socket %d (SHUT_RDWR)", fd);
            */
          }
          //	fprintf(logfile,"\n");
        }
        break;
      case 14: // setsockopt
        {
          int fd, level, option, *ptr;
	
          //	fprintf(logfile,"PID %3d (%16s)[sys_setskop 102]%d: ",
          //            pid, command, EBX);
          paddr = cpu_get_phys_addr(env, ECX);
          if (paddr!=-1)	{
            bzero(tempbuf, 120);
            cpu_physical_memory_read(paddr, tempbuf, nargs[EBX] ); //-> get the args
            ptr = (int*) tempbuf;
            fd = *ptr++; level=*ptr++; option=*ptr++;
            //	  fprintf(logfile,"socket %d; level %d; option %d", 
            //            fd, level, option);
            info_flow_log_syscall_write(scp,EBX,fd,level,option);
          }
          //	fprintf(logfile,"\n");
        }
        break;
      case 15: // getsockopt
        {
          int fd, level, option, *ptr;
	
          //	fprintf(logfile,"PID %3d (%16s)[sys_getskop 102]%d: ", pid, command, EBX);
          paddr = cpu_get_phys_addr(env, ECX);
          if (paddr!=-1)	{
            bzero(tempbuf, 120);
            cpu_physical_memory_read(paddr, tempbuf, nargs[EBX] ); //-> get the args
            ptr = (int*) tempbuf;
            fd = *ptr++; level=*ptr++; option=*ptr++;
	    /*
            IFLW_PUT_UINT32_T(fd);
            IFLW_PUT_UINT32_T(level);
            IFLW_PUT_UINT32_T(option);
	    */
            //	  fprintf(logfile,"socket %d; level %d; option %d", fd, level, option);
            info_flow_log_syscall_write(scp,EBX,fd,level,option);
          }
          //	fprintf(logfile,"\n");
        }
        break;
      case 16: // sendmsg
        {
          int fd, *ptr;
	
          //	fprintf(logfile,"PID %3d (%16s)[sys_sendmsg 102]%d: ", pid, command, EBX);
          paddr = cpu_get_phys_addr(env, ECX);
          if (paddr!=-1)	{
            bzero(tempbuf, 120);
            cpu_physical_memory_read(paddr, tempbuf, nargs[EBX] ); //-> get the args
            ptr = (int*) tempbuf;
            fd = *ptr++; 
            //	  fprintf(logfile,"socket %d", fd);
            info_flow_log_syscall_write(scp,EBX,fd);
          }
          //	fprintf(logfile,"\n");
        }
        break;
      case 17: // recvmsg
        {
          int fd, *ptr;
	
          //	fprintf(logfile,"PID %3d (%16s)[sys_recvmsg 102]%d: ", pid, command, EBX);
          paddr = cpu_get_phys_addr(env, ECX);
          if (paddr!=-1)	{
            bzero(tempbuf, 120);
            cpu_physical_memory_read(paddr, tempbuf, nargs[EBX] ); //-> get the args
            ptr = (int*) tempbuf;
            fd = *ptr++; 
            //	  fprintf(logfile,"socket %d", fd);
            info_flow_log_syscall_write(scp,EBX,fd);
          }
          //	fprintf(logfile,"\n");
        }
        break;
      }
      break;
    case 103 : 
      // long sys_syslog(int type, char __user *buf, int len);
      {
        int size=30;
        paddr = cpu_get_phys_addr(env, ECX);
        //      fprintf(logfile,"PID %3d (%16s)[sys_syslog  103]: \n", pid, command);
        bzero(tempbuf, 120);
        if (EDX>0 && EDX <= 30) size = EDX; 
        cpu_physical_memory_read(paddr, tempbuf, size); 
        for (i=0; i<size; i++)
          if (iscntrl(tempbuf[i])) tempbuf[i]='.';
        //      fprintf(logfile,"(%s)(%d)\n", tempbuf, EDX);
        info_flow_log_syscall_write(scp,EBX,tempbuf,EDX);
      }
      break;
    case 104 : 
      // long sys_setitimer(int which,
      //                    struct itimerval __user *value,
      //                    struct itimerval __user *ovalue);
      info_flow_log_syscall_write(scp,EBX);
      break;
    case 105 : 
      // long sys_getitimer(int which, struct itimerval __user *value);
      info_flow_log_syscall_write(scp,EBX);
      break;
    case 106 :
      // long sys_stat(char __user *filename,
      //               struct __old_kernel_stat __user *statbuf);
      if (copy_string(str1,EBX)) {
        info_flow_log_syscall_write(scp,str1);
      }
      break;
    case 107 :
      // long sys_lstat(char __user *filename,
      //                struct __old_kernel_stat __user *statbuf);
      if (copy_string(str1,EBX)) {
        info_flow_log_syscall_write(scp,str1);
      }
      break;
    case 108 : 
      // long sys_fstat(unsigned int fd,
      //                struct __old_kernel_stat __user *statbuf);
      info_flow_log_syscall_write(scp,EBX);
      break;
    case 109 : 
      // sys_olduname is missing from syscalls.h
      info_flow_log_syscall_write(scp);
      break;
    case 110 : 
      // sys_iopl is missing from syscalls.h
      info_flow_log_syscall_write(scp);
      break;
    case 111 : 
      //  long sys_vhangup(void);
      info_flow_log_syscall_write(scp);
      break;
    case 112 :
      // sys_idle is missing from syscalls.h
      info_flow_log_syscall_write(scp);
      break;
    case 113 :
      // sys_vm86old is missing from syscalls.h
      info_flow_log_syscall_write(scp);
      break;
    case 114 :
      // long sys_wait4(pid_t pid, int __user *stat_addr,
      //                int options, struct rusage __user *ru);
      info_flow_log_syscall_write(scp,EBX,EDX);
      break;
    case 115 :
      // long sys_swapoff(const char __user *specialfile);
      if (copy_string(str1,EBX)) {
        info_flow_log_syscall_write(scp,str1,EDX);
      }
      break;
    case 116 :
      // long sys_sysinfo(struct sysinfo __user *info);
      info_flow_log_syscall_write(scp);
      break;
    case 117 :
      // sys_ipc is missing from syscalls.h
      // Xuxian seems to know what to do with it, though. 

      // call first second third ptr fifth
      //->EBX  ECX  EDX   ESI   EDI  EBP  
      // man 2 ipc: 
      // int ipc(unsigned int call, int first, int second, int third,
      //         void *ptr, long fifth);

      // write op and all regs. 
      info_flow_log_syscall_write(scp,EBX,ECX,EDX,ESI,EDI);
      //    fprintf(logfile,"PID %3d (%16s)[sys_ipc     117]: ", pid, command);
      /*
        switch (EBX){
        case 1: // SEMOP
        fprintf(logfile, "SEMOP sem_id %d, sembuf 0x%08x, nsops %d\n", ECX, EDI, EDX);
        break;
        case 2: // SEMGET
        fprintf(logfile, "SEMGET key %d, nsems %d, semflags %d\n", ECX, EDX, ESI);
        break;
        case 3: // SEMCTL
        fprintf(logfile, "SEMCTL sem_id %d, sem_num %d, cmd %d arg 0x%08x\n", ECX, EDX, ESI, EDI);
        break;
        case 11: // MSGSND
        fprintf(logfile, "MSGSND msg_id %d, msg 0x%08x, size %d, flag %d\n", 
        ECX, EDI, EDX, ESI);
        break;
        default:
        fprintf(logfile, "call %d, first %d, second %d, third %d, ptr 0x%08x, fifth %d\n", 
        EBX, ECX, EDX, ESI, EDI, EBP);
        break;
        }
      */
      break;
    case 118 : 
      // long sys_fsync(unsigned int fd);
      info_flow_log_syscall_write(scp,EBX);
      break;
    case 119 :
      // sys_sigreturn is missing in syscalls.h
      // man 2 sigreturn:
      //        int sigreturn(unsigned long __unused);
      info_flow_log_syscall_write(scp);
      break;
    case 120 : 
      // sys_clone is missing in syscalls.h
      // man 2 clone:
      //       int clone(int (*fn)(void *), void *child_stack,
      //                 int flags, void *arg, ...
      info_flow_log_syscall_write(scp);
      break;
    case 121 : 
      //  long sys_setdomainname(char __user *name, int len);
      if (copy_string(str1,EBX)) {
        info_flow_log_syscall_write(scp,str1,ECX);
      break;
    case 122 :
      // sys_uname is missing in syscalls.h
      // man 2 uname:
      //        int uname(struct utsname *buf);
      info_flow_log_syscall_write(scp);
      break;
    case 123 : 
      // sys_modify_ldt is missing in syscalls.h
      // man 2 modify_ldt
      //        int modify_ldt(int func, void *ptr, unsigned long bytecount);
      info_flow_log_syscall_write(scp,EBX);
      break;
    case 124 : 
      // long sys_adjtimex(struct timex __user *txc_p);
      info_flow_log_syscall_write(scp);
      break;
    case 125 :
      // long sys_mprotect(unsigned long start, size_t len,
      //                   unsigned long prot);
      info_flow_log_syscall_write(scp,EBX,ECX,EDX);
      break;
    case 126 :
      // long sys_sigprocmask(int how, old_sigset_t __user *set,
      //                      old_sigset_t __user *oset);
      info_flow_log_syscall_write(scp,EBX,ECX);
      break;
    case 127 : 
      // sys_create_module is missing from syscalls.h
      // Xuxian seems to know what to do with it. 
      //      IFLS_S_SIMP(CREATE_MODULE,EBX);
      if (copy_string(str1,EBX)) {
        info_flow_log_syscall_write(scp,str1);
      break;
    case 128 : 
      // long sys_init_module(void __user *umod, unsigned long len,
      //                      const char __user *uargs);
      if (copy_string(str1,EDX)) {
        info_flow_log_syscall_write(scp,ECX,str1);
      }
      // WTF? How can Xuxian be using EBX as the string ptr? 
      // shouldnt it be EDX? 
      /*
        fprintf(logfile,"PID %3d (%16s)[sys_ini_mod 128]: module ", pid, command);
        paddr = cpu_get_phys_addr(env, EBX);
        bzero(tempbuf, 120);
        cpu_physical_memory_read(paddr, tempbuf, 120); //-> get the file name
        fprintf(logfile,"%s\n", tempbuf);
      */
      break;
    case 129 : 
      // long sys_delete_module(const char __user *name_user,
      //                        unsigned int flags);
      if (copy_string(str1,EBX)) {
        info_flow_log_syscall_write(scp,str1);
      }
      break;
    case 130 : 
      // sys_get_kernel_syms is missing from syscalls.h
      info_flow_log_syscall_write(scp);
      break;
    case 131 : 
      // long sys_quotactl(unsigned int cmd, const char __user *special,
      //                   qid_t id, void __user *addr);
      if (copy_string(str1,ECX)) {
        info_flow_log_syscall_write(scp,EBX,str1,EDX);
      }
      break;
    case 132 :
      // long sys_getpgid(pid_t pid);
      info_flow_log_syscall_write(scp,EBX);
      break;
    case 133 :
      // long sys_fchdir(unsigned int fd);
      info_flow_log_syscall_write(scp,EBX);
      break;
    case 134 : 
      // long sys_bdflush(int func, long data);
      info_flow_log_syscall_write(scp,EBX,ECX);
      break;
    case 135 : 
      // long sys_sysfs(int option,
      //                unsigned long arg1, unsigned long arg2);
      info_flow_log_syscall_write(scp,EBX,ECX,EDX);
      break;
    case 136 : 
      // long sys_personality(u_long personality);
      info_flow_log_syscall_write(scp,EBX);
      break;
    case 137 :
      // sys_afs_syscall is missing from syscalls.h
      info_flow_log_syscall_write(scp);
      break;
    case 138 : 
      // long sys_setfsuid(uid_t uid);
      info_flow_log_syscall_write(scp,EBX);
      break;
    case 139 :
      // long sys_setfsgid(gid_t gid);
      info_flow_log_syscall_write(scp,EBX);
      break;
    case 140 :
      // sys__llseek missing from syscalls.h
      info_flow_log_syscall_write(scp);
      break;
    case 141 : 
      // long sys_getdents(unsigned int fd,
      //                   struct linux_dirent __user *dirent,
      //                   unsigned int count);
      info_flow_log_syscall_write(scp,EBX);
      break;
    case 142 : 
      // sys__newselect missing from syscalls.h
      info_flow_log_syscall_write(scp);
      break;
    case 143 : 
      // long sys_flock(unsigned int fd, unsigned int cmd);
      info_flow_log_syscall_write(scp,EBX,ECX);
      break;
    case 144 :
      // long sys_msync(unsigned long start, size_t len, int flags);
      info_flow_log_syscall_write(scp,EBX,ECX,EDX);
      break;
    case 145 : // sys_readv
      // ssize_t sys_readv(unsigned long fd,
      //                   const struct iovec __user *vec,
      //                   unsigned long vlen);
      info_flow_log_syscall_write(scp,EBX,ECX);
      break;
    case 146 : 
      //  ssize_t sys_writev(unsigned long fd,
      //                     const struct iovec __user *vec,
      //                     unsigned long vlen);
      info_flow_log_syscall_write(scp,EBX,EDX);
      /*
        { int i, len; struct iovec *iovp; 
        fprintf(logfile,"PID %3d (%16s)[sys_writev  146]: fd %d;", pid, command, EBX);
        if (EDX>0) {
        paddr = cpu_get_phys_addr(env, ECX);
        bzero(tempbuf, 120);
        cpu_physical_memory_read(paddr, tempbuf, EDX*sizeof(struct iovec)); //-> get the iov list
        iovp = (struct iov*) tempbuf;
        for(i=0; i<EDX; i++, iovp++) {
        fprintf(logfile," iov[%d].base 0x%08x len %d;", i, iovp->iov_base, iovp->iov_len);
        } // for
        iovp = (struct iov*) tempbuf;
        paddr = cpu_get_phys_addr(env, iovp->iov_base);
        if (iovp->iov_len > 30) len = 30;	
        else len = iovp->iov_len;
        bzero(tempbuf, 120); 
        cpu_physical_memory_read(paddr, tempbuf, len); //-> get the iov list
        for (i=0; i<30; i++)
        if (iscntrl(tempbuf[i])) tempbuf[i]='.';
        fprintf(logfile," iov[0] (%s)\n", tempbuf);				
        } // if
        } // case
      */
      break;
    case 147 : 
      // long sys_getsid(pid_t pid);
      info_flow_log_syscall_write(scp,EBX);
      break;
    case 148 : 
      // long sys_fdatasync(unsigned int fd);
      info_flow_log_syscall_write(scp,EBX);
      break;
    case 149:
      // sys__sysctl missing from syscalls.h
      info_flow_log_syscall_write(scp);
    case 150 :
      // long sys_mlock(unsigned long start, size_t len);
      info_flow_log_syscall_write(scp,EBX,ECX);
      break;
    case 151 : 
      // long sys_munlock(unsigned long start, size_t len);
      info_flow_log_syscall_write(scp,EBX,ECX);
      break;
    case 152 : 
      // long sys_mlockall(int flags);
      info_flow_log_syscall_write(scp,EBX);
      break;
    case 153 : 
      // long sys_munlockall(void);
      info_flow_log_syscall_write(scp);
      break;
    case 154 :
      // long sys_sched_setparam(pid_t pid,
      //                         struct sched_param __user *param);
      info_flow_log_syscall_write(scp,EBX);
      break;
    case 155 : // sys_sched_getparam
      // long sys_sched_getparam(pid_t pid,
      //                         struct sched_param __user *param);
      info_flow_log_syscall_write(scp,EBX);
      break;
    case 156 : 
      // long sys_sched_setscheduler(pid_t pid, int policy,
      //                             struct sched_param __user *param);
      info_flow_log_syscall_write(scp,EBX,ECX);
      break;
    case 157 :
      // long sys_sched_getscheduler(pid_t pid);
      info_flow_log_syscall_write(scp,EBX);
      break;
    case 158 : 
      // long sys_sched_yield(void);
      info_flow_log_syscall_write(scp);
      break;
    case 159 :
      // long sys_sched_get_priority_max(int policy);
      info_flow_log_syscall_write(scp,EBX);
      break;
    case 160 : 
      // long sys_sched_get_priority_min(int policy);
      info_flow_log_syscall_write(scp,EBX);
      break;
    case 161 : 
      // long sys_sched_rr_get_interval(pid_t pid,
      //                                struct timespec __user *interval);
      info_flow_log_syscall_write(scp,EBX);
      break;
    case 162 :
      // long sys_nanosleep(struct timespec __user *rqtp, struct timespec __user *rmtp);
      info_flow_log_syscall_write(scp);
      break;
    case 163 : 
      // unsigned long sys_mremap(unsigned long addr,
      //                          unsigned long old_len, unsigned long new_len,
      //                          unsigned long flags, unsigned long new_addr);
      info_flow_log_syscall_write(scp,EBX,ECX,EDX,ESI);
      break;
    case 164 :
      // long sys_setresuid(uid_t ruid, uid_t euid, uid_t suid);
      info_flow_log_syscall_write(scp,EBX,ECX,EDX);
      break;
    case 165 : 
      // long sys_getresuid16(old_uid_t __user *ruid,
      //                      old_uid_t __user *euid, old_uid_t __user *suid);
      info_flow_log_syscall_write(scp);
      //      fprintf(logfile,"PID %3d (%16s)[sys_getresu 165]\n", pid, command);
      break;
    case 166 : 
      // sys_vm86 is missing from syscalls.h
      info_flow_log_syscall_write(scp);
      break;
    case 167 : 
      // sys_query_module is missing from syscalls.h    
      info_flow_log_syscall_write(scp,EBX);
      break;
    case 168 :
      // long sys_poll(struct pollfd __user *ufds, unsigned int nfds,
      //               long timeout);
      // again, punt.  no good way to put the list of fds into the iferret log... 
      {
        int i; struct pollfd *iovp; 
        //      fprintf(logfile,"PID %3d (%16s)[sys_poll    168]: nfd %d; timeout %d; ", 
        //	      pid, command, ECX, EDX);
        info_flow_log_syscall_write(scp,ECX,EDX);
        /*
        IFLS_II(POLL,ECX,EDX);
        if (ECX>0) {
          paddr = cpu_get_phys_addr(env, EBX);
          bzero(tempbuf, 120);
          cpu_physical_memory_read(paddr, tempbuf, ECX*sizeof(struct pollfd)); 
          iovp = (struct pollfd*) tempbuf;
          for(i=0; i<ECX; i++, iovp++) {
            //	  fprintf(logfile," fds[%d].fd %d (events %d);", i, iovp->fd, iovp->events);
            IFLW_PUT_UINT32_T(iovp->fd);
            IFLW_PUT_UINT32_T(iovp->events);
          } 
        }
        */
       //      fprintf(logfile,"\n");
      } 
      break;
    case 169 :
      // long sys_nfsservctl(int cmd,
      //                     struct nfsctl_arg __user *arg,
      //                     void __user *res);
      info_flow_log_syscall_write(scp,EBX);
      break;
    case 170 : 
      // sys_setresgid(gid_t rgid, gid_t egid, gid_t sgid);
      info_flow_log_syscall_write(scp,EBX,ECX,EDX);
      break;
    case 171 : 
      // long sys_getresgid(gid_t __user *rgid, gid_t __user *egid, gid_t __user *sgid);
      info_flow_log_syscall_write(scp);
      break;
    case 172 : 
      // long sys_prctl(int option, unsigned long arg2, unsigned long arg3,
      //                unsigned long arg4, unsigned long arg5);
      info_flow_log_syscall_write(scp,EBX,ECX,EDX);
      break;
    case 173 : 
      // sys_rt_sigreturn is missing from syscalls.h
      info_flow_log_syscall_write(scp);
      break;
    case 174 :
      // sys_rt_sigaction is missing from syscalls.h
      // Xuxian seems to know how to decode it. 
      // man 2 rt_sigaction:
      //       int sigaction(int signum, const struct sigaction *act,
      //                     struct sigaction *oldact);
      //    fprintf(logfile,"PID %3d (%16s)[sys_rt_siga 174]: sig %d; handler 0x%08x\n", 
      //	    pid, command, EBX, ECX);
      info_flow_log_syscall_write(scp,EBX,ECX);
      break;
    case 175 : 
      // sys_rt_sigprocmask(int how, sigset_t __user *set,
      //                    sigset_t __user *oset, size_t sigsetsize);
      // Why is Xuxian logging the set pointer?
      //    fprintf(logfile,"PID %3d (%16s)[sys_rt_mask 175]: how %d; set 0x%08x\n", 
      //	    pid, command, EBX, ECX);
      info_flow_log_syscall_write(scp,EBX,ECX);
      break;
    case 176 : 
      // long sys_rt_sigpending(sigset_t __user *set, size_t sigsetsize);
      info_flow_log_syscall_write(scp);
      break;
    case 177 : 
      // long sys_rt_sigtimedwait(const sigset_t __user *uthese,
      //                          siginfo_t __user *uinfo,
      //                          const struct timespec __user *uts,
      //            	        size_t sigsetsize);
      info_flow_log_syscall_write(scp);
      break;
    case 178 : 
      // long sys_rt_sigqueueinfo(int pid, int sig, siginfo_t __user *uinfo);
      info_flow_log_syscall_write(scp,EBX,ECX);
      break;
    case 179 :
      // sys_rt_sigsuspend is missing from syscalls.h
      info_flow_log_syscall_write(scp);
      break;
    case 180 : 
      // ssize_t sys_pread64(unsigned int fd, char __user *buf,
      //                     size_t count, loff_t pos);
      info_flow_log_syscall_write(scp,EBX,EDX,ESI);
      break;
    case 181 : 
      // ssize_t sys_pwrite64(unsigned int fd, const char __user *buf,
      //             	    size_t count, loff_t pos);
      info_flow_log_syscall_write(scp,EBX,EDX,ESI);
      break;
    case 182 : 
      // long sys_chown(const char __user *filename,
      //                uid_t user, gid_t group);
      if (copy_string(str1,EBX)) {
        info_flow_log_syscall_write(scp,str1,ECX,EDX);
      }
      break;
    case 183 : 
      // long sys_getcwd(char __user *buf, unsigned long size);
      if (copy_string(str1,EBX)) {
        info_flow_log_syscall_write(scp,str1,ECX);
      }
      break;
    case 184 : 
      // long sys_capget(cap_user_header_t header,
      //                 cap_user_data_t dataptr);
      info_flow_log_syscall_write(scp);
      break;
    case 185 : 
      // long sys_capset(cap_user_header_t header,
      //                 const cap_user_data_t data);
      info_flow_log_syscall_write(scp);
      break;
    case 186 : 
      // sys_sigaltstack is missing from syscalls.h
      // man 2 sigaltstack
      //       int sigaltstack(const stack_t *ss, stack_t *oss);
      info_flow_log_syscall_write(scp);
      break;
    case 187 : 
      // ssize_t sys_sendfile(int out_fd, int in_fd,
      //                      off_t __user *offset, size_t count);
      info_flow_log_syscall_write(scp,EBX,ECX,ESI);
      break;
    case 188 : 
      // sys_getpmsg is missing from syscalls.h
      info_flow_log_syscall_write(scp);
      break;
    case 189 : 
      // sys_putpmsg is missing from syscalls.h
      info_flow_log_syscall_write(scp);
      break;
    case 190 :
      // sys_vfork is missing from syscalls.h
      // man 2 vfork:
      //       pid_t vfork(void);
      info_flow_log_syscall_write(scp);
      break;
    case 191 : 
      // sys_ugetrlimit missing from sysalls.h
      // man 2 ugetrlimit:
      //       int getrlimit(int resource, struct rlimit *rlim);
      info_flow_log_syscall_write(scp,EBX);
      break;
    case 192 :
      // sys_mmap2 is missing from syscalls.h
      // yet Xuxian knows how to decode it..

      //    fprintf(logfile,"PID %3d (%16s)[sys_mmap2   192]: addr 0x%08x, len 0x%08x, prot %d, flags %d, fd %d, offset 0x%08x\n", pid, command, EBX, ECX, EDX, ESI, EDI, EBP);
      info_flow_log_syscall_write(scp,EBX,ECX,EDX,ESI,EDI,EBP);
      break;
    case 193 : 
      // long sys_truncate64(const char __user *path, loff_t length);
      if (copy_string(str1,EBX)) {
        info_flow_log_syscall_write(scp,str1,ECX);
      }
      break;
    case 194 : 
      // long sys_ftruncate64(unsigned int fd, loff_t length);
      info_flow_log_syscall_write(scp,EBX,ECX);
      break;
    case 195 : 
      // long sys_stat64(char __user *filename,
      //                 struct stat64 __user *statbuf);
      if (copy_string(str1,EBX)) {
        info_flow_log_syscall_write(scp,str1);
      }
      break;
    case 196 : 
      // long sys_lstat64(char __user *filename,
      //                  struct stat64 __user *statbuf);
      if (copy_string(str1,EBX)) {
        info_flow_log_syscall_write(scp,str1);
      }
      break;
    case 197 : 
      // long sys_fstat64(unsigned long fd, struct stat64 __user *statbuf);
      info_flow_log_syscall_write(scp,EBX);
      //fprintf(logfile,"PID %3d (%16s)[sys_fstat64 197]\n", pid, command);
      // What's up with this.  EBX should be a file descriptor, not a pointer.
      // I think this is just wrong. 
      /*
        paddr = cpu_get_phys_addr(env, EBX);
        if (paddr!=-1)  {
        fprintf(logfile,"PID %3d (%16s)", pid, command);
        fprintf(logfile,"[sys_fstat64 108]: ");
        bzero(tempbuf, 120);
        cpu_physical_memory_read(paddr, tempbuf, 120); //-> get the dir name
        fprintf(logfile,"%s \n", tempbuf);
        }
      */
      break;
    case 198 :
      // sys_lchown32 is missing from syscalls.h
      // Xuxian seems to know how to decode it.
      // man 2 lchown:
      //      int lchown(const char *path, uid_t owner, gid_t group);
      if (copy_string(str1,EBX)) {
        info_flow_log_syscall_write(scp,str1,ECX,EDX);
      }
      /*
        fprintf(logfile,"PID %3d (%16s)[sys_lchow32 198]\n", pid, command);
        paddr = cpu_get_phys_addr(env, EBX);
        bzero(tempbuf, 120);
        cpu_physical_memory_read(paddr, tempbuf, 120); //-> get the file name
        fprintf(logfile,"%s (uid %d, gid %d)\n", tempbuf, ECX, EDX);
      */
      break;
    case 199 : 
      // sys_getuid32 is missing from syscalls.h
      // man 2 getuid32
      //       uid_t getuid(void);
      info_flow_log_syscall_write(scp);
      break;
    case 200 : 
      // sys_getgid32 is missing from syscalls.h
      // man 2 getgid32
      //       gid_t getgid(void);
      info_flow_log_syscall_write(scp);
      break;
    case 201 : 
      // sys_geteuid32 is missing from syscalls.h
      // man 2 geteuid
      //       uid_t geteuid(void);
      info_flow_log_syscall_write(scp);
      break;
    case 202 : 
      // sys_getegid32 is missing from syscalls.h
      // man 2 get edgid32:
      //       gid_t getegid(void);
      info_flow_log_syscall_write(scp);
      break;
    case 203 : 
      // sys_setreuid32 is missing from syscalls.h
      // Xuxian knows how to decode.
      // man 2 setreuid32
      //       int setreuid(uid_t ruid, uid_t euid);
      info_flow_log_syscall_write(scp,EBX,ECX);
      //    fprintf(logfile,"PID %3d (%16s)[sys_seteu32 203]: ruid %d; euid %d\n",
      //             pid, command, EBX, ECX);
      break;
    case 204 : 
      // sys_setregid32 is missing from syscalls.h
      // Xuxian knows how to decode.
      // man 2 setregid32
      //        int setregid(gid_t rgid, gid_t egid);
      info_flow_log_syscall_write(scp,EBX,ECX);
      //    fprintf(logfile,"PID %3d (%16s)[sys_seteg32 204]: rgid %d; egid %d\n", 
      //            pid, command, EBX, ECX);
      break;
    case 205 : 
      // sys_getgroups32 is missing from syscalls.h
      // man 2 getgroups32:
      //        int getgroups(int size, gid_t list[]);
      info_flow_log_syscall_write(scp,EBX,ECX);
      //    fprintf(logfile,"PID %3d (%16s)[sys_getgr32 205]\n", pid, command);
      break;
    case 206 : 
      // sys_setgroups32 is missing from syscalls.h
      // man 2 setgroups32:
      //        int setgroups(size_t size, const gid_t *list);
      info_flow_log_syscall_write(scp);
      break;
    case 207 : 
      // sys_fchown32 is missing from syscalls.h
      // Xuxian knows how to decode.
      // man 2 fchown32:
      //       int fchown(int fd, uid_t owner, gid_t group);
      info_flow_log_syscall_write(scp,EBX,ECX,EDX);
      //    fprintf(logfile,"PID %3d (%16s)[sys_fchow32 207]: fd %d; uid %d; gid %d\n", pid, command, EBX, ECX, EDX);
      break;
    case 208 : 
      // sys_setresuid32 is missing from syscalls.h
      // Xuxian knows how to decode
      // man 2 setresuid32:
      //       int setresuid(uid_t ruid, uid_t euid, uid_t suid);
      info_flow_log_syscall_write(scp,EBX,ECX,EDX);
      //    fprintf(logfile,"PID %3d (%16s)[sys_setsu32 208]: ruid %d; euid %d; suid %d;\n", pid, command, EBX, ECX, EDX);
      break;
    case 209 : 
      // sys_getresuid32 is missing from syscalls.h
      // man 2 getresuid32:
      //       int getresuid(uid_t *ruid, uid_t *euid, uid_t *suid);
      info_flow_log_syscall_write(scp);
      break;
    case 210 :
      // sys_setresgid32 is missing from syscalls.h
      // Xuxian knows how to decode.
      // man setresgid32:
      //        int setresgid(gid_t rgid, gid_t egid, gid_t sgid);
      info_flow_log_syscall_write(scp,EBX,ECX,EDX);
      //    fprintf(logfile,"PID %3d (%16s)[sys_setes32 208]: rgid %d; egid %d; sgid %d;\n", pid, command, EBX, ECX, EDX);
      break;
    case 211 : 
      // sys_getresgid32 is missing from syscalls.h
      // man 2 getresgid32:
      //       int getresgid(gid_t *rgid, gid_t *egid, gid_t *sgid);
      IFLS(GETRESGID32);
      break;
    case 212 : 
      // sys_chown32 is missing from syscalls.h
      // Xuxian knows how to decode.
      info_flow_log_syscall_write(scp);
      /*
        fprintf(logfile,"PID %3d (%16s)[sys_chown32 212]: ", pid, command);
        paddr = cpu_get_phys_addr(env, EBX);
        bzero(tempbuf, 120);
        cpu_physical_memory_read(paddr, tempbuf, 120); //-> get the file name
        fprintf(logfile,"%s (uid %d, gid %d)\n", tempbuf, ECX, EDX);
      */
      break;
    case 213 :
      // sys_setuid32 is missing from syscalls.h
      // Xuxian knows how to decode.
      // man 2 setuid32:
      //        int setuid(uid_t uid);
      info_flow_log_syscall_write(scp,EBX);
      //    fprintf(logfile,"PID %3d (%16s)[sys_setui32 213]: uid %d\n", pid, command, EBX);
      break;
    case 214 : 
      // sys_setgid32 is missing from syscalls.h
      // Xuxian knows how to decode.
      // man 2 setgid32:
      //        int setgid(gid_t gid);
      info_flow_log_syscall_write(scp,EBX);
      //    fprintf(logfile,"PID %3d (%16s)[sys_setgi32 214]: gid %d\n", pid, command, EBX);
      break;
    case 215 : 
      // sys_setfsuid32 is missing from syscalls.h
      // Xuxian knows how to decode.
      // man 2 setfsuid32:
      //       int setfsuid(uid_t fsuid);
      info_flow_log_syscall_write(scp,EBX);
      //    fprintf(logfile,"PID %3d (%16s)[sys_setfs32 215]: fsuid %d\n", pid, command, EBX);
      break;
    case 216 : 
      // sys_setfsgid32 is missing from syscalls.h
      // Xuxian knows how to decode.
      // man 2 setfsgid32:
      //       int setfsgid(uid_t fsgid);
      info_flow_log_syscall_write(scp,EBX);
      //    fprintf(logfile,"PID %3d (%16s)[sys_setfs32 216]: fsgid %d\n", pid, command, EBX);
      break;
    case 217 :
      // long sys_pivot_root(const char __user *new_root,
      //                     const char __user *put_old);
      if (copy_string(str1,EBX)) {
	if (copy_string(str2,ECX)) {
	  info_flow_log_syscall_write(scp,str1,str2);	  
	}
      }
      // Why not log 2nd string?
      /*
        fprintf(logfile,"PID %3d (%16s)[sys_pivot_r 217]: ", pid, command);
        paddr = cpu_get_phys_addr(env, EBX);
        bzero(tempbuf, 120);
        cpu_physical_memory_read(paddr, tempbuf, 120); //-> get the file name
        fprintf(logfile,"%s\n", tempbuf);
      */
      break;
    case 218 : 
      // long sys_mincore(unsigned long start, size_t len,
      //                  unsigned char __user * vec);
      info_flow_log_syscall_write(scp,EBX,ECX);
      break;
    case 219 : 
      // long sys_madvise(unsigned long start, size_t len, int behavior);
      info_flow_log_syscall_write(scp,EBX,ECX,EDX);
      break;
    case 220 : 
      // long sys_getdents64(unsigned int fd,
      //                     struct linux_dirent64 __user *dirent,
      //                     unsigned int count);
      info_flow_log_syscall_write(scp,EBX,EDX);
      break;
    case 221 : 
      // long sys_fcntl64(unsigned int fd,
      //	                unsigned int cmd, unsigned long arg);
      info_flow_log_syscall_write(scp,EBX,ECX,EDX);
      break;
    case 224 : 
      //  long sys_gettid(void);
      info_flow_log_syscall_write(scp);
      break;
    case 225 : 
      // ssize_t sys_readahead(int fd, loff_t offset, size_t count);
      info_flow_log_syscall_write(scp,EBX,ECX,EDX);
      break;
    case 226 : 
      // long sys_setxattr(const char __user *path, const char __user *name,
      //                   const void __user *value, size_t size, int flags);
      if (copy_string(str1,EBX)) {
	if (copy_string(str2,ECX)) {
	  info_flow_log_syscall_write(scp,str1,str2),ESI,EDI;	  
	}
      }
      break;
    case 227 : 
      // long sys_lsetxattr(const char __user *path, const char __user *name,
      //                    const void __user *value, size_t size, int flags);
      if (copy_string(str1,EBX)) {
	if (copy_string(str2,ECX)) {
	  info_flow_log_syscall_write(scp,str1,str2,ESI,EDI);	  
	}
      }
      break;
    case 228 : 
      //  long sys_fsetxattr(int fd, const char __user *name,
      //                     const void __user *value, size_t size, int flags);
      if (copy_string(str1,ECX)) {
	info_flow_log_syscall_write(scp,EBX,str1,ESI,EDI);
      }
      break;
    case 229 : 
      // ssize_t sys_getxattr(const char __user *path, const char __user *name,
      //              	    void __user *value, size_t size);
      if (copy_string(str1,EBX)) {
	if (copy_string(str2,ECX)) {
	  info_flow_log_syscall_write(scp,str1,str2,ESI,EDI);	  
	}
      }
      break;
    case 230 : 
      // ssize_t sys_lgetxattr(const char __user *path, const char __user *name,
      //                       void __user *value, size_t size);
      if (copy_string(str1,EBX)) {
	if (copy_string(str2,ECX)) {
	  info_flow_log_syscall_write(scp,str1,str2,ESI);	  
	}
      }
      break;
    case 231 : // sys_fgetxattr
      // ssize_t sys_fgetxattr(int fd, const char __user *name,
      //                       void __user *value, size_t size);
      if (copy_string(str1,ECX)) {
	info_flow_log_syscall_write(scp,EBX,str1,ESI);
      }
      break;
    case 232 : 
      // ssize_t sys_listxattr(const char __user *path, char __user *list,
      //	                     size_t size);
      if (copy_string(str1,EBX)) {
	if (copy_string(str2,ECX)) {
	  info_flow_log_syscall_write(scp,str1,str2,EDX);
	}
      }
      break;
    case 233 :
      // ssize_t sys_llistxattr(const char __user *path, char __user *list,
      //                        size_t size);
      if (copy_string(str1,EBX)) {
	if (copy_string(str2,ECX)) {
	  info_flow_log_syscall_write(scp,str1,str2,EDX);
	}
      }
      break;
    case 234 : 
      // ssize_t sys_flistxattr(int fd, char __user *list, size_t size);
      info_flow_log_syscall_write(scp,EBX);
      break;
    case 235 : 
      // long sys_removexattr(const char __user *path,
      //                      const char __user *name);
      if (copy_string(str1,EBX)) {
	if (copy_string(str2,ECX)) {
	  info_flow_log_syscall_write(scp,str1,str2);
	}
      }
      break;
    case 236 :     
      // long sys_lremovexattr(const char __user *path,
      //                       const char __user *name);
      if (copy_string(str1,EBX)) {
	if (copy_string(str2,ECX)) {
	  info_flow_log_syscall_write(scp,str1,str2);
	}
      }
      break;
    case 237 : 
      // long sys_fremovexattr(int fd, const char __user *name);
      if (copy_string(str1,ECX)) {
	info_flow_log_syscall_write(scp,EBX,str1);
      }
      break;
    case 238 : 
      // long sys_tkill(int pid, int sig);
      info_flow_log_syscall_write(scp,EBX,ECX);
      break;
    case 239 : 
      //  ssize_t sys_sendfile64(int out_fd, int in_fd,
      //                         loff_t __user *offset, size_t count);
      info_flow_log_syscall_write(scp,EBX,ECX);
      break;
    case 240 : 
      // long sys_futex(u32 __user *uaddr, int op, u32 val,
      //                struct timespec __user *utime, u32 __user *uaddr2,
      //                u32 val3);
      info_flow_log_syscall_write(scp,ECX,EDX);
      break;
    case 241 : 
      // long sys_sched_setaffinity(pid_t pid, unsigned int len,
      //                            unsigned long __user *user_mask_ptr);
      info_flow_log_syscall_write(scp,EBX,ECX);
      break;
    case 242 : 
      // long sys_sched_getaffinity(pid_t pid, unsigned int len,
      //                            unsigned long __user *user_mask_ptr);
      info_flow_log_syscall_write(scp,EBX,ECX);
      break;
    case 243 :
      // sys_set_thread_area is missing from syscalls.h    
      // man 2 set_thread_area
      //       int set_thread_area(struct user_desc *u_info);
      info_flow_log_syscall_write(scp);
      break;
    case 244 :
      // sys_get_thread_area is missing from syscall.h
      // man 2 get_thread_area
      //       int get_thread_area(struct user_desc *u_info);
      info_flow_log_syscall_write(scp);
      break;
    case 245 : 
      // int sys_io_setup(unsigned nr_reqs, aio_context_t __user *ctx);
      info_flow_log_syscall_write(scp);
      break;
    case 246 : 
      // long sys_io_destroy(aio_context_t ctx);
      info_flow_log_syscall_write(scp);
      break;
    case 247 : 
      //  long sys_io_getevents(aio_context_t ctx_id,
      //                        long min_nr,
      //                        long nr,
      //                        struct io_event __user *events,
      //                        struct timespec __user *timeout);
      info_flow_log_syscall_write(scp);
      break;
    case 248 :
      // long sys_io_submit(aio_context_t, long,
      //                    struct iocb __user * __user *);
      info_flow_log_syscall_write(scp);
      break;
    case 249 : 
      // long sys_io_cancel(aio_context_t ctx_id, struct iocb __user *iocb,
      //                    struct io_event __user *result);
      info_flow_log_syscall_write(scp);
      break;
    case 250 : 
      // sys_fadvise64(int fd, loff_t offset, size_t len, int advice); 
      info_flow_log_syscall_write(scp,EBX,ECX,EDX,ESI);
      break;
    case 252 : 
      // void sys_exit_group(int error_code);
      info_flow_log_syscall_write(scp,EBX);
      break;
    case 253 : 
      //  long sys_lookup_dcookie(u64 cookie64, char __user *buf, size_t len);
      info_flow_log_syscall_write(scp);
      break;
    case 254 :
      // long sys_epoll_create(int size);
      info_flow_log_syscall_write(scp,EBX);
      break;
    case 255 :
      //  long sys_epoll_ctl(int epfd, int op, int fd,
      //                     struct epoll_event __user *event);
      info_flow_log_syscall_write(scp,EBX,ECX,EDX);
      break;
    case 256 : 
      // ong sys_epoll_wait(int epfd, struct epoll_event __user *events,
      //                    int maxevents, int timeout);
      info_flow_log_syscall_write(scp,EBX,EDX,ESI);
      break;
    case 257 : 
      // long sys_remap_file_pages(unsigned long start, unsigned long size,
      //                           unsigned long prot, unsigned long pgoff,
      //                           unsigned long flags);
      info_flow_log_syscall_write(scp,EBX,ECX,EDX,ESI,EDI);
      break;
    case 258 :
      // long sys_set_tid_address(int __user *tidptr);
      IFLS(SET_TID_ADDRESS);
      break;
    case 259 : 
      // long sys_timer_create(clockid_t which_clock,
      //                       struct sigevent __user *timer_event_spec,
      //                       timer_t __user * created_timer_id);
      IFLS_I(TIMER_CREATE,EBX);
      break;
    case 260: 
      // long sys_timer_settime(timer_t timer_id, int flags,
      //                        const struct itimerspec __user *new_setting,
      //                        struct itimerspec __user *old_setting);
      IFLS_II(TIMER_SETTIME,EBX,ECX);
      break;
    case 261: 
      // timer_gettime	(__NR_timer_create+2)
      // long sys_timer_gettime(timer_t timer_id,
      //                        struct itimerspec __user *setting);
      IFLS_I(TIMER_GETTIME,EBX);
      break;
    case 262: 
      // sys_timer_getoverrun missing from syscalls.h
      IFLS(TIMER_GETOVERRUN);
      break;   
    case 263: 
      //  long sys_timer_delete(timer_t timer_id);
      IFLS_I(TIMER_DELETE,EBX);
      break;
    case 264: 
      // long sys_clock_settime(clockid_t which_clock,
      //                        const struct timespec __user *tp);
      IFLS_I(CLOCK_SETTIME,EBX);
      break;
    case 265: 
      // long sys_clock_gettime(clockid_t which_clock,
      //                        struct timespec __user *tp);
      IFLS_I(CLOCK_GETTIME,EBX);
      break;
    case 266:
      // clock_getres	(__NR_timer_create+7)
      // long sys_clock_getres(clockid_t which_clock,
      //                       struct timespec __user *tp);
      IFLS_I(CLOCK_GETRES,EBX);
      break;
    case 267: 
      // clock_nanosleep	(__NR_timer_create+8)
      // long sys_clock_nanosleep(clockid_t which_clock, int flags,
      //                          const struct timespec __user *rqtp,
      //                          struct timespec __user *rmtp);
      IFLS_II(CLOCK_NANOSLEEP,EBX,ECX);
      break;
    case 268: 
      // long sys_statfs64(const char __user *path, size_t sz,
      //                   struct statfs64 __user *buf);
      IFLS_SI_SIMP(STATFS64,EBX,ECX);
      break;
    case 269: 
      // long sys_fstatfs64(unsigned int fd, size_t sz,
      //                    struct statfs64 __user *buf);
      IFLS_II(FSTATFS64,EBX,ECX);
      break;
    case 270: 
      // long sys_tgkill(int tgid, int pid, int sig);
      IFLS_III(TGKILL,EBX,ECX,EDX);
      break;
    case 271: 
      //long sys_utimes(char __user *filename,
      //                struct timeval __user *utimes);
      IFLS_S_SIMP(UTIMES,EBX);
      break;
    case 272:
      // long sys_fadvise64_64(int fd, loff_t offset, loff_t len, int advice);
      IFLS_IIII(FADVISE64_64,EBX,ECX,EDX,ESI);
      break;
    case 273: 
      // sys_vserver missing from syscalls.h
      IFLS(VSERVER);
      break;
    case 274: 
      // long sys_mbind(unsigned long start, unsigned long len,
      //                unsigned long mode,
      //                unsigned long __user *nmask,
      //                unsigned long maxnode,
      //                unsigned flags);
      IFLS_IIIII(MBIND,EBX,ECX,EDX,EDI,EBP);
      break;
    case 275: 
      // long sys_get_mempolicy(int __user *policy,
      //                        unsigned long __user *nmask,
      //                        unsigned long maxnode,
      //                        unsigned long addr, unsigned long flags);
      IFLS_II(GET_MEMPOLICY,ESI,EDI);
      break;
    case 276: 
      //  long sys_set_mempolicy(int mode, unsigned long __user *nmask,
      //                         unsigned long maxnode);
      IFLS_I(SET_MEMPOLICY,EBX);
      break;
    case 277: 
      // long sys_mq_open(const char __user *name, int oflag, mode_t mode, 
      //                  struct mq_attr __user *attr);
      IFLS_SII_SIMP(MQ_OPEN,EBX,ECX,EDX);
      break;
    case 278: 
      // long sys_mq_unlink(const char __user *name);
      IFLS_S_SIMP(MQ_UNLINK,EBX);
      break;
    case 279: 
      // long sys_mq_timedsend(mqd_t mqdes, const char __user *msg_ptr, 
      //                       size_t msg_len, unsigned int msg_prio, 
      //                       const struct timespec __user *abs_timeout);
      IFLS(MQ_TIMEDSEND);
      break;
    case 280: 
      // mq_timedreceive	(__NR_mq_open+3)
      // ssize_t sys_mq_timedreceive(mqd_t mqdes, char __user *msg_ptr, 
      //                             size_t msg_len, unsigned int __user *msg_prio,
      //                             const struct timespec __user *abs_timeout);
      IFLS(MQ_TIMEDRECEIVE);
      break;
    case 281: 
      // long sys_mq_notify(mqd_t mqdes, const struct sigevent __user *notification);
      IFLS(MQ_NOTIFY);
      break;
    case 282:
      // long sys_mq_getsetattr(mqd_t mqdes, const struct mq_attr __user *mqstat, 
      //                        struct mq_attr __user *omqstat);
      IFLS(MQ_GETSETATTR);
      break;
    case 283: 
      // long sys_kexec_load(unsigned long entry, unsigned long nr_segments,
      //                     struct kexec_segment __user *segments,
      //                     unsigned long flags);
      IFLS_III(KEXEC_LOAD,EBX,ECX,ESI);
      break;
    case 284: 
      // long sys_waitid(int which, pid_t pid,
      //                 struct siginfo __user *infop,
      //                 int options, struct rusage __user *ru);
      IFLS_III(WAITID,EBX,ECX,ESI);
      break;
    case 285: 
      // sys_setaltroot missing from syscalls.h
      IFLS(SETALTROOT);
      break;
    case 286: 
      // long sys_add_key(const char __user *_type,
      //                  const char __user *_description,
      //                  const void __user *_payload,
      //                  size_t plen,
      //                  key_serial_t destringid);
      IFLS_SSI_SIMP(ADD_KEY,EBX,ECX,EDI);
      break;
    case 287: 
      // long sys_request_key(const char __user *_type,
      //                      const char __user *_description,
      //                      const char __user *_callout_info,
      //                      key_serial_t destringid);
      IFLS_SSSI_SIMP(REQUEST_KEY,EBX,ECX,EDX,ESI);
      break;
    case 288:
      // long sys_keyctl(int cmd, unsigned long arg2, unsigned long arg3,
      //                 unsigned long arg4, unsigned long arg5);
      IFLS_IIIII(KEYCTL,EBX,ECX,EDX,ESI,EDI);
      break;
    case 289: 
      // long sys_ioprio_set(int which, int who, int ioprio);
      IFLS_III(IOPRIO_SET,EBX,ECX,EDX);
      break;
    case 290: 
      // long sys_ioprio_get(int which, int who);
      IFLS_II(IOPRIO_GET,EBX,ECX);
      break;
    case 291: 
      // long sys_inotify_init(void);
      IFLS(INOTIFY_INIT);
      break;
    case 292:
      // long sys_inotify_add_watch(int fd, const char __user *path,
      //                            u32 mask);
      IFLS_ISI_SIMP(INOTIFY_ADD_WATCH,EBX,ECX,EDX);
      break;   
    case 293 :
      // long sys_inotify_rm_watch(int fd, u32 wd);
      IFLS_II(INOTIFY_RM_WATCH,EBX,ECX);
      break;
    case 294:
      // long sys_migrate_pages(pid_t pid, unsigned long maxnode,
      //                        const unsigned long __user *from,
      //                        const unsigned long __user *to); 
      IFLS_II(MIGRATE_PAGES,EBX,ECX);
      break;
    case 295: 
      // long sys_openat(int dfd, const char __user *filename, int flags,
      //                 int mode);
      IFLS_ISII_SIMP(OPENAT,EBX,ECX,EDX,ESI);
      break;
    case 296: 
      // long sys_mkdirat(int dfd, const char __user * pathname, int mode);
      IFLS_ISI_SIMP(MKDIRAT,EBX,ECX,EDX);
      break;
    case 297: 
      // long sys_mknodat(int dfd, const char __user * filename, int mode,
      //                  unsigned dev);
      IFLS_ISII_SIMP(MKNODAT,EBX,ECX,EDX,ESI);
      break;
    case 298: 
      // long sys_fchownat(int dfd, const char __user *filename, uid_t user,
      //                   gid_t group, int flag);
      IFLS_ISIII_SIMP(FCHOWNAT,EBX,ECX,EDX,ESI,EDI);
      break;
    case 299: 
      // long sys_futimesat(int dfd, char __user *filename,
      //                    struct timeval __user *utimes);
      IFLS_IS_SIMP(FUTIMESAT,EBX,ECX);
      break;
    case 300: 
      // long sys_fstatat64(int dfd, char __user *filename,
      //                    struct stat64 __user *statbuf, int flag);
      IFLS_ISI_SIMP(FSTATAT64,EBX,ECX,ESI);
      break;
    case 301: 
      // long sys_unlinkat(int dfd, const char __user * pathname, int flag);
      IFLS_ISI_SIMP(UNLINKAT,EBX,ECX,EDX);
      break;
    case 302: 
      // long sys_renameat(int olddfd, const char __user * oldname,
      //                   int newdfd, const char __user * newname);
      IFLS_ISIS_SIMP(RENAMEAT,EBX,ECX,EDX,ESI);
      break;
    case 303: 
      // long sys_linkat(int olddfd, const char __user *oldname,
      //                 int newdfd, const char __user *newname, int flags);
      IFLS_ISISI_SIMP(LINKAT,EBX,ECX,EDX,ESI,EDI);
      break;
    case 304: 
      //  long sys_symlinkat(const char __user * oldname,
      //                     int newdfd, const char __user * newname)
      IFLS_SIS_SIMP(SYMLINKAT,EBX,ECX,EDX);
      break;
    case 305: 
      // long sys_readlinkat(int dfd, const char __user *path, char __user *buf,
      // int bufsiz);
      IFLS_ISS_SIMP(READLINKAT,EBX,ECX,EDX);
      break;
    case 306: 
      // long sys_fchmodat(int dfd, const char __user * filename,
      //                   mode_t mode);
      IFLS_ISI_SIMP(FCHMODAT,EBX,ECX,EDX);
      break;
    case 307: 
      // long sys_faccessat(int dfd, const char __user *filename, int mode);
      IFLS_ISI_SIMP(FACCESSAT,EBX,ECX,EDX);
      break;
    case 308: 
      // sys_pselect6 missing from syscalls.h
      IFLS(PSELECT6);
      break;
    case 309: 
      // sys_ppoll missing from syscalls.h
      IFLS(PPOLL);
      break;
    case 310: 
      // long sys_unshare(unsigned long unshare_flags);
      IFLS_I(UNSHARE,EBX);
      break;
    case 311: 
      // long sys_set_robust_list(struct robust_list_head __user *head,
      //                          size_t len);
      IFLS(SET_ROBUST_LIST);
      break;
    case 312:
      // long sys_get_robust_list(int pid,
      //                          struct robust_list_head __user * __user *head_ptr,
      //                          size_t __user *len_ptr);
      IFLS_I(GET_ROBUST_LIST,EBX);
      break;
    case 313:
      // long sys_splice(int fd_in, loff_t __user *off_in,
      //                 int fd_out, loff_t __user *off_out,
      //                 size_t len, unsigned int flags);
      IFLS_IIII(SPLICE,EBX,EDX,EDI,EBP);
      break;
    case 314: 
      // long sys_sync_file_range(int fd, loff_t offset, loff_t nbytes,
      //                          unsigned int flags);
      IFLS_IIII(SYNC_FILE_RANGE,EBX,ECX,EDX,ESI);
      break;
    case 315: 
      // long sys_tee(int fdin, int fdout, size_t len, unsigned int flags);
      IFLS_IIII(TEE,EBX,ECX,EDX,ESI);
      break;
    case 316: 
      // long sys_vmsplice(int fd, const struct iovec __user *iov,
      //                   unsigned long nr_segs, unsigned int flags);
      IFLS_II(VMSPLICE,EBX,ESI);
      break;
    case 317: 
      // long sys_move_pages(pid_t pid, unsigned long nr_pages,
      //                     const void __user * __user *pages,
      //                     const int __user *nodes,
      //                     int __user *status,
      //                     int flags);
      IFLS_III(MOVE_PAGES,EBX,ECX,EBP);
      break;
    case 318:
      // long sys_getcpu(unsigned __user *cpu, unsigned __user *node, 
      //                 struct getcpu_cache __user *cache);
      IFLS(GETCPU);
      break;
    case 319: 
      // long sys_epoll_wait(int epfd, struct epoll_event __user *events,
      //                     int maxevents, int timeout);
      IFLS_III(EPOLL_WAIT,EBX,EDX,ESI);
      break;
    case 320: 
      // long sys_utimensat(int dfd, char __user *filename,
      //                    struct timespec __user *utimes, int flags);
      IFLS_ISI_SIMP(UTIMENSAT,EBX,ECX,ESI);
      break;
    case 321: 
      // long sys_signalfd(int ufd, sigset_t __user *user_mask, size_t sizemask);
      IFLS_I(SIGNALFD,EBX);
      break;
    case 322: 
      //  long sys_timerfd_create(int clockid, int flags);
      IFLS_II(TIMER_CREATE,EBX,ECX);
      break;
    case 323: 
      // long sys_eventfd(unsigned int count);
      IFLS_I(EVENTFD,EBX);
      break;
    case 324:
      // long sys_fallocate(int fd, int mode, loff_t offset, loff_t len);
      IFLS_IIII(FALLOCATE,EBX,ECX,EDX,ESI);
      break;
    case 325:
      // long sys_timerfd_settime(int ufd, int flags,
      //                          const struct itimerspec __user *utmr,
      //                          struct itimerspec __user *otmr);
      IFLS_II(TIMERFD_SETTIME,EBX,ECX);
      break;
    case 326: 
      //  long sys_timerfd_gettime(int ufd, struct itimerspec __user *otmr); 
      IFLS_I(TIMERFD_GETTIME,EBX);
      break;
    case 327: 
      // long sys_signalfd4(int ufd, sigset_t __user *user_mask, size_t sizemask, int flags);
      IFLS_II(SIGNALFD4,EBX,ESI);
      break;
    case 328: 
      //  long sys_eventfd2(unsigned int count, int flags);
      IFLS_II(EVENTFD2,EBX,ECX);
      break;
    case 329: 
      //  long sys_epoll_create1(int flags);
      IFLS_I(EPOLL_CREATE1,EBX);
      break;
    case 330: 
      // long sys_dup3(unsigned int oldfd, unsigned int newfd, int flags); 
      IFLS_III(DUP3,EBX,ECX,EDX);
      break;
    case 331: 
      // sys_pipe2 missing from syscalls.h
      IFLS(PIPE2);
      break;
    case 332: 
      // long sys_inotify_init1(int flags);
      IFLS_I(INOTIFY_INIT1,EBX);
      break;
      
    default:
      IFLS_IIIIIII(UNKNOWN,EAX,EBX,ECX,EDX,ESI,EDI,EBP);
      break;
    }
  }
}


// log system call return value
// is_ret is 1 (TRUE) iff this is a return via iret.
// is_ret is 0 (FALSE) iff this is a return via sys_exit.
// callsite_esp is stack pointer for callsite. 
// what's this another_eip?  
void iferret_log_syscall_ret(uint8_t is_iret, uint32_t callsite_esp, uint32_t another_eip) {
  uint8_t is_sysenter;
  target_ulong current_task;
  target_phys_addr_t paddr, eip_for_callsite;
  struct syscall_entry syscall_element;
  int pid,uid;
  char command[COMM_SIZE];

  // get addr of pointer to current task
  current_task = get_task_struct_ptr(callsite_esp);
  //  printf ("current_task = 0x%x\n", current_task);
  
  pid = 0;
  // grab process id, uid and command string. 
  copy_task_struct_slot(current_task, PID_OFFSET, PID_SIZE, (char *) &pid);
  copy_task_struct_slot(current_task, UID_OFFSET, UID_SIZE, (char *) &uid);
  copy_task_struct_slot(current_task, COMM_OFFSET, COMM_SIZE, command);
  //  printf ("pid = %d  uid = %d\n", pid, uid);

  // get callsite eip. 
  if (is_iret) {
    // Return was via iret instruction.
    // Call site eip is 3 4-byte params up the stack.
    paddr = cpu_get_phys_addr(env, ESP+4*3);
    is_sysenter = 0;
  }
  else {
    // Return was via sysexit.
    // Why is callsite eip at ECX+4*3? 
    paddr = cpu_get_phys_addr(env, ECX+4*3);
    is_sysenter = 1;
  }
  if (paddr!=-1) {
    // callsite eip is stack_val
    cpu_physical_memory_read(paddr, (char *) &eip_for_callsite, 4);    
    // find corresponding call to do_interrupt or sys_enter that preceded this return
    if (is_iret) {
      syscall_element = find_element_with_eip(pid, eip_for_callsite, another_eip);
    }
    else {
      syscall_element = find_element_with_eip(pid, eip_for_callsite, -1);
    }
    if (syscall_element.eip != -1){
      // found it!  Log it. 
      // NB: PID & EIP should be enough to match up.  EAX is the retval. 
      IFLS_IIII(IRET_OR_SYSEXIT, pid, eip_for_callsite, syscall_element.syscall_num, EAX);
      // and remove that call site item from the stack
      del_element(pid,syscall_element.offset-1);
    }	      
  }
}



// log system call (via sysexit) return value 
void iferret_log_syscall_ret_sysexit(uint32_t callsite_esp) {
  iferret_log_syscall_ret(0, callsite_esp, -1);
}


// log system call (vial iret) return value 
void iferret_log_syscall_ret_iret(uint32_t callsite_esp, uint32_t another_eip) {
  iferret_log_syscall_ret(1, callsite_esp, another_eip);
}
