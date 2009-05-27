
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
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/uio.h>
#include <poll.h>
#include <ctype.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

#include "exec.h"

#include "linux_task_struct_offsets.h"
#include "../iferret_log.h"
#include "iferret_syscall.h"
#include "iferret_syscall_stack.h"

extern struct CPUX86State *env;
extern pid_t pid;
extern uid_t uid;

// defined and set in vl.c
extern uint8_t iferret_target_os;

target_phys_addr_t cpu_get_phys_addr(CPUState *env, target_ulong addr);
int cpu_virtual_memory_read(CPUState *env, uint32_t addr, char *out, uint32_t length);

pid_t current_pid, last_pid;
uid_t current_uid, last_uid;
char current_command[256];
pid_t parent_pid;
uid_t parent_uid;
char parent_command[256];
struct timespec current_start_time;
struct timespec current_real_start_time;
struct timespec parent_start_time;
struct timespec parent_real_start_time;

uint8_t no_pid_flag = 1;
uint8_t no_uid_flag = 1;

uint8_t iferret_in_kernel(void);

void write_current_uid_to_iferret_log(void);
void write_current_pid_to_iferret_log(void);


/* Argument list sizes for sys_socketcall */
/*
#define AL(x) ((x) * sizeof(unsigned long))
static unsigned char nargs[18]={AL(0),AL(3),AL(3),AL(3),AL(2),AL(3),
                                AL(3),AL(3),AL(4),AL(4),AL(4),AL(6),
                                AL(6),AL(2),AL(5),AL(5),AL(3),AL(3)};
*/

/* Windows Introspection for XPSP2 */

// BDG 5/15/2009
// Get a system call argument in Windows.
// The system call stub for a generic Windows syscall looks like:
// NtSystemCall:
//     mov eax, Ordinal
//     mov edx, KiFastSystemCall
//     call [edx]
// KiFastSystemCall:
//     mov edx, esp
//     sysenter
// When systenter occurs, we can use EDX to retrieve the
// values from the original stack (ESP gets modified by
// sysenter). There are two additional things on the stack
// at this point--the return address for KiFastSystemCall
// and the return address for the userland entry point to
// the system call (e.g. the interface exposed by ntdll.dll).
static inline target_ulong iferret_get_arg_win(int i) {
    return EDX+(sizeof(target_ulong)*(i+2));
}

void iferret_get_uchar(uint32_t uchar_addr, uint8_t *out) {
    if(!cpu_virtual_memory_read(env, uchar_addr, (char *) out, 1)) {
        *out = 0;
    }
    return;
}
void iferret_get_large_integer(uint32_t largeint_addr, uint64_t *out) {
    if(!cpu_virtual_memory_read(env, largeint_addr, (char *) out, 8)) {
        *out = 0;
    }
    return;
}

void iferret_get_unicode_string(uint32_t ustr_addr, char **out) {
    int i;
    uint16_t slen,smaxlen;
    uint32_t sbuf_addr;
    char *outbuf = NULL;
    char *sbuf = NULL;

    if(!read_member(env, ustr_addr, UNICODE_STRING, Length, &slen)) {
        goto get_unicode_string_error;
    }
    if(!read_member(env, ustr_addr, UNICODE_STRING, MaximumLength, &smaxlen)) {
        goto get_unicode_string_error;
    }
    if(!read_member(env, ustr_addr, UNICODE_STRING, Buffer, &sbuf_addr)) {
        goto get_unicode_string_error;
    }

    // Length is the min of the two
    slen = (slen < smaxlen) ? slen : smaxlen;

    sbuf = (char *)malloc(slen);
    outbuf = (char *)malloc((slen / 2) + 1);
    if(!cpu_virtual_memory_read(env, sbuf_addr, (char *) sbuf, slen)) {
        goto get_unicode_string_error;
    }

    // "Unicode" handling -- skip every other byte
    for (i = 0; i < (slen / 2); i++) {
        outbuf[i] = sbuf[i*2];
    }
    outbuf[i] = '\0';
    free(sbuf);
    *out = outbuf;
    return;

get_unicode_string_error:
    if (sbuf) free(sbuf);
    if (outbuf) free(outbuf);
    outbuf = (char *) malloc(1);
    outbuf[0] = '\0';
    *out = outbuf;
    return;
}

