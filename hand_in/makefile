all: matMultp

parsing.o: parsing.c parsing.h
	gcc -c $^ -I.

main.o: main.c parsing.h
	gcc -c $^ -I.

matMultp: main.o parsing.o
	gcc -o matMultp main.o parsing.o -lpthread
