#ifndef _IFERRET_SYSCALL_H_
#define _IFERRET_SYSCALL_H_

#include "iferret_log.h"

void iferret_log_syscall_enter(uint8_t is_sysenter, uint32_t callsite_eip);

void iferret_log_syscall_ret_sysexit(uint32_t callsite_esp);

void iferret_log_syscall_ret_iret(uint32_t callsite_esp, uint32_t callsite_eip);
void iferret_log_socketcall_write_va(iferret_syscall_t *sc, iferret_log_op_enum_t op_num, ...);

void iferret_log_socketcall(iferret_syscall_t *scp) {

#endif
