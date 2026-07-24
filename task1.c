#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#define THREADS 3
#define TURNS 2

int balance = 1000;
int current = 0;

pthread_mutex_t lock;

void *customer(void *arg)
{
    int id = *(int *)arg;

    for(int i = 0; i < TURNS; i++)
    {
        // Round Robin Scheduler
        while(current != id)
            usleep(1000);

        pthread_mutex_lock(&lock);

        printf("Customer %d is using the ATM\n", id + 1);

        balance -= 100;

        printf("Customer %d withdrew $100\n", id + 1);
        printf("Remaining Balance = $%d\n\n", balance);

        sleep(1);

        pthread_mutex_unlock(&lock);

        // Next customer's turn
        current = (current + 1) % THREADS;
    }

    return NULL;
}

int main()
{
    pthread_t t[THREADS];
    int id[THREADS];

    pthread_mutex_init(&lock, NULL);

    printf("=== Bank ATM Round Robin Simulation ===\n\n");

    for(int i = 0; i < THREADS; i++)
    {
        id[i] = i;
        pthread_create(&t[i], NULL, customer, &id[i]);
    }

    for(int i = 0; i < THREADS; i++)
        pthread_join(t[i], NULL);

    pthread_mutex_destroy(&lock);

    printf("Final Balance = $%d\n", balance);

    return 0;
}
