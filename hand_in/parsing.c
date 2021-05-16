#include "parsing.h"

int parse_input(const char *name, int *rows_ptr, int *cols_ptr, int (**mat_ptr)[]){
    int err = 0, n_rows, n_cols, i, j;
    char line[MAX_BUF], *str_err, *token; //str error is the error from parsing a line
    FILE *file = fopen(name, "r");
    if(file == NULL){
        return IO_ERROR; //safely return without closing files
    }
    str_err = fgets(line,MAX_BUF,file);
    if(str_err == NULL){
        err = IO_ERROR;
    }else{
        err = sscanf(line," row=%d col=%d ", rows_ptr, cols_ptr);
        if(err == EOF) {
            err = IO_ERROR;
        }else if(err != 2) {
            err = NO_DIMENSIONS;
        }else if(*rows_ptr < 0 || *cols_ptr < 0){
            err = NEGATIVE_DIMENSIONS;
        }else {
            err = 0;
            n_rows = *rows_ptr;
            n_cols = *cols_ptr;

            *mat_ptr = (int (*)[])malloc(sizeof(int[n_rows][n_cols]));
            int (*mat)[n_cols] = *mat_ptr;//cast mat_ptr to pointer to 1D array of size n_cols to be able to address it normally

            for (i = 0; i < n_rows && err == 0; i++) {
                str_err = fgets(line, MAX_BUF,file);
                if(str_err == NULL && !ferror(file)){
                    err = NOT_ENOUGH_NUMBERS;
                    break;
                }else if(ferror(file)){
                    err = IO_ERROR;
                    break;
                }

                token = strtok(line, "\t");

                for (j = 0; j < n_cols && token != NULL; j++) {
                    mat[i][j] = atoi(token); //TODO for the time being, convert to strtol
                    token = strtok(NULL,"\t");
                }
                if(token == NULL && j < n_cols){
                    err = NOT_ENOUGH_NUMBERS;
                }
            }
        }
    }

    fclose(file);
    return err;
}

_Bool handle_input_error(int err, const char* name){
    switch(err){
        case IO_ERROR:
            printf("IO_ERROR occured in %s matrix. Are you sure the file is in the "
                   "directory of the executable and correctly named?\n",name);
            break;
        case NO_DIMENSIONS:
            printf("Missing dimensions in %s matrix.\n",name);
            break;
        case NEGATIVE_DIMENSIONS:
            printf("One or both dimensions of %s matrix are negative or too large.\n", name);
            break;
        case NOT_ENOUGH_NUMBERS:
            printf("Number of elements in %s matrix file does not match dimensions.\n", name);
            break;
        default:
            break;
    }
    return err;
}