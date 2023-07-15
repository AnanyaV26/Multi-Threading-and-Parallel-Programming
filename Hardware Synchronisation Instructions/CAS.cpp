#include <iostream>
#include <pthread.h>
#include <ctime>
#include <chrono>
#include <atomic>
#include <random>
#include <unistd.h>
#include <cstdlib>
#include <cstring>

using namespace std;
atomic<bool> lock(0);
FILE *filePointer;
int l1, l2, k;
double wait_sec, max_time;
time_t t0;

void *routine(void *arg)
{
    int id = *(int *)arg;
    double t1, t2;
    int i;
    char *entrytime, *acttime, *exittime;
    bool expected = false;
    bool new_value = true;

    default_random_engine generator;
    exponential_distribution<double> distribution1(l1);
    exponential_distribution<double> distribution2(l2);

    t1 = distribution1(generator) / (pow(10, 3));
    t2 = distribution2(generator) / (pow(10, 3));

    while (i < k)
    {

        tm *req_time = localtime(&t0);
        clock_t t;
        t = clock();

        while (lock.compare_exchange_weak(expected, new_value) != false)
            ;

        fprintf(filePointer, "%dth CS Requested at %d:%d:%d by thread %d \n", i + 1, req_time->tm_hour, req_time->tm_min, req_time->tm_sec, id);

        tm *entry_time = localtime(&t0);
        fprintf(filePointer, "%dth CS Entered at %d:%d:%d by thread %d \n", i + 1, entry_time->tm_hour, entry_time->tm_min, entry_time->tm_sec, id);
        t = clock() - t;

        double time = ((double)t / CLOCKS_PER_SEC);
        if (max_time < time)
        {
            max_time = time;
        }
        wait_sec = wait_sec + time;

        sleep(t1);

        tm *exit_time = localtime(&t0);
        fprintf(filePointer, "%dth CS Exited at %d:%d:%d by thread %d \n", i + 1, exit_time->tm_hour, exit_time->tm_min, exit_time->tm_sec, id);

        i++;
        lock = false;

        sleep(t2);
    }
}

int main(int argc, char *argv[])
{
    int n;

    FILE *fp;
    if (argc < 2)
    {
        printf("Please mention the input file\n");
        return 0;
    }
    fp = fopen(argv[1], "r");
    fscanf(fp, "%d %d %d %d", &n, &k, &l1, &l2);
    fclose(fp);

    float wait_in_sec;

    char file[100];
    filePointer = fopen("output_cas.txt", "w");

    pthread_t th[n];
    t0 = time(0);
    for (int i = 0; i < n; i++)
    {
        int *a = (int *)malloc(sizeof(int));
        *a = i + 1;
        pthread_create(&th[i], NULL, &routine, a);
    }
    for (int i = 0; i < n; i++)
    {
        pthread_join(th[i], NULL);
    }

    wait_in_sec = wait_sec / (n * k);
    fprintf(filePointer, "Total waiting time is %f secs \n", wait_sec);
    fprintf(filePointer, "Average waiting time is %f secs \n", wait_in_sec);
    fprintf(filePointer, "The worst case time taken %f secs\n", max_time);
    fclose(filePointer);
    return 0;
}