void iferret_get_object_attributes(uint32_t oattr_addr, char **out) {
    uint32_t ustr;
    
    //if(!cpu_virtual_memory_read(env, oattr_addr+8, (char *) &ustr, 4)) {
    if (!read_member(env, oattr_addr, OBJECT_ATTRIBUTES, ObjectName, &ustr)) {
        goto get_object_attributes_error;
    }

    if(!ustr) {
        goto get_object_attributes_error;
    }

    iferret_get_unicode_string(ustr, out);
    return;

get_object_attributes_error:
    *out = (char *) malloc(1);
    *out[0] = '\0';
    return;
}

static inline long iferret_get_current_process_win() {
    uint32_t eproc, ethread;
    read_member(env, KPCR_ADDRESS+offsetof(KPCR,PrcbData), KPRCB, CurrentThread, &ethread);
    if(!ethread) {
        printf("XPSYS getpid: CurrentThread == NULL\n");
        return 0;
    }
    read_member(env, ethread, ETHREAD, ThreadsProcess, &eproc);
    return eproc;
}

static inline int iferret_get_current_command_win(char *cmdname) {
    target_ulong eproc;
    if(!(eproc = iferret_get_current_process_win())) {
        //printf("iferret_get_current_command_win: Couldn't get current process\n");
        return -1;
    }
    read_member(env, eproc, EPROCESS, ImageFileName, cmdname);
    if (cmdname[15] != '\0')
        cmdname[16] = '\0';
    return 1;
}

static inline target_ulong iferret_get_current_pid_win() {
    target_ulong pid;
    target_ulong eproc;

    if(!(eproc = iferret_get_current_process_win())) {
        //printf("iferret_get_current_pid_win: Couldn't get current process\n");
        return 0;
    }

    read_member(env, eproc, EPROCESS, UniqueProcessId, &pid);
    return pid;
}

static inline uid_t iferret_get_current_uid_win() {
    uid_t uid;
    uint32_t eproc,token,ug_count,sid_and_attr,sid;
    uint32_t subauth_array_base, subauth_array_off;
    uint8_t auth_count;

    if(!(eproc = iferret_get_current_process_win())) {
        //printf("iferret_get_current_uid_win: Couldn't get current process\n");
        return -1;
    }

    read_member(env, eproc, EPROCESS, Token, &token);
    token &= ~0x7; // Lowest 3 bits are used to encode refcount
    read_member(env, token, TOKEN, UserAndGroupCount, &ug_count);
    read_member(env, token, TOKEN, UserAndGroups, &sid_and_attr);
    read_member(env, sid_and_attr, SID_AND_ATTRIBUTES, Sid, &sid);
    read_member(env, sid, SID, SubAuthorityCount, &auth_count);

    // Read the last SubAuthority value (kind of ugly)
    subauth_array_base = sid+offsetof(SID,SubAuthority);
    subauth_array_off  = (auth_count-1)*membsize(SID,SubAuthority);
    cpu_virtual_memory_read(env, subauth_array_base+subauth_array_off, (char *) &uid, 4);
    return uid;
}

/* End Windows introspection code */

static inline int current_pid_valid() {
  switch(iferret_target_os) {
    case OS_LINUX:
      return (current_pid>=0 && current_pid<=32768);
    case OS_WINXPSP2:
      return (current_pid & 3) == 0;
  }
}


/*
static inline uint32_t phys_addr(uint32_t addr) {
  addr = cpu_get_phys_addr(env,addr); 
  if (addr == -1)
    return 0;
  else
    return (addr); 
}
*/


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
    iferret_cpu_physical_memory_read(paddr, dest, slot_size);
  }
}


// copy string of length at most len-1
// from physical adddress physaddr into buffer tempbuf
// null terminate
// NB: assumes tempbuf allocated!
static inline void copy_string_phys(char *tempbuf, target_phys_addr_t physaddr, uint32_t len) {
  assert (len > 0);
  bzero(tempbuf, len);
  iferret_cpu_physical_memory_read((unsigned long) physaddr, tempbuf, len-1);
}


