default : myshell.o builtin.o argparse.o
	gcc myshell.c -g -Wall -Werror -o myshell builtin.o argparse.o

myshell : myshell.o builtin.o argparse.o
	gcc myshell.c -g -Wall -Werror -o myshell builtin.o argparse.o

builtin.o : builtin.c builtin.h
	gcc -g -Wall -Werror -c builtin.c

argparse.o : argparse.c argparse.h
	gcc -g -Wall -Werror -c argparse.c

clean : 
	rm -f *.o *~* *.exe