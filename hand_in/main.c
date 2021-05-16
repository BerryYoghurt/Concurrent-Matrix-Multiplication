#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>
#include "parsing.h"

int (*a)[] = NULL, (*b)[] = NULL, (*c)[] = NULL, rows_a, cols_a, rows_b, cols_b;

typedef struct element_info{
    int row;
    int col;
} element_info;

/*multiplies the two matrices*/
void *thread_per_mat();
/* multiplies a row of a by each column of b and stores the results in c
 * @param integer, the index of the row of c to be filled*/
void *thread_per_row(void *);
/* multiplies a row of a by a column of b and stores the result in c
 * @param pointer to element_info, the coordinates of the element of c to be filled
 * Note that this thread frees the memory allocated to the element_info*/
void *thread_per_elem(void *);
/* prints the matrix specified as the fourth parameter, with dimensions
 * specified as the second and third arguments,
 * into the file stream passed as the first argument*/
void print_mat(FILE *, int r, int c, int[r][c]);

int main(int argc,char *argv[]){
    FILE*c_file = NULL;
    int err[2];
    char *a_name = "a.txt", *b_name = "b.txt", *c_name = "c.txt";
    struct timeval start, stop;

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

        if(cols_a != rows_b){
            printf("The number of columns of the first matrix does not match that of the second\n");
        }else{
            /*printf("Matrix A\n");
        print_mat(stdout, rows_a, cols_a, a);
        printf("Matrix B\n");
        print_mat(stdout,rows_b, cols_b, b);*/

            c_file = fopen(c_name,"w");
            c = (int (*)[])malloc(sizeof(int[rows_a][cols_b]));

            fprintf(c_file,"Method 1:\n");
            gettimeofday(&start, NULL);
            //Multiplication Logic
            thread_per_mat();

            gettimeofday(&stop, NULL);
            print_mat(c_file, rows_a, cols_b, c);
            fprintf(c_file, "Seconds taken: %lu\n", stop.tv_sec-start.tv_sec);
            fprintf(c_file, "Microseconds taken: %lu\n", stop.tv_usec-start.tv_usec);

            memset(c, 0, rows_a*cols_b*sizeof(int)); // to make sure it works

            /*printf("Matrix A\n");
            print_mat(stdout, rows_a, cols_a, a);
            printf("Matrix B\n");
            print_mat(stdout,rows_b, cols_b, b);*/

            fprintf(c_file, "Method 2:\n");
            pthread_t threads[rows_a];
            gettimeofday(&start, NULL);
            //Multiplication logic
            for(int i = 0; i < rows_a; i++){
                if(pthread_create(&threads[i], NULL, thread_per_row, (void*) i)){
                    printf("Error creating thread\n");
                }
            }
            for(int i = 0; i < rows_a; i++){
                pthread_join(threads[i], NULL);
            }

            gettimeofday(&stop, NULL);
            print_mat(c_file, rows_a, cols_b, c);
            fprintf(c_file, "Seconds taken: %lu\n", stop.tv_sec-start.tv_sec);
            fprintf(c_file, "Microseconds taken: %lu\n", stop.tv_usec-start.tv_usec);

            memset(c, 0, rows_a*cols_b*sizeof(int)); // to make sure it works

            fprintf(c_file, "Method 3\n");
            pthread_t threads_mat[rows_a][cols_b];
            element_info (*inf)[rows_a][cols_b] = malloc(sizeof(element_info[rows_a][cols_b]));//this saves
            //some memory in comparison with allocating each single element
            gettimeofday(&start, NULL);
            //Multiplication Logic
            for(int i = 0; i < rows_a; i++){
                for(int j = 0; j < cols_b; j++){
                    /*element_info *inf = malloc(sizeof(element_info));
                    inf->row = i;
                    inf->col = j;*/
                    (*inf)[i][j].row = i;
                    (*inf)[i][j].col = j;
                    if(pthread_create(&threads_mat[i][j], NULL, thread_per_elem, &(*inf)[i][j])){
                        printf("Cannot create thread\n");
                    }
                }
            }

            for(int i = 0; i < rows_a; i++){
                for(int j = 0; j < cols_b; j++){
                    pthread_join(threads_mat[i][j], NULL);
                }
            }
            gettimeofday(&stop, NULL);

            free(inf);
            print_mat(c_file, rows_a, cols_b, c);
            fprintf(c_file, "Seconds taken: %lu\n", stop.tv_sec-start.tv_sec);
            fprintf(c_file, "Microseconds taken: %lu\n", stop.tv_usec-start.tv_usec);
        }
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
    int res, idx = (int)row_index, (*mat_a)[cols_a], (*mat_b)[cols_b], (*mat_ans)[cols_b];
    printf("in row %d\n", idx);

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

void *thread_per_elem(void *info){
    element_info inf = *(element_info*) info;
    int res = 0, (*mat_a)[cols_a], (*mat_b)[cols_b], (*mat_c)[cols_b];
    mat_a = a; mat_b = b; mat_c = c;
    for(int i = 0; i < cols_a; i++){
        res += mat_a[inf.row][i] * mat_b[i][inf.col];
    }
    mat_c[inf.row][inf.col] = res;
    //free(info);
    return NULL;
}
