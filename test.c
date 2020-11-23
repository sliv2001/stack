#include "stack.h"

int main(int argc, char** argv){
	key_t key;
	int i1, i=0;
	char a;
	struct stack_t* stack;
	key = ftok(argv[1], 0);
	if (argc!=2)
		return -1;
	for (i=0; i<15; i++){
		if (fork()==0){
			stack = attach_stack(key, 1);
			if (stack == NULL) {
				printf("sth wrong");
				return -1;
			}
			for (a=0; a<10; a++)
				push(stack, &a);
		        for (a=0; a<10; a++)
				pop(stack, &i1);
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