// str assumed to be at least 120 bytes allocated.
// vaddr is virt addr of a string. 
// return 0 on failure, 1 on success.
int copy_string(char *str, uint32_t vaddr) { 
  bzero(str, 120);
  if(!cpu_virtual_memory_read(env, vaddr, str, 120)) {
    return 0;
  }
  else {
    return 1;
  }
}
  

// returns the virtual (?) address of the current task structure
static inline target_ulong get_task_struct_ptr (target_ulong current_esp) {
  target_phys_addr_t paddr;
  target_ulong current_task;
  // paddr is physical address of the current task pointer.
  paddr = cpu_get_phys_addr(env, (target_ulong) (current_esp & CURRENT_TASK_MASK)); 
  if (paddr!=-1) {
    // read 4 bytes our of physical memory starting from phys addr paddr
    // and deposit them in current_task, which contains, therefore
    // a virtual address.
     iferret_cpu_physical_memory_read(paddr, (char *) &current_task, 4);
     return (current_task);
  }
  //  assert (paddr != -1);
  return (0);
}

void iferret_get_current_pid_uid_win() {
  // save last pid.
  last_pid = current_pid;

  // compute current pid &c
  current_pid = iferret_get_current_pid_win();
  current_uid = iferret_get_current_uid_win();

  if (no_pid_flag == 1) {
    //    printf ("1 last_pid=%d current_pid is %d\n", last_pid, current_pid);
    //printf("new PID: %u\r\n",current_pid);
    write_current_pid_to_iferret_log();
    write_current_uid_to_iferret_log();
    no_pid_flag = 0;
  }
  else {
    if (last_pid != current_pid) {
      //      printf ("2 last_pid=%d current_pid is %d\n", last_pid, current_pid);
      //printf("new PID: %d\r\n",current_pid);
      write_current_pid_to_iferret_log();
      write_current_uid_to_iferret_log();
    }
  }
}

void iferret_get_current_pid_uid_linux() {
  target_ulong current_task; 
  target_ulong parent_task;

  // save last pid.
  last_pid = current_pid;

  // compute current pid &c
  current_task = get_task_struct_ptr(ESP);
  if (current_task == 0) 
    return;

  copy_task_struct_slot(current_task, PID_OFFSET, PID_SIZE, (char *) &current_pid);
  copy_task_struct_slot(current_task, UID_OFFSET, UID_SIZE, (char *) &current_uid);  
  //  copy_task_struct_slot(current_task, COMM_OFFSET, COMM_SIZE, current_command);
  copy_task_struct_slot(current_task, REAL_START_TIME_OFFSET, REAL_START_TIME_SIZE, 
			(char *) &current_real_start_time);
  copy_task_struct_slot(current_task, START_TIME_OFFSET, START_TIME_SIZE, 
			(char *) &current_start_time);
  copy_task_struct_slot(current_task, PARENT_TASK_PTR_OFFSET, sizeof(char *), 
			(char *) &parent_task);
  copy_task_struct_slot(parent_task, PID_OFFSET, PID_SIZE, (char *) &parent_pid);
  copy_task_struct_slot(parent_task, UID_OFFSET, UID_SIZE, (char *) &parent_uid);

  if (! (current_pid_valid())) {
    // map pids that can't be valid to 0
    current_pid = 0;
    current_uid = 0;
  }


  if (no_pid_flag == 1) {
    //    printf ("1 last_pid=%d current_pid is %d\n", last_pid, current_pid);
    write_current_pid_to_iferret_log();
    write_current_uid_to_iferret_log();
    no_pid_flag = 0;
  }
  else {
    if (last_pid != current_pid) {
      //      printf ("2 last_pid=%d current_pid is %d\n", last_pid, current_pid);
      write_current_pid_to_iferret_log();
      write_current_uid_to_iferret_log();
    }
  }

  //  copy_task_struct_slot(parent_task, COMM_OFFSET, COMM_SIZE, parent_command);
}

