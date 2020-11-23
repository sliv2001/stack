all:
	gcc -O0 -g stack.c a1.c -std=c11 -o a1
	gcc -O0 -g stack.c a2.c -std=c11 -o a2
	gcc -O0 -g stack.c test.c -std=c11 -o test

release:
	gcc stack.c test.c -std=c11 -o test

clean:
	rm ./a.out ./a1 ./a2
