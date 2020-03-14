
fari : fari.o
	gcc -o fari fari.o -ljansson -Wall -Werror -Wextra -Wformat
fari.o : fari.c fari.h
	gcc -o fari.o -c fari.c -Wall 
