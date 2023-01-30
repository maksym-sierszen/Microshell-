microshell: microshell.o
	gcc -std=c99 -Wall -o microshell microshell.o -lm

microshell.o: microshell.c
	gcc -std=c99 -Wall -c -o microshell.o microshell.c

clean:
	rm -f microshell.o microshell