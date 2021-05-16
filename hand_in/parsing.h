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

/* parses the matrix input file
 *
 * The function does not detect number format errors. If a number in the file
 * is not a valid decimal integer, a zero is read instead.
 *
 * @param name : the name of the file to parse
 * @param rows_ptr : the function stores in it the number of rows written in the file
 * @param cols_ptr : same as above for rows
 * @param matrix : a pointer to the matrix that should be read. Memory for this matrix
 * is allocated within this function.*/
int parse_input(const char* name, int* rows_ptr, int* cols_ptr, int (**matrix)[]);
/* handles the errors returned by parse_input and outputs a suitable message to stdout.
 * @param name : the name of the matrix
 * @return false if no error has occured, true otherwise*/
_Bool handle_input_error(int errno, const char* name);

#endif //MATMULT_PARSING_H

