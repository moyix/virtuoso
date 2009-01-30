struct syscall_entry{
	int syscall_num;
	int eip;
};

struct syscall_stack{
	int size;
	int capacity;
	struct syscall_entry* stack;
};



//int table_initialized=0;

//struct syscall_stack table[32768];

void init_table(void); 

void add_element(int PID, int eip, int syscall_num);

int get_stack_size(int pid);

void del_element(int PID, int offset);

struct syscall_entry find_element(int PID, int offset);

struct syscall_entry find_element_with_eip(int PID, int this_eip, int another_eip);
