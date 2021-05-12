

#ifndef MATMULT_PARSING_H
#define MATMULT_PARSING_H
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define MAX_BUF 100
//error codes
#define NOT_ENOUGH_NUMBERS 100
#define NEGATIVE_DIMENSIONS 101
#define NO_DIMENSIONS 102
#define IO_ERROR 103

int parse_input(const char*, int*, int*, int (**)[]);
_Bool handle_input_error(int, const char*);

#endif //MATMULT_PARSING_H

