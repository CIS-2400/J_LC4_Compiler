all: jc

stack.o : stack.c stack.h
	gcc -g -Wall -std=gnu11 -c stack.c

jc : jc.o token.o stack.o
	gcc -g -Wall -std=gnu11 -o jc jc.o token.o stack.o

jc.o : jc.c token.h stack.h
	gcc -g -Wall -std=gnu11 -c jc.c

token.o : token.c token.h
	gcc -g -Wall -std=gnu11 -c token.c

clean:
	rm -f *.o jc