#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>
#include "parsing.h"
//a,b,c: each of them is a pointer to a 1D array (with unknown size because it is declared globally
// before any input)
int (*a)[] = NULL, (*b)[] = NULL, (*c)[] = NULL, rows_a, cols_a, rows_b, cols_b;

//struct to store the row, col index (used when multiplying each element on its own)
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
    char *a_name = "a.txt", *b_name = "b.txt", *c_name = "c.txt"; //default names
    struct timeval start, stop;

    if(argc == 4){
        a_name = argv[1];
        b_name = argv[2];
        c_name = argv[3];
    }else if(argc != 1){
        printf("Please provide the name of exactly 3 files.\n");
        exit(1);
    }

    err[0] = parse_input(a_name, &rows_a, &cols_a, &a); //fill matrices and report any errors
    err[1] = parse_input(b_name, &rows_b, &cols_b, &b);

    //handle possible errors
    if(!handle_input_error(err[0], "first") && !handle_input_error(err[1], "second")){

        if(cols_a != rows_b){
            printf("The number of columns of the first matrix does not match that of the second\n");
        }else{
            c_file = fopen(c_name,"w");
            //allocate enough memory to store *rows_a* 1D arrays of size cols_b
            c = (int (*)[])malloc(sizeof(int[rows_a][cols_b]));

            fprintf(c_file,"Method 1:\n");
            fprintf(stdout,"Method 1:\n");
            gettimeofday(&start, NULL);
            //Multiplication Logic
            thread_per_mat();

            gettimeofday(&stop, NULL);
            print_mat(c_file, rows_a, cols_b, c);
            fprintf(stdout, "threads created: Only Main thread\n");
            fprintf(stdout, "Seconds taken: %lu\n", stop.tv_sec-start.tv_sec);
            fprintf(stdout, "Microseconds taken: %lu\n", stop.tv_usec-start.tv_usec);

            memset(c, 0, rows_a*cols_b*sizeof(int)); // clear matrix

            fprintf(c_file, "Method 2:\n");
            fprintf(stdout,"Method 2:\n");

            pthread_t threads[rows_a];
            gettimeofday(&start, NULL);
            //Multiplication logic
            for(int i = 0; i < rows_a; i++){
                if(pthread_create(&threads[i], NULL, thread_per_row, (void*) i)){
                    printf("Error creating thread\n");
                }
            }
            for(int i = 0; i < rows_a; i++){//wait till all threads finish
                pthread_join(threads[i], NULL);
            }

            gettimeofday(&stop, NULL);
            print_mat(c_file, rows_a, cols_b, c);
            fprintf(stdout, "threads created: %d\n",rows_a);
            fprintf(stdout, "Seconds taken: %lu\n", stop.tv_sec-start.tv_sec);
            fprintf(stdout, "Microseconds taken: %lu\n", stop.tv_usec-start.tv_usec);

            memset(c, 0, rows_a*cols_b*sizeof(int)); // clear matrix

            fprintf(c_file, "Method 3\n");
            fprintf(stdout,"Method 3:\n");
            pthread_t threads_mat[rows_a][cols_b];
            element_info (*inf)[rows_a][cols_b] = malloc(sizeof(element_info[rows_a][cols_b]));//this saves
            //some memory in comparison with allocating each single element
            gettimeofday(&start, NULL);
            //Multiplication Logic
            for(int i = 0; i < rows_a; i++){
                for(int j = 0; j < cols_b; j++){
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
            fprintf(stdout, "threads created: %d\n",rows_a*cols_b);
            fprintf(stdout, "Seconds taken: %lu\n", stop.tv_sec-start.tv_sec);
            fprintf(stdout, "Microseconds taken: %lu\n", stop.tv_usec-start.tv_usec);
        }
    }


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
            fprintf(file, "%7d\t",mat[i][j]);//pretty print
        }
        fprintf(file,"\n");
    }
}


void *thread_per_mat(){
    //normal sequential matrix multiplication
    //cast each matrix (a, b or c) to pointer to 1D array of KNOWN size
    //so that it can be accessed normally
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
    //cast matrices as explained in thread_per_mat
    int res, idx = (int)row_index, (*mat_a)[cols_a], (*mat_b)[cols_b], (*mat_ans)[cols_b];

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
    //cast argument and cast matrices as explained before
    element_info inf = *(element_info*) info;
    int res = 0, (*mat_a)[cols_a], (*mat_b)[cols_b], (*mat_c)[cols_b];
    mat_a = a; mat_b = b; mat_c = c;
    for(int i = 0; i < cols_a; i++){
        res += mat_a[inf.row][i] * mat_b[i][inf.col];
    }
    mat_c[inf.row][inf.col] = res;
    return NULL;
}
