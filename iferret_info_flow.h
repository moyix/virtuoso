#ifndef __IFERRET_INFO_FLOW_H_
#define __IFERRET_INFO_FLOW_H_

#include "taint.h"
#include "iferret.h"

#define IF_DEBUG_OFF 0
#define IF_DEBUG_LOW 1
#define IF_DEBUG_MED 2
#define IF_DEBUG_HIGH 3
#define IF_DEBUG_OMG 4


#define wctull(p) p


#define MIN_ADDR 1000
#define MAX_ADDR 0xfffffffffffffff0
#define MIN_SIZE 1
#define MAX_SIZE 100000 


#ifndef FOO
extern uint8_t iferret_debug;
#endif



#define ctull(p) (unsigned long long) p

void iferret_info_flow_process_op(iferret_t *iferret, iferret_op_t *op);

#ifdef OTAINT
#define TRUE 1
#define FALSE 0
#endif


uint8_t info_flow_exists(iferret_t *iferret, uint64_t p, size_t n);
void info_flow_delete(iferret_t *iferret, uint64_t p, size_t n);
void info_flow_copy(iferret_t *iferret, uint64_t p1,uint64_t p2, size_t n);
void info_flow_compute(iferret_t *iferret, uint64_t p1, size_t n1, uint64_t p2, size_t n2);
void info_flow_label(iferret_t *iferret, uint64_t p, size_t n, char *label);


#endif // __INFO_FLOW_H_ 
