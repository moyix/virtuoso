
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

#include "exec.h"
#include "lookup_table.h"
#include "linux_task_struct_offsets.h"
#include "../iferret_log.h"
#include "iferret_syscall.h"

extern struct CPUX86State *env;
extern pid_t pid;
extern uid_t uid;

target_phys_addr_t cpu_get_phys_addr(CPUState *env, target_ulong addr);


pid_t current_pid, last_pid;
uid_t current_uid, last_uid;

uint8_t no_pid_flag = 1;
uint8_t no_uid_flag = 1;

/* Argument list sizes for sys_socketcall */
/*
#define AL(x) ((x) * sizeof(unsigned long))
static unsigned char nargs[18]={AL(0),AL(3),AL(3),AL(3),AL(2),AL(3),
                                AL(3),AL(3),AL(4),AL(4),AL(4),AL(6),
                                AL(6),AL(2),AL(5),AL(5),AL(3),AL(3)};
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
  
  //target_phys_addr_t paddr; 
  //char tempbuf[1204];
  target_ulong current_task;
  char command[COMM_SIZE];
  char str1[MAX_STRING_LEN], str2[MAX_STRING_LEN], str3[MAX_STRING_LEN];
  int pid, uid; // , len, i;

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
    iferret_syscall_t sc, *scp;

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

    // this is the big switch stmt.  auto-generated by make_iferret_code.pl
#include "./iferret_syscall_switch.h"

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
      //      IFLS_IIII(IRET_OR_SYSEXIT, pid, eip_for_callsite, syscall_element.syscall_num, EAX);
      if (is_iret) {
	iferret_log_op_write_4444(IFLO_IRET, pid, eip_for_callsite, syscall_element.syscall_num, EAX);
      }
      else {
	iferret_log_op_write_4444(IFLO_SYSEXIT_RET, pid, eip_for_callsite, syscall_element.syscall_num, EAX);
      }
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
