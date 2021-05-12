#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "parsing.h"

int (*a)[] = NULL, (*b)[] = NULL, (*c)[] = NULL, rows_a, cols_a, rows_b, cols_b;

void *thread_per_mat();
void *thread_per_row(void *);
void *thread_per_elem();
void print_mat(FILE *, int r, int c, int[r][c]);

int main(int argc,char *argv[]){
    FILE*c_file = NULL;
    int err[2];
    char *a_name = "a.txt", *b_name = "b.txt", *c_name = "c.txt";

    if(argc == 4){
        a_name = argv[1];
        b_name = argv[2];
        c_name = argv[3];
    }else if(argc != 1){
        printf("Please provide the name of exactly 3 files.\n");
        exit(1);
    }

    err[0] = parse_input(a_name, &rows_a, &cols_a, &a);
    err[1] = parse_input(b_name, &rows_b, &cols_b, &b);

    if(!handle_input_error(err[0], "first") && !handle_input_error(err[1], "second")){

        /*printf("Matrix A\n");
        print_mat(stdout, rows_a, cols_a, a);
        printf("Matrix B\n");
        print_mat(stdout,rows_b, cols_b, b);*/

        c_file = fopen(c_name,"w");
        c = (int (*)[])malloc(sizeof(int[rows_a][cols_b]));

        fprintf(c_file,"Method 1:\n");
        //TODO TIME
        thread_per_mat();
        print_mat(c_file, rows_a, cols_b, c);

        memset(c, 0, rows_a*cols_b*sizeof(int)); // to make sure it works

        /*printf("Matrix A\n");
        print_mat(stdout, rows_a, cols_a, a);
        printf("Matrix B\n");
        print_mat(stdout,rows_b, cols_b, b);*/

        fprintf(c_file, "Method 2:\n");
        pthread_t threads[rows_a];
        //TODO TIME
        for(int i = 0; i < rows_a; i++){
            if(pthread_create(&threads[i], NULL, thread_per_row, (void*) &i)){
                printf("Error creating thread\n");
            }
        }
        for(int i = 0; i < rows_a; i++){
            pthread_join(threads[i], NULL);
        }
        print_mat(c_file, rows_a, cols_b, c);

    }


    /*printf("Matrix A\n");
    print_mat(a, rows_a, cols_a);
    printf("Matrix B\n");
    print_mat(b, rows_b, cols_b);*/


    if(a != NULL)
        free(a);
    if(b != NULL)
        free(b);
    if(c != NULL)
        free(c);
    if(c_file != NULL)
        fclose(c_file);
    return 0;
}


void print_mat(FILE* file, int rows, int cols, int mat[rows][cols]){
    for(int i = 0; i < rows; i++){
        for(int j = 0; j < cols; j++){
            fprintf(file, "%7d\t",mat[i][j]);
        }
        fprintf(file,"\n");
    }
}


void *thread_per_mat(){
    int res, (*mat_a)[cols_a], (*mat_b)[cols_b], (*mat_ans)[cols_b];
    mat_a = a;
    mat_b = b;
    mat_ans = c;
    for(int row_a = 0; row_a < rows_a; row_a++){
        for(int col_b = 0; col_b < cols_b; col_b++){
            res = 0;
            for(int k = 0; k < cols_a; k++){
                res += mat_a[row_a][k] * mat_b[k][col_b];
            }
            mat_ans[row_a][col_b] = res;
        }
    }
    return NULL;
}

void *thread_per_row(void *row_index){
    int res, idx = *(int*)row_index, (*mat_a)[cols_a], (*mat_b)[cols_b], (*mat_ans)[cols_b];
    if(idx == 0){
        printf("In thread 0\n");
    }

    mat_a = a;
    mat_b = b;
    mat_ans = c;
    for(int col_b = 0; col_b < cols_b; col_b++){
        res = 0;
        for(int k = 0; k < cols_a; k++){
            res += mat_a[idx][k] * mat_b[k][col_b];
        }
        mat_ans[idx][col_b] = res;
    }
    return NULL;
}