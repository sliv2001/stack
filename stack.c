#include "stack.h"

const size_t elemcount = 100000;

int wrStack(stack_h s, void* addr) {
	*((stack_h*)addr) = s;
	return 0;
};

struct stack_t* attach_stack(key_t key, int size){
	int shmem = shmget(key, elemcount*size+sizeof(stack_h), IPC_CREAT|IPC_EXCL);  //здесь можно поправить длину памяти
	void* addr;
	int newStack=1;
	stack_h s;
	if (shmem==(-1)) {
		shmem = shmget(key, elemcount*size+sizeof(stack_h), IPC_CREAT);
		newStack = 0;
		if (shmem==(-1))
			return NULL;
	}
	addr = shmat(shmem, 0, 0);
	if (addr == NULL) 
		return NULL;
	if (newStack) {
		s.size = size;
		s.top = (void*)sizeof(stack_h);
		s.count = 0;
		s.progs = 0;
		wrStack(s, addr);
	}
	s = *((stack_h*)addr);
	s.progs++;
	wrStack(s, addr);
	if (!newStack&&size!=s.size){
		detach_stack(addr);
		return NULL;
	}
	return (struct stack_t*)addr;
}

int detach_stack(struct stack_t* stack){
	stack_h s;
	s = *((stack_h*)stack);
	s.progs--;
	return shmdt(stack);
}

int main(int argc, char** argv){
	return 0;
}
