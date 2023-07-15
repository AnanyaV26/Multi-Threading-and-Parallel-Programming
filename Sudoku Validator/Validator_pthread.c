#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <pthread.h>

// global variables
int N, K, error;
int **matrix;
FILE *filePointer;

// function for checking rows
void row_checker(int thread_no, int row_no)
{
    int i, j, index;
    int arr[N];

    for (j = 0; j < N; j++)
    {
        arr[j] = 0;
    }

    i = row_no;
    for (j = 0; j < N; j++)
    {
        index = matrix[i][j];
        if (arr[index - 1] == 1)
        {
            fprintf(filePointer, "Thread %d checks row %d and is invalid\n", thread_no + 1, row_no + 1);
            error++;
            break;
        }
        else
        {
            arr[index - 1] = 1;
        }
    }
    if (error == 0)
    {
        fprintf(filePointer, "Thread %d checks row %d and is valid\n", thread_no + 1, row_no + 1);
    }
    return;
}

// function for checking columns
void col_checker(int thread_no, int col_no)
{
    int i, j, index;
    int arr[N];

    for (i = 0; i < N; i++)
    {
        arr[i] = 0;
    }

    j = col_no;
    for (i = 0; i < N; i++)
    {
        index = matrix[i][j];
        if (arr[index - 1] == 1)
        {
            fprintf(filePointer, "Thread %d checks col %d and is invalid\n", thread_no + 1, col_no + 1);
            error++;
            break;
        }
        else
        {
            arr[index - 1] = 1;
        }
    }
    if (error == 0)
    {
        fprintf(filePointer, "Thread %d checks col %d and is valid\n", thread_no + 1, col_no + 1);
    }
    return;
}

// function for checking subgrids
void grid_checker(int thread_no, int grid_no)
{
    int n, i, j, f, index;
    n = sqrt(N);
    int arr[N];
    f = grid_no / n;
    error = 0;

    for (i = 0; i < N; i++)
    {
        arr[i] = 0;
    }
    if (grid_no % 3 == 0)
    {
        for (i = 0; i < n; i++)
        {
            for (j = 0 + f * n; j < n + f * n; j++)
            {
                index = matrix[i][j];
                if (arr[index - 1] == 1)
                {
                    error++;
                    break;
                }
                else
                {
                    arr[index - 1] = 1;
                }
            }
        }
    }
    else if (grid_no % 3 == 1)
    {
        for (i = n; i < 2 * n; i++)
        {
            for (j = 0 + f * n; j < n + f * n; j++)
            {
                index = matrix[i][j];
                if (arr[index - 1] == 1)
                {

                    error++;
                    break;
                }
                else
                {
                    arr[index - 1] = 1;
                }
            }
        }
    }
    else
    {
        for (i = 2 * n; i < 3 * n; i++)
        {
            for (j = 0 + f * n; j < n + f * n; j++)
            {
                index = matrix[i][j];
                if (arr[index - 1] == 1)
                {

                    error++;
                    break;
                }
                else
                {
                    arr[index - 1] = 1;
                }
            }
        }
    }

    if (error == 0)
    {
        fprintf(filePointer, "Thread %d checks grid %d and is valid\n", thread_no + 1, grid_no + 1);
    }
    else
    {
        fprintf(filePointer, "Thread %d checks grid %d and is invalid\n", thread_no + 1, grid_no + 1);
    }
    return;
}

// function for a thread to do and check rows, column, grid
void *routine(void *arg)
{
    int i, thread_no, check_no;
    thread_no = *(int *)arg;
    for (i = 0; i < N; i++)
    {
        check_no = thread_no + i * K;
        if (check_no < N)
        {
            row_checker(thread_no, check_no);
            col_checker(thread_no, check_no);
            grid_checker(thread_no, check_no);
        }
    }
}

int main(int argc, char *argv[])
{
    // take input
    int i, j, time_taken;
    double timetaken;
    FILE *fp;
    if (argc < 2)
    {
        printf("Please mention the input file\n");
        return 0;
    }
    fp = fopen(argv[1], "r");
    fscanf(fp, "%d %d", &K, &N);
    matrix = (int **)malloc(N * sizeof(int *));
    for (i = 0; i < N; i++)
    {
        matrix[i] = (int *)malloc(N * sizeof(int));
    }
    for (i = 0; i < N; i++)
    {
        for (j = 0; j < N; j++)
        {
            fscanf(fp, "%d ", &matrix[i][j]);
        }
    }
    fclose(fp);

    // opening an output file to write
    char file[100];
    filePointer = fopen("output.txt", "w");

    // thread part
    int *a = (int *)malloc(K * sizeof(int));

    pthread_t th[K];

    // time start
    clock_t t;
    t = clock();

    for (i = 0; i < K; i++)
    {

        a[i] = i;
        pthread_create(&th[i], NULL, &routine, &a[i]);
    }
    for (i = 0; i < K; i++)
    {
        pthread_join(th[i], NULL);
    }

    // time stop
    t = clock() - t;
    timetaken = (double)t / CLOCKS_PER_SEC;
    time_taken = timetaken * pow(10, 6);

    if (error == 0)
    {
        fprintf(filePointer, "Sudoku is valid\n");
    }
    else
    {
        fprintf(filePointer, "Sudoku is invalid\n");
    }
    fprintf(filePointer, "The total time taken is %d microseconds\n", time_taken);

    // freeing allocated memory and closing file
    free(a);
    fclose(filePointer);

    return 0;
}