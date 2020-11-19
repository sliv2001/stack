all:
	gcc -O0 -g stack.c a1.c -o a1
	gcc -O0 -g stack.c a2.c -o a2

clean:
	rm ./a.out ./a1 ./a2
