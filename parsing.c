#include "parsing.h"

/*parses the two files with the provided names and allocates memory*/
int parse_input(const char *name, int *rows_ptr, int *cols_ptr, int ***mat_ptr){
    int err = 0, n_rows, n_cols, **mat, i, j;
    char line[MAX_BUF], *str_err, *token; //str error is the error from parsing a line
    FILE *file = fopen(name, "r");
    if(file == NULL){
        return IO_ERROR; //safely return without closing files
    }
    str_err = fgets(line,MAX_BUF,file);
    if(str_err == NULL){
        err = IO_ERROR;
    }else{
        err = sscanf(line," row=%d col=%d ", rows_ptr, cols_ptr);//TODO numeric overflow checkk
        if(err == EOF) {//TODO check these errors
            err = IO_ERROR;
        }else if(*rows_ptr < 0 || *cols_ptr < 0){
            err = NEGATIVE_DIMENSIONS;
        }else {
            err = 0;
            n_rows = *rows_ptr;
            n_cols = *cols_ptr;

            *mat_ptr = (int**)malloc(n_rows * sizeof(int*));
            for(i = 0; i < n_rows; i++){
                (*mat_ptr)[i] = (int*)malloc(n_cols * sizeof(int));
            }
            mat = *mat_ptr;

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