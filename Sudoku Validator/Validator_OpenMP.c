#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

// global variables
int N, K;
int **matrix;
FILE *filePointer;

int main(int argc, char *argv[])
{

    int time_taken, grid_no, check = 0;

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
    for (int i = 0; i < N; i++)
    {
        matrix[i] = (int *)malloc(N * sizeof(int));
    }
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            fscanf(fp, "%d ", &matrix[i][j]);
        }
    }
    fclose(fp);

    // opening an output file to write
    char file[100];
    filePointer = fopen("output.txt", "w");

    // time start
    clock_t t;
    t = clock();

    // code to execute in parallel
#pragma omp parallel
    {

        int x = omp_get_thread_num();
        // column checking
#pragma omp for
        for (int j = 0; j < N; j++)
        {
            int error = 0;
            int *arr = (int *)calloc(N, sizeof(int));
            for (int i = 0; i < N; i++)
            {
                int index = matrix[i][j];
                if (arr[index - 1] == 1)
                {
                    // printf("%d %d %d \n", i, j, matrix[i][j]);
                    fprintf(filePointer, "Thread %d checks col %d and is invalid\n", x + 1, j + 1);
                    error++;
                    check++;
                    break;
                }
                else
                {
                    arr[index - 1] = 1;
                }
            }
            if (error == 0)
            {
                fprintf(filePointer, "Thread %d checks col %d and is valid\n", x + 1, j + 1);
            }
            free(arr);
        }
        // row checking
#pragma omp for
        for (int i = 0; i < N; i++)
        {
            int error = 0;
            int *rr = (int *)calloc(N, sizeof(int));
            for (int j = 0; j < N; j++)
            {
                int index = matrix[i][j];
                if (rr[index - 1] == 1)
                {
                    // printf("%d %d %d \n", i, j, matrix[i][j]);
                    fprintf(filePointer, "Thread %d checks row %d and is invalid\n", x + 1, i + 1);
                    error++;
                    check++;
                    break;
                }
                else
                {
                    rr[index - 1] = 1;
                }
            }
            if (error == 0)
            {
                fprintf(filePointer, "Thread %d checks row %d and is valid\n", x + 1, i + 1);
            }
            free(rr);
        }
        // grid checking
#pragma omp for
        for (grid_no = 0; grid_no < N; grid_no++)
        {
            int error = 0, index;
            int n = sqrt(N);
            int f = grid_no / n;
            int *arr = (int *)calloc(N, sizeof(int));
            if (grid_no % 3 == 0)
            {
                for (int i = 0; i < n; i++)
                {
                    for (int j = 0 + f * n; j < n + f * n; j++)
                    {
                        index = matrix[i][j];
                        if (arr[index - 1] == 1)
                        {
                            error++;
                            check++;
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
                for (int i = n; i < 2 * n; i++)
                {
                    for (int j = 0 + f * n; j < n + f * n; j++)
                    {
                        index = matrix[i][j];
                        if (arr[index - 1] == 1)
                        {

                            error++;
                            check++;
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
                for (int i = 2 * n; i < 3 * n; i++)
                {
                    for (int j = 0 + f * n; j < n + f * n; j++)
                    {
                        index = matrix[i][j];
                        if (arr[index - 1] == 1)
                        {

                            error++;
                            check++;
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
                fprintf(filePointer, "Thread %d checks grid %d and is valid\n", x + 1, grid_no + 1);
            }
            else
            {
                fprintf(filePointer, "Thread %d checks grid %d and is invalid\n", x + 1, grid_no + 1);
            }

            free(arr);
        }
    }
    // time stop
    t = clock() - t;
    timetaken = (double)t / CLOCKS_PER_SEC;
    time_taken = timetaken * pow(10, 6);

    if (check == 0)
    {
        fprintf(filePointer, "Sudoku is valid\n");
    }
    else
    {
        fprintf(filePointer, "Sudoku is invalid\n");
    }

    fprintf(filePointer, "The total time taken is %d microseconds\n", time_taken);
    // close the file
    fclose(filePointer);
    return 0;
}