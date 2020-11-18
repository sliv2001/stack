#include "stack.h"

const size_t elemcount = 100000;

struct stack_ctrl {
	int size;
	void* top;
	int count;
};

struct stack_t* attach_stack(key_t key, int size){
	int shmem = shmget(key, elemcount*size, IPC_CREAT|IPC_EXCL);  //здесь можно поправить длину памяти
	void* addr;
	int newStack=1;
	if (shmem==(-1)) {
		shmem = shmget(key, elemcount*size, IPC_CREAT);
		newStack = 0;
		if (shmem==(-1))
			return NULL;
	}
	addr = shmat(shmem, 0, 0);
	return addr;
}

int main(int argc, char** argv){
	return 0;
}
