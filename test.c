#include "stack.h"

int main(int argc, char** argv){
	key_t key;
	int i=0;
	char a;
	struct stack_t* stack;
	pid_t pid, i1;
	key = ftok(argv[1], 0);
	if (argc!=2)
		return -1;
	for (i=0; i<15; i++){
		if (fork()==0){
			stack = attach_stack(key, sizeof(pid_t));
			if (stack == NULL) {
				printf("sth wrong");
				return -1;
			}
			pid = getpid();
			for (a=0; a<10; a++)
				if (push(stack, &pid)<0) printf("%d: wrong push\n", pid);
			printf("%d: push complete; stack size = %d\n", pid, get_count(stack));
		        for (a=0; a<10; a++){
				if (pop(stack, &i1)<0) printf("%d: wrong pop\n", getpid());
				printf("%d ", i1);
			}
			printf("\n%d: pop complete; stack size = %d\n", getpid(), get_count(stack));
			detach_stack(stack);
			return 0;
		}
	}
	stack = attach_stack(key, sizeof(pid_t));
	printf("%d\n", get_count(stack));
        mark_destruct(stack);
        detach_stack(stack);
	return 0;
}
