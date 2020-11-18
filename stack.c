#include "stack.h"

const size_t elemcount = 100000;

int shmem = -1;

int wrStack(stack_h s, void* addr) {
	*((stack_h*)addr) = s;
	return 0;
};

struct stack_t* attach_stack(key_t key, int size){
	void* addr;
	int newStack=1;
	stack_h s;
	shmem = shmget(key, elemcount*size+sizeof(stack_h), IPC_CREAT|IPC_EXCL);  //здесь можно поправить длину памяти
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
		s.top = sizeof(stack_h);
		s.count = 0;
		s.progs = 0;
		s.mdestruct = 0;
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
	wrStack(s, (void*)stack);
	if (s.mdestruct&&s.progs == 0)
		return shmctl(shmem, IPC_RMID, NULL);
	else
		return shmdt(stack);
}

int mark_destruct(struct stack_t* stack){
	stack_h s;
	s = *((stack_h*)stack);
	s.mdestruct = 1;
	return wrStack(s, (void*)stack);
}

int get_size(struct stack_t* stack){
	stack_h s;
	s = *((stack_h*)stack);
	return s.size*elemcount;
}

int get_count(struct stack_t* stack){
	stack_h s;
	s = *((stack_h*)stack);
	return s.count*s.size;
}

int push(struct stack_t* stack, void* val){
	stack_h s;
	int i;
	s = *((stack_h*)stack);
	for (i=0; i<s.size; i++){
		*((char*)stack+s.top+i) = *((char*)val+i);
	}
	s.count++;
	s.top+=s.size;
	*((stack_h*)stack) = s;
	return 0;
}

int pop(struct stack_t* stack, void* val){
	stack_h s;
	int i;
	s = *((stack_h*)stack);
	for (i=0; i<s.size; i++){
		*((char*)val+i) = *((char*)stack+s.top+i);
	}
	s.count--;
	s.top-=s.size;
	*((stack_h*)stack) = s;
	return 0;
}

int main(int argc, char** argv){
	return 0;
}