void iferret_get_current_pid_uid() {
  switch(iferret_target_os) {
    case OS_LINUX:
        iferret_get_current_pid_uid_linux();
        break;
    case OS_WINXPSP2:
        iferret_get_current_pid_uid_win();
        break;
  }
}

static inline uint32_t get_uint32_t_phys(uint32_t virt_addr) {
  target_phys_addr_t paddr;
  uint32_t retval;
  paddr = cpu_get_phys_addr(env, virt_addr);
  if (paddr!=-1) {
    iferret_cpu_physical_memory_read((unsigned long) paddr, (char*) &retval, sizeof(uint32_t));
  }    
  return(retval);
}

void iferret_check_log_full() {
  // NB: if we *arent* doing info-flow, then
  // we can get away with checking the log only
  // whenever we do any syscall logging.  
  // but, if we *are* doing info-flow, there's not much point, eh? 
#ifndef IFERRET_INFO_FLOW
  // check if info flow log is anywhere near overflow
  check_rollup("iferret_syscall.c");
  /*
  if ((iferret_log_ptr - iferret_log_base) + 10000 > IFERRET_LOG_SIZE) {
    iferret_log_rollup();
  }
  */
#endif
}

// write an entry to iferret log to capture
// context (eip & pid), number, and arguments of 
// current system call 
void iferret_log_syscall_enter_lin (uint8_t is_sysenter, uint32_t eip_for_callsite) {
#ifdef IFERRET_SYSCALL

  /*
  if (iferret_in_kernel() || (!(current_pid_valid()))) {
    return;
  }
  */

  iferret_check_log_full();
  iferret_get_current_pid_uid();

  //target_phys_addr_t paddr; 
  //char tempbuf[1204];
  target_ulong current_task;
  char command[COMM_SIZE];
  char str1[MAX_STRING_LEN], str2[MAX_STRING_LEN], str3[MAX_STRING_LEN];
  int pid, uid; // , len, i;
  
  
  // find current_task, the ptr to the currently executing process' task_struct
  current_task = get_task_struct_ptr(ESP);
  if (current_task == 0) 
    return;
  
  // grab process id, uid and command string. 
  copy_task_struct_slot(current_task, PID_OFFSET, PID_SIZE, (char *) &pid);
  copy_task_struct_slot(current_task, UID_OFFSET, UID_SIZE, (char *) &uid);
  copy_task_struct_slot(current_task, COMM_OFFSET, COMM_SIZE, command);


  /*
  if (eip_for_callsite < 1000) {
    if (pid !=0 && pid < 32768) {
      printf ("huh?  eip is 0x%x and pid is %d\n", eip_for_callsite, pid);
    }
  }
  */
  
  //  init_table();
  
  
  /*
  if ((EAX==11) || (EAX==119)
      ) {
    return;
  }
  */
  
  
  {
    iferret_syscall_t sc, *scp;
    
    scp = &sc;
        
    scp->is_sysenter = is_sysenter;
    scp->pid = pid;
    scp->callsite_eip = eip_for_callsite;
    scp->command = command;
    scp->eax = EAX;
    scp->ebx = EBX;

    scp->op_num = EAX + IFLO_SYS_CALLS_START + 1;



    // SIGRETURN just clogs things up and doesn't indicate much of use
    if (scp->op_num == IFLO_SYS_SIGRETURN) {
      return;
    }

    
    if (EAX==102) {
      // sys_socketcall has 17 sub-possibilities.
      // EBX=1 (socket) ... EBX=17 (recvmsg)
      // each with its own arg format.
      scp->op_num = IFLO_SYS_SOCKETCALLS_START + EBX;      
    }

    /*
    printf ("Pushing syscall ");
    iferret_syscall_print(*scp);
    printf ("\n");
    */

    // EXIT_GROUP doesn't return, so why push it?
    if (scp->op_num != IFLO_SYS_SYS_EXIT_GROUP) {
      // manage Ryan's stack
      iferret_syscall_stack_push(*scp);    
    }

    if (EAX==102) {
      iferret_log_socketcall(scp);
      return;
    }
    
    /*
    if (scp->op_num == IFLO_SYS_SYS_READ) {
      printf ("its a read.\n");
      if ((phys_addr(ECX)) == 0) {
	printf ("phys_addr(ECX) returned 0, i.e. %d  -- so we are to write to an address that's not mapped? \n", phys_addr(ECX));
      }
    }
    */    

    
    // fprintf(logfile, "PID: %d, stack size:%d\n",pid,get_stack_size(pid));
    
    // this is the big switch stmt.  auto-generated by make_iferret_code.pl
#include "./iferret_syscall_switch.h"    
  }
#endif
}

