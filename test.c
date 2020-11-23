#include "stack.h"

int main(int argc, char** argv){
	key_t key;
	int i1, i=0;
	char a;
	struct stack_t* stack;
	char ch = 'g';
	key = ftok(argv[1], 0);
	if (argc!=2)
		return -1;
	for (i=0; i<2; i++){
		if (fork()==0){
			stack = attach_stack(key, 1);
			if (stack == NULL) {
				printf("sth wrong");
				return -1;
			}
			for (a=0; a<10; a++)
				if (push(stack, &a)<0) printf("%d: wrong push\n", getpid());
			printf("%d: push complete; stack size = %d\n", getpid(), get_count(stack));
		        for (a=0; a<10; a++)
				if (pop(stack, &i1)<0) printf("%d: wrong pop\n", getpid());
			printf("%d: pop complete; stack size = %d\n", getpid(), get_count(stack));
			detach_stack(stack);
			return 0;
		}
	}
	stack = attach_stack(key, 1);
	printf("%d\n", get_count(stack));
        mark_destruct(stack);
        detach_stack(stack);
	return 0;
}
