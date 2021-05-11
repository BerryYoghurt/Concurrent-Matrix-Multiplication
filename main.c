#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "parsing.h"

int **a, **b, **c, rows_a, cols_a, rows_b, cols_b;

void *thread_per_mat();
void *thread_per_row();
void *thread_per_elem();
void print_mat(FILE *, int **, int, int);

int main(int argc,char *argv[]){
    FILE*c_file;
    int err;
    char *a_name = "a.txt", *b_name = "b.txt", *c_name = "c.txt";
    if(argc == 4){
        a_name = argv[1];
        b_name = argv[2];
        c_name = argv[3];
    }else if(argc != 1){
        printf("Please provide the name of exactly 3 files.\n");
        exit(1);
    }

    err = parse_input(a_name, &rows_a, &cols_a, &a);
    if(err > 0){
        printf("An error occured\n"); //TODO handle errors properly
    }else{
        err = parse_input(b_name, &rows_b, &cols_b, &b);
        if(err > 0){
            printf("An error occured\n");
        }else{
            /*printf("Matrix A\n");
            print_mat(a, rows_a, cols_a);
            printf("Matrix B\n");
            print_mat(b, rows_b, cols_b);*/
            c_file = fopen(c_name,"w");
            c = (int**) malloc(rows_a * sizeof(int*));
            for(int i = 0; i < rows_a; i++){
                c[i] = (int*) malloc(cols_b * sizeof(int));
            }
            fprintf(c_file,"Method 1:\n");
            thread_per_mat();
            print_mat(c_file,c,rows_a, cols_b);
        }
    }


    //write results
    /*c_file = fopen(c_name, "w");
    fclose(c_file);*/
    free(a);
    free(b);
    free(c);
    fclose(c_file);
    return 0;
}


void print_mat(FILE* file, int **mat, int rows, int cols){
    for(int i = 0; i < rows; i++){
        for(int j = 0; j < cols; j++){
            fprintf(file, "%7d\t",mat[i][j]);
        }
        fprintf(file,"\n");
    }
}

void *thread_per_mat(){
    int res;
    for(int row_a = 0; row_a < rows_a; row_a++){
        for(int col_b = 0; col_b < cols_b; col_b++){
            res = 0;
            for(int k = 0; k < cols_a; k++){
                res += a[row_a][k] * b[k][col_b];
            }
            c[row_a][col_b] = res;
        }
    }
    return NULL;
}