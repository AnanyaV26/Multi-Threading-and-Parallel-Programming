#include <iostream>
#include <pthread.h>
#include <ctime>
#include <chrono>
#include <atomic>
#include <random>
#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <math.h>
#include <semaphore.h>

using namespace std;

// global variables defined.
int p, c, lp, lc, k, check, check1;
sem_t *S;
sem_t *mutex;
clock_t t;
FILE *filePointer;
int *car_ride;
bool done;
double timetaken_ta, timetaken_tb, timetaken_tc, timetaken_td, timetaken_te, t1, t2;
int time_taken_p, time_taken_c;
double time_passengers, time_cars;

// ride function which defines ride procedure for each car thread.
void *ride(void *arg)
{
    int id = *(int *)arg;
    double t2;
    default_random_engine generator2;
    exponential_distribution<double> distribution2(pow(10, 3) / lc);
    t2 = distribution2(generator2);
    sem_post(mutex); // sends signal to other threads about the availability of car.
    while (done != true)
    {
        if (car_ride[id] != 0)
        {
            sleep(t2);
            car_ride[id] = 0;
        }
    }
    sem_wait(mutex); // makes the threads to wait to access the available car correspondingly.
    return NULL;
}

// routine function defines the procedure for each passenger thread.
void *routine(void *arg)
{
    int id = *(int *)arg;
    int i = 0, j;
    done = false;

    default_random_engine generator1;
    exponential_distribution<double> distribution1(pow(10, 3) / lp);
    t1 = distribution1(generator1);

    clock_t ta = clock() - t;
    timetaken_ta = ((double)ta / CLOCKS_PER_SEC) * pow(10, 6);
    fprintf(filePointer, "Passenger %d enters the museum after %lf us \n", id, timetaken_ta);
    for (i = 0; i < k; i++)
    {
        clock_t tb = clock() - t;
        timetaken_tb = ((double)tb / CLOCKS_PER_SEC) * pow(10, 6);
        fprintf(filePointer, "Passenger %d made request number %d after %lf us \n", id, i + 1, timetaken_tb);
        sem_wait(S);

        for (check = 0; check <= k * c * 2; check++)
        {
            j = check1 % c;
            sem_wait(mutex);
            if (car_ride[j] == 0)
            {
                car_ride[j] = 1;
                clock_t tc = clock() - t;
                timetaken_tc = ((double)tc / CLOCKS_PER_SEC) * pow(10, 6);
            }
            sem_post(mutex);

            fprintf(filePointer, "Car %d accepts passenger %d request number %d after time %lf us\n", j + 1, id, i + 1, timetaken_tc);

            while (car_ride != 0)
            {
                sleep(t1);
                break;
            }
            clock_t td = clock() - t;
            timetaken_td = ((double)td / CLOCKS_PER_SEC) * pow(10, 6);
            fprintf(filePointer, "Car %d finished passenger %d ride number %d after time %lf us\n", j + 1, id, i + 1, timetaken_td);
            check1++;
            break;
        }

        sleep(t1);
        sem_post(S);

        time_taken_c = (timetaken_td - timetaken_tc) + time_taken_c; // ride duration = finish time - request accept time.
    }
    clock_t te = clock() - t;
    timetaken_te = ((double)te / CLOCKS_PER_SEC) * pow(10, 6);
    fprintf(filePointer, "Passenger %d exits the museum afte time %lf us\n", id, timetaken_te);
    time_taken_p = (timetaken_te - timetaken_ta) + time_taken_p; // tour duration = exit time - entry time.

    return NULL;
}

int main(int argc, char *argv[])
{
    // taking input.
    FILE *fp;
    if (argc < 2)
    {
        printf("Please mention the input file\n");
        return 0;
    }
    fp = fopen(argv[1], "r");
    fscanf(fp, "%d %d %d %d %d", &p, &c, &lp, &lc, &k);
    fclose(fp);

    pthread_t passengers[p];
    pthread_t cars[c];

    // allocating memory
    S = (sem_t *)malloc(sizeof(sem_t));
    int n1 = sem_init(S, 0, c);

    mutex = (sem_t *)malloc(sizeof(sem_t));
    int n2 = sem_init(mutex, 0, 1);

    car_ride = (int *)malloc(c * sizeof(int));
    for (int i = 0; i < c; i++)
    {
        car_ride[i] = 0;
    }

    char file[100];
    filePointer = fopen("output.txt", "w");

    t = clock();
    // creation of threads
    for (int i = 0; i < p; i++)
    {
        int *a = (int *)malloc(sizeof(int));
        *a = i + 1;
        pthread_create(&passengers[i], NULL, &routine, a);
    }

    for (int i = 0; i < c; i++)
    {
        int *b = (int *)malloc(sizeof(int));
        *b = i + 1;
        pthread_create(&cars[i], NULL, &ride, b);
    }
    // joining of threads
    for (int i = 0; i < p; i++)
    {

        pthread_join(passengers[i], NULL);
    }
    time_passengers = (double)time_taken_p / p;
    fprintf(filePointer, "Average time taken by the passengers is %lf us\n", time_passengers);

    done = true; // work done!

    for (int i = 0; i < c; i++)
    {
        pthread_join(cars[i], NULL);
    }
    time_cars = (double)time_taken_c / (p * k);
    fprintf(filePointer, "Average time taken by the cars is %lf us\n", time_cars);

    // freeing allocated memory
    free(S);
    free(mutex);
    free(car_ride);
    fclose(filePointer);
    return 0;
}
