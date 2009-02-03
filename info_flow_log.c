
#include <stdlib.h>
#include <stdio.h>
#include <zlib.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>
#include "info_flow.h"

// ptr to first byte of info flow log
char *if_log_base = NULL;      

// ptr to next byte to be written in info flow log
char *if_log_ptr = NULL;      

uint32_t if_log_rollup_count = 0;  

char *if_keyboard_label=NULL;
uint8_t if_keyboard_label_changed = 0;

char* if_network_label=NULL;
uint8_t if_network_label_changed = 0;

// we'll be using these as "addresses" for registers
unsigned long long ifregaddr[16];


// this gets called from the qemu monitor in order to 
// change the current keyboard label
void if_set_keyboard_label(const char *label) {
  if (if_keyboard_label == NULL) 
    if_keyboard_label = (char *) malloc(IF_MAX_KEYBOARD_LABEL_LEN);
  assert (if_keyboard_label != NULL);
  printf ("if_set_keyboard_label: label is henceforth [%s]\n", label);
  strncpy(if_keyboard_label, label, IF_MAX_KEYBOARD_LABEL_LEN);
  if_keyboard_label_changed = 1;
}

// this gets called from the qemu monitor in order to 
// change the current network label
void if_set_network_label(const char *label) {
  if (if_network_label == NULL) 
    if_network_label = (char *) malloc(IF_MAX_NETWORK_LABEL_LEN);
  assert (if_network_label != NULL);
  printf ("if_set_network_label: label is henceforth [%s]\n", label);
  strncpy(if_network_label, label, IF_MAX_NETWORK_LABEL_LEN);
  if_network_label_changed = 1;
}

void if_log_create() {
  int i;
  // initial info flow log allocation.
  if_log_ptr = if_log_base = (char *) calloc (IF_LOG_SIZE,1);
  if_set_keyboard_label("keyboard_startup");
  if_set_network_label("network_startup");
  // set up ifregaddr array.
  for (i=0; i<16; i++) {
    // all addresses memory addresses will be on the interval 
    // 0 .. phys_ram_size-1.  
    // We'll pretend the registers live starting at phys_ram_size.
    // and we'll make them 8 bytes just for fun.
    ifregaddr[i] = (unsigned long long) ( ((uint64_t) phys_ram_size) + i*8);
    printf("Register i=%d is at 0x%p\r\n",i, (char *)ifregaddr[i]);
  }

}



// if flag has been set, write new network label in the log
void new_network_label_check(){
  if (if_network_label_changed) { 
    if_network_label_changed = 0; 
    printf ("info_flow_new_network_label [%s]\r\n", if_network_label); 
    IFLW_WRAPPER(
		 IFLW_PUT_OP(INFO_FLOW_OP_NEW_NETWORK_LABEL); 
		 IFLW_PUT_STRING(if_network_label);
		 );
 }
}


// save current if log to a file for some reason    
void if_log_to_file() {
  char filename[1024];
  gzFile out;
  //  FILE *fp;

  printf ("if_log_ptr - if_log_base = %d\n", if_log_ptr - if_log_base);
  printf ("IF_LOG_SIZE = %d\n", IF_LOG_SIZE);

  snprintf (filename, 1024, "/scratch/tmp/ifl-%d-%d", getpid(), if_log_rollup_count);
  //  fp = fopen (filename, "w");
  out = gzopen(filename,"wb");

  //  fwrite(if_log_base, 1, if_log_ptr-if_log_base, fp);
  gzwrite(out, (char *) if_log_base, (unsigned int) (if_log_ptr - if_log_base)); 
  //  fclose(fp);
  gzclose(out);

  printf ("wrote if log to %s\n", filename);
  if_log_rollup_count ++;

  // processing complete; ready to write over old log.  
  if_log_ptr = if_log_base; 

}




// info-flow log is full.  Dump it to a file. 
void if_log_rollup() {

  if_log_to_file();

}

