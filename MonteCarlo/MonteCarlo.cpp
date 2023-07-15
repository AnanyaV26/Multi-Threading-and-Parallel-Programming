#include <iostream>
#include <cstdlib>
#include <pthread.h>
#include <ctime>
#include <cmath>

using namespace std;

// global variables
int N, K;                    // N -> no. of points and K ->  no. of threads
struct coord *points_square; // storing all points generated in the square

// a struct to store the information of a coordinate
struct coord
{
    double x;
    double y;
    bool inside_circle;
};
// a struct containing the necessary information needed for routine
struct input
{
    pthread_t id;
    int thread_no;
    struct coord *points;
};

// a boolean funtion to check whether the point is in the circle or not
bool check_circle(struct coord point)
{
    return (point.x * point.x + point.y * point.y < 1);
}
// a function which checks all the points contained in a thread
void *routine(void *arg)
{
    struct input *thread;
    thread = (struct input *)arg;
    for (int i = 0; i < N / K; i++)
    {
        thread->points[i].inside_circle = check_circle(thread->points[i]);
    }
}

int main(int argc, char *argv[])
{
    // declaring variables
    int count = 0, time;
    double pi, time_taken;
    struct input *input_to_routine;

    // reading the values from input.txt
    FILE *fp;
    if (argc < 2)
    {
        cout << "Please mention the input file\n";
        return 0;
    }
    fp = fopen(argv[1], "r");
    fscanf(fp, "%d %d", &N, &K);
    fclose(fp);

    // allocating memory
    points_square = (struct coord *)malloc(N * sizeof(struct coord));
    input_to_routine = (struct input *)malloc(K * sizeof(struct input));

    // time start
    clock_t t;
    t = clock();

    // generating the points of the square
    for (int i = 0; i < N; i++)
    {
        points_square[i].x = 2 * (double)rand() / RAND_MAX - 1;
        points_square[i].y = 2 * (double)rand() / RAND_MAX - 1;
    }

    // creating threads and assigning the points corresponding to each thread
    for (int i = 0; i < K; i++)
    {
        input_to_routine[i].thread_no = i;                                                       // giving each input_to_routine it's number
        input_to_routine[i].points = (struct coord *)malloc((N / K + 1) * sizeof(struct coord)); // allocating memory to the array of points
        // assigning the points correspondingly
        for (int j = i * N / K; j < (i + 1) * N / K; j++)
        {
            input_to_routine[i].points[j - i * N / K].x = points_square[j].x;
            input_to_routine[i].points[j - i * N / K].y = points_square[j].y;
        }
        // creating threads
        pthread_create(&input_to_routine[i].id, NULL, &routine, (void *)&input_to_routine[i]);
    }

    // closing threads
    for (int i = 0; i < K; i++)
    {
        pthread_join(input_to_routine[i].id, NULL);
    }

    // counting the number of points in the circle
    for (int i = 0; i < K; i++)
    {
        for (int j = 0; j < N / K; j++)
        {
            if (input_to_routine[i].points[j].inside_circle)
            {
                count++;
            }
        }
    }

    // the value of pi
    pi = 4 * ((double)count / N);

    // time stop
    t = clock() - t;
    time_taken = (double)t / CLOCKS_PER_SEC;
    time = (time_taken * pow(10, 6));
    // print
    char file[100];
    FILE *filePointer;
    filePointer = fopen("output.txt", "w");
    fprintf(filePointer, "Time: %d us\n\n", time);
    fprintf(filePointer, "Value Computed: %lf \n\n", pi);
    fprintf(filePointer, "Log: \n\n");
    for (int i = 0; i < K; i++)
    {
        count = 0;
        for (int j = 0; j < N / K; j++)
        {
            if (input_to_routine[i].points[j].inside_circle)
            {
                count++;
            }
        }
        fprintf(filePointer, "Thread%d: Number of points inside square = %d , Number of points inside the circle = %d \n", i + 1, N / K, count);
        fprintf(filePointer, "Points inside the square :");
        for (int j = i * N / K; j < (i + 1) * N / K; j++)
        {
            fprintf(filePointer, "(%lf,%lf) ", points_square[j].x, points_square[j].y);
        }
        fprintf(filePointer, "\nPoints inside the circle :");
        for (int j = 0; j < N / K; j++)
        {
            if (input_to_routine[i].points[j].inside_circle)
            {
                fprintf(filePointer, "(%lf,%lf) ", input_to_routine[i].points[j].x, input_to_routine[i].points[j].y);
            }
        }
        fprintf(filePointer, "\n\n");
    }

    // closing the file
    fclose(filePointer);

    // freeing the allocated memory
    for (int i = 0; i < K; i++)
    {
        free(input_to_routine[i].points);
    }
    free(points_square);
    free(input_to_routine);
    return 0;
}