void iferret_log_syscall_enter_win (uint8_t is_sysenter, uint32_t eip_for_callsite) {
#ifdef IFERRET_SYSCALL
    iferret_check_log_full();
    iferret_get_current_pid_uid();

    char command[COMM_SIZE];
    int pid;
    iferret_get_current_command_win(command);
    pid = iferret_get_current_pid_win();

    {
        iferret_syscall_t sc, *scp;
        
        scp = &sc;
            
        scp->is_sysenter = is_sysenter;
        scp->pid = pid;
        scp->callsite_eip = eip_for_callsite;
        scp->command = command;
        scp->eax = EAX;
        scp->ebx = EBX;

        scp->op_num = EAX + IFLO_SYS_WINCALLS_START + 1;

        // NtContinue doesn't return
        if (scp->op_num != IFLO_SYS_NTCONTINUE_32) {
          // manage Ryan's stack
          iferret_syscall_stack_push(*scp);    
        }

// this is the big switch stmt.  auto-generated by make_iferret_code.pl
#include "./iferret_syscall_switch_xpsp2.h"
    }
#endif
}

void iferret_log_syscall_enter (uint8_t is_sysenter, uint32_t eip_for_callsite) {
    printf("syscall enter is_sysenter=%d eip_for_callsite=%08x\n", is_sysenter, eip_for_callsite);
    switch(iferret_target_os) {
        case OS_LINUX:
            iferret_log_syscall_enter_lin(is_sysenter, eip_for_callsite);
            break;
        case OS_WINXPSP2:
            iferret_log_syscall_enter_win(is_sysenter, eip_for_callsite);
            break;
    }
}

// log system call return value
// is_ret is 1 (TRUE) iff this is a return via iret.
// is_ret is 0 (FALSE) iff this is a return via sys_exit.
// callsite_esp is stack pointer for callsite. 
// what's this another_eip?  
void iferret_log_syscall_ret_linux(uint8_t is_iret, uint32_t callsite_esp, uint32_t another_eip) {
#ifdef IFERRET_SYSCALL
  uint8_t is_sysenter;
  target_ulong current_task;
  target_phys_addr_t paddr, eip_for_callsite;
  iferret_syscall_stack_element_t element;

  int pid,uid;
  char command[COMM_SIZE];

  /*
  if (iferret_in_kernel() || (!(current_pid_valid()))) {
    return;
  }
  */

  iferret_check_log_full();
  iferret_get_current_pid_uid();
    
  // get addr of pointer to current task
  current_task = get_task_struct_ptr(callsite_esp);
  if (current_task == 0) 
    return;

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
    //    iferret_spit_stack(ESP,"stack (iret)");
    paddr = cpu_get_phys_addr(env, ESP+4*3);
    is_sysenter = 0;
  }
  else {
    // Return was via sysexit.
    // Why is callsite eip at ECX+4*3? 
    //    iferret_spit_stack(ECX,"stack (sysexit)");
    paddr = cpu_get_phys_addr(env, ECX+4*3);
    is_sysenter = 1;
  }
  if (paddr!=-1) {
    iferret_cpu_physical_memory_read(paddr, (char *) &eip_for_callsite, 4);    
    // find corresponding call to do_interrupt or sys_enter that preceded this return
    /*
    printf ("looking for syscall is_iret=%d pid=%d eip_for_callsite=%x ", 
	    is_iret, pid, eip_for_callsite);
    iferret_syscall_stacks_print();
    */
    if (is_iret) {
      element = iferret_syscall_stack_get_with_eip(pid, eip_for_callsite, another_eip);
      //      printf ("another_eip=%x\n", another_eip);
    }
    else {
      element = iferret_syscall_stack_get_with_eip(pid, eip_for_callsite, -1);
      //      printf ("\n");
    }
    if (element.syscall.callsite_eip != -1){
      /*
            printf ("  found it ret_val=%d ", EAX);
            iferret_syscall_print(element.syscall);
            printf ("\n");
      */
      // found it!  Log it. 
      // NB: PID & EIP should be enough to match up.  EAX is the retval. 
      //      IFLS_IIII(IRET_OR_SYSEXIT, pid, eip_for_callsite, element.syscall_num, EAX);
      if (is_iret) {
	iferret_log_sysret_op_write_44444(IFLO_IRET, pid, eip_for_callsite, another_eip, element.syscall.eax, EAX);
      }
      else {
	iferret_log_sysret_op_write_44444(IFLO_SYSEXIT_RET, pid, eip_for_callsite, another_eip, element.syscall.eax, EAX);
      }
      if (element.syscall.eax == 120) {
	//	printf ("came back from a clone.  %d spawned %d \n", element.syscall.pid, EAX);
	iferret_log_op_write_44(IFLO_SPAWN_NEW_PID,EAX,element.syscall.pid);
      }

      // and remove that call site item from the stack
      iferret_syscall_stack_delete_at_index(pid, element.index);
    }
    else {
      //     printf ("  not found.\n");
    }
  }
#endif
}

void iferret_log_syscall_ret_win(uint8_t is_iret, uint32_t callsite_esp, uint32_t another_eip) {
#ifdef IFERRET_SYSCALL
    iferret_syscall_stack_element_t element;
    target_ulong eip_for_callsite;
    int pid;

    // We don't care about INT-based syscalls for now
    if (is_iret) return;

    iferret_check_log_full();
    iferret_get_current_pid_uid();

    pid = iferret_get_current_pid_win();

    // On Windows callsite is second down on the stack
    // Note: is this an invariant?
    if(!cpu_virtual_memory_read(env, ECX+4, (char *) &eip_for_callsite, 4))
        return;

    element = iferret_syscall_stack_get_with_eip(pid, eip_for_callsite, -1);
    if (element.syscall.callsite_eip != -1) {
        printf ("syscall exit found: is_iret=%d pid=%d eip_for_callsite=%x\n", is_iret, pid, eip_for_callsite);
        iferret_log_sysret_op_write_44444(IFLO_SYSEXIT_RET, pid, eip_for_callsite, another_eip, element.syscall.eax, EAX);
        iferret_syscall_stack_delete_at_index(pid, element.index);
    }
    else {
        printf("Syscall exit but no matching enter found!\n");
        printf ("syscall is_iret=%d pid=%d eip_for_callsite=%x\n", is_iret, pid, eip_for_callsite);
    }
#endif
}

void iferret_log_syscall_ret(uint8_t is_iret, uint32_t callsite_esp, uint32_t another_eip) {
#ifdef IFERRET_SYSCALL
    switch(iferret_target_os) {
        case OS_LINUX:
            iferret_log_syscall_ret_linux(is_iret, callsite_esp, another_eip);
            break;
        case OS_WINXPSP2:
            iferret_log_syscall_ret_win(is_iret, callsite_esp, another_eip);
            break;
    }
#endif
}

// log system call (via sysexit) return value 
void iferret_log_syscall_ret_sysexit(uint32_t callsite_esp) {
#ifdef IFERRET_SYSCALL
  /*
  if (iferret_in_kernel() || (!(current_pid_valid()))) {
    return;
  }
  */
  iferret_log_syscall_ret(0, callsite_esp, -1);
#endif
}


// log system call (vial iret) return value 
void iferret_log_syscall_ret_iret(uint32_t callsite_esp, uint32_t another_eip) {
#if IFERRET_SYSCALL 
  /*
  if (iferret_in_kernel() || (!(current_pid_valid()))) {
    return;
  }
  */
  iferret_log_syscall_ret(1, callsite_esp, another_eip);
#endif
}




