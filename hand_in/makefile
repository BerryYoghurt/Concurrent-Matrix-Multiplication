all: matmult

parsing.o: parsing.c parsing.h
	gcc -c $^ -I.

main.o: main.c parsing.h
	gcc -c $^ -I.

matmult: main.o parsing.o
	gcc -o matmult main.o parsing.o -lpthread
