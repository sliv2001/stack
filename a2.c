#include "stack.h"

int main(int argc, char** argv){
        key_t key = ftok("ipc", 0);
        int la, i1=2020, i=0;
        char a;
        struct stack_t* stack = attach_stack(key, 1);
        char* str = argv[1];
        if (stack == NULL) return -1;
        while ((a=str[i])!=0){
                i++;
                push(stack, &a);
        }
        i=0;
        while ((a=str[i])!=0){
                pop(stack, &a);
                i++;
                printf("%c", a);
        }
        mark_destruct(stack);
        detach_stack(stack);
        return 0;
}







