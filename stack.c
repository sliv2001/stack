#include "stack.h"

const size_t elemcount = 100*(2^15);

struct timespec TIME = {0, 0};
int timeval=0;

int setSem(struct stack_t* stack){
	struct sembuf semb;
	int done=0;
	semb.sem_num = 0;
	semb.sem_op = -1;
	semb.sem_flg = SEM_UNDO;
	if (timeval>0&&(TIME.tv_sec>0||TIME.tv_nsec>0)){
		semb.sem_flg|=IPC_NOWAIT;
		if ((done=semop(stack->sem, &semb, 1))<0&&errno == EAGAIN){
			nanosleep(&TIME, NULL);
			return semop(stack->sem, &semb, 1);
		}
		if (done<0&&errno!=EAGAIN) return -1;
	}
	if (timeval==0)
		return semop(stack->sem, &semb, 1);
	if (timeval<0) {
		semb.sem_flg |= IPC_NOWAIT;
		return semop(stack->sem, &semb, 1);
	}
}

int unsetSem(struct stack_t* stack){
	struct sembuf semb;
	semb.sem_num = 0;
	semb.sem_op = 1;
	semb.sem_flg = SEM_UNDO;
	return (semop(stack->sem, &semb, 1));
}

int wrStack(stack_h s, struct stack_t* stack, void* addr) {
	*((stack_h*)addr) = s;
	return 0;
};

int rdStack(stack_h* s, struct stack_t* stack, void* addr) {
	*s = *((stack_h*)addr);
	return 0;
}

int initSem(int sem){
	union semun {
               int              val;
               struct semid_ds *buf;
               unsigned short  *array;
               struct seminfo  *__buf;
        } s;
	s.val = 1;
	return semctl(sem, 0, SETVAL, s);
}

struct stack_t* attach_stack(key_t key, int size){
	void* addr;
	int newStack=1, newSem=1;
	struct shmid_ds str;
	struct stack_t* stack;
	stack_h s;
	int sem = semget(key, 1, IPC_CREAT|IPC_EXCL|0666);
	int shmem = shmget(key, elemcount*size+sizeof(stack_h), IPC_CREAT|0666);  //здесь можно поправить длину памяти
	if (shmem<0) {
		return NULL;
	}
	if (sem<0){
		sem = semget(key, 1, IPC_CREAT|0666);
		newSem = 0;
		if (sem<0)
			return NULL;
	}
	stack = (struct stack_t*)malloc(sizeof(struct stack_t));
	stack->shmem = shmem;
	stack->sem = sem;
	addr = shmat(shmem, 0, 0);
	stack->addr = addr;
	shmctl(shmem, IPC_STAT, &str);
	if (str.shm_cpid!=getpid())
		newStack=0;
	if (addr == (void*)-1)
		return NULL;
	if (newSem||newStack)
		initSem(sem);
	if (newStack) {
		s.size = size;
		s.top = sizeof(stack_h);
		s.count = 0;
		s.progs = 0;
		s.mdestruct = 0;
		wrStack(s, stack, addr);
	}
	if (setSem(stack)<0) {
		mark_destruct(stack);
		detach_stack(stack);
		return NULL;
	}
	rdStack(&s, stack, addr);
	s.progs++;
	wrStack(s, stack, addr);
	unsetSem(stack);
	if (!newStack&&size!=s.size){
		detach_stack(addr);
		return NULL;
	}
	return stack;
}

int detach_stack(struct stack_t* stack){
	stack_h s;
	int a;
	if (setSem(stack)<0) return -1;
	rdStack(&s, stack, stack->addr);
	s.progs--;
	wrStack(s, stack, (void*)stack->addr);
	if (s.mdestruct&&s.progs == 0){
		semctl(stack->sem, 0, IPC_RMID);
		return shmctl(stack->shmem, IPC_RMID, NULL);
	}
	else
		a = shmdt(stack->addr);
		unsetSem(stack);
		return a;
}

int mark_destruct(struct stack_t* stack){
	stack_h s;
	if (setSem(stack)<0) return -1;
	rdStack(&s, stack, stack->addr);
	s.mdestruct = 1;
	wrStack(s, stack, (void*)stack->addr);
	return unsetSem(stack);
}

int get_size(struct stack_t* stack){
	stack_h s;
	int a;
	if (setSem(stack)<0) return -1;
	rdStack(&s, stack, stack->addr);
	a = s.size*elemcount;
	unsetSem(stack);
	return a;
}

int get_count(struct stack_t* stack){
	stack_h s;
	int a;
	if (setSem(stack)<0) return -1;
	rdStack(&s, stack, stack->addr);
	a = s.count*s.size;
	unsetSem(stack);
	return a;
}

int push(struct stack_t* stack, void* val){
	stack_h s;
	int i, done = 0;
	if (setSem(stack)<0) {
		unsetSem(stack);
		printf("sem malfuncts");
		return -1;
	}
	s = *((stack_h*)stack->addr);
	if (s.count<elemcount-1){
		for (i=0; i<s.size; i++){
			*((char*)stack->addr+s.top+i) = *((char*)val+i);
		}
		s.count++;
		s.top+=s.size;
		*((stack_h*)stack->addr) = s;
	}
	else {
		printf("stack owerflow");
		done = -2;
	}
	unsetSem(stack);
	return done;
}

int pop(struct stack_t* stack, void* val){
	stack_h s;
	int i, done = 0;
	if (setSem(stack)<0) return -1;
	s = *((stack_h*)stack->addr);
	if (s.count>0){
		s.top-=s.size;
		for (i=0; i<s.size; i++){
			*((char*)val+i) = *((char*)stack->addr+s.top+i);
		}
		s.count--;
		*((stack_h*)stack->addr) = s;
	}
	else {
		val=NULL;
		done = -2;
	}
	unsetSem(stack);
	return done;
}

int set_wait(int val, struct timespec* timeout){
	timeval = val;
	if (timeout!=NULL)
		TIME = *timeout;
	return 0;
}
