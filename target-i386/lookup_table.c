
int table_initialized=0;

struct syscall_stack table[32768];

void init_table(){
  int i;

  if (table_initialized == 0) {
    for(i=0; i<32768; i++){
      table[i].size =0;
      table[i].capacity =0;
      table[i].stack=0;
    }
    table_initialized = 1;
  }
}

// push this (eip,syscall_num) pair to stack for PID.
void add_element(int PID, int eip, int syscall_num){
  int old_syscall_num;

  if(PID > 32767){
    printf("Error PID out of range\n");
    exit(1);
  }
	
  //check to see if there's capacity in the stack
  if(table[PID].size + 1 <= table[PID].capacity){ //then there's room
    table[PID].size++;
    table[PID].stack[table[PID].size-1].syscall_num = syscall_num;	
    table[PID].stack[table[PID].size-1].eip = eip;		
  }else{ //we need to allocate space
    if(table[PID].capacity == 0){
      table[PID].stack = malloc(sizeof(struct syscall_entry)*2);
      table[PID].capacity = 2;
    } else {
      table[PID].stack = realloc(table[PID].stack,sizeof(struct syscall_entry)*table[PID].capacity*2);
      table[PID].capacity *= 2;
    }
    table[PID].size++;
    table[PID].stack[table[PID].size-1].syscall_num = syscall_num;	
    table[PID].stack[table[PID].size-1].eip = eip;		
  }

}

int get_stack_size(int pid){
  return table[pid].size;
}

void del_element(int PID, int offset){
  int i;	

  if(PID > 32767){
    printf("Error PID out of range\n");
    exit(1);
  }
  if(table[PID].size == 0){
    printf("Error, attempting to underflow\n");
    exit(1);
  }

  if(offset == 0){
    table[PID].size -= 1;
  }
  else{
    for(i=table[PID].size - offset - 1; i < table[PID].size - 1; i++){
      table[PID].stack[i].eip = table[PID].stack[i+1].eip;
      table[PID].stack[i].syscall_num = table[PID].stack[i+1].syscall_num;
    }
    table[PID].size -= 1; 
  }
}

struct syscall_entry find_element(int PID, int offset){
  struct syscall_entry default_return;
  default_return.eip = -1;
  default_return.syscall_num = -1;

  if(PID > 32767){
    printf("Error PID out of range\n");
    exit(1);
  }
  /*	if(table[PID] == -1){
	printf("Error no syscall for this PID\n");
	exit(1);
	}
  */
  if(table[PID].size - offset <= 0){
    return default_return;		
  }
  return table[PID].stack[table[PID].size-1-offset];
}


// search the stack (start with most recently added element) 
// for this pid linearly until you find first item matching eip. 
// Return that element. 
// NB: special element with .eip slot set to -1 will be returned if not found. 
struct syscall_entry find_element_with_eip(int PID, int this_eip, int another_eip) {
  int offset;
  struct syscall_entry syscall_element;
  
  offset = 0; 
  do {
    syscall_element = find_element(pid,offset);
    offset++;
  } while ((syscall_element.eip != -1)
	   
	   && (syscall_element.eip != this_eip)
	   && ((or_this_eip != -1) && (syscall_element.eip != another_eip)));
  return (syscall_element);
}


/*
  void print_table(){
  int i;

  for(i=0;i<32768;i++){
  if(table[i].syscall_num != -1)
  printf("PID: %d syscall:%d\n",i,table[i].syscall_num);
  }
  }*/

/*
  int main(int argc, char** argv){
  init_table();

  add_element(13,4);
  add_element(133,10);
  add_element(144,15);
  add_element(32455,255);
  add_element(14155,223);
  add_element(1555,16);
  add_element(32000,0);
  add_element(1,35);
  add_element(0,4);

  del_element(14155);
  del_element(1555);
  del_element(1);

  printf("syscall for 144: %d\n",find_element(144));

  print_table();
  }*/
