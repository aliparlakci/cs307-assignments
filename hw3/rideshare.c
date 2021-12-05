#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/types.h>

typedef struct semaphore
{
    int val;
    pthread_mutex_t lock;
    pthread_cond_t c;
} sem_t;

void sem_init(sem_t *s, int val)
{
    pthread_mutex_init(&s->lock, NULL);
    pthread_cond_init(&s->c, NULL);
    s->val = val;
}

void sem_wait(sem_t *s)
{
    // decrement the value of s by one
    // wait if the value of s is negative
    pthread_mutex_lock(&s->lock);
    s->val--;
    if (s->val < 0)
    {
        pthread_cond_wait(&s->c, &s->lock);
    }
    pthread_mutex_unlock(&s->lock);
}

void sem_post(sem_t *s)
{
    // increment the value of s by one
    // wake a sleeping thread if exists
    pthread_mutex_lock(&s->lock);
    s->val++;
    pthread_cond_signal(&s->c);
    pthread_mutex_unlock(&s->lock);
}

void *fanA(void *args);
void *fanB(void *args);
int isEven(int x);
int divisibleByFour(int x);

pthread_mutex_t lock;
sem_t semA;
sem_t semB;

pthread_t *threads;

pthread_barrier_t barrier;

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("The main terminates\n");
        return 0;
    }

    int teamA_fans = atoi(argv[1]);
    int teamB_fans = atoi(argv[2]);
    int total_fans = teamA_fans + teamB_fans;

    if (!(isEven(teamA_fans) && isEven(teamB_fans) && divisibleByFour(total_fans)))
    {
        printf("The main terminates\n");
        return 0;
    }

    sem_init(&semA, 0);
    sem_init(&semB, 0);

    pthread_barrier_init(&barrier, NULL, total_fans);

    threads = malloc(sizeof(pthread_t) * total_fans);
    for (int i = 0; i < teamA_fans; i++)
    {
        pthread_create(&threads[i], NULL, &fanA, NULL);
    }
    for (int i = 0; i < teamB_fans; i++)
    {
        pthread_create(&threads[teamA_fans+i], NULL, &fanB, NULL);
    }

    for (int i = 0; i < total_fans; i++)
    {
        pthread_join(threads[i], NULL);
    }
    printf("The main terminates\n");

    return 0;
}

void *fanA(void *args)
{
    int is_captain = 0;

    printf("Thread ID: %d, Team: A, I am looking for a car\n", gettid());

    pthread_mutex_lock(&lock);
    if (semA.val <= -3)
    {
        sem_post(&semA);
        sem_post(&semA);
        sem_post(&semA);
        is_captain = 1;
        pthread_mutex_unlock(&lock);
    }
    else if (semA.val <= -1 && semB.val <= -2)
    {
        sem_post(&semA);
        sem_post(&semB);
        sem_post(&semB);
        is_captain = 1;
        pthread_mutex_unlock(&lock);
    }
    else
    {
        pthread_mutex_unlock(&lock);
        sem_wait(&semA);
    }

    printf("Thread ID: %d, Team: A, I have found a spot in a car\n", gettid());
    pthread_barrier_wait(&barrier);

    if (is_captain)
    {
        printf("Thread ID: %d, Team: A, I am the captain and driving the car\n", gettid());
    }
}

void *fanB(void *args)
{
    int is_captain = 0;

    printf("Thread ID: %d, Team: B, I am looking for a car\n", gettid());

    pthread_mutex_lock(&lock);
    if (semB.val <= -3)
    {
        sem_post(&semB);
        sem_post(&semB);
        sem_post(&semB);
        is_captain = 1;
        pthread_mutex_unlock(&lock);
    }
    else if (semB.val <= -1 && semA.val <= -2)
    {
        sem_post(&semB);
        sem_post(&semA);
        sem_post(&semA);
        is_captain = 1;
        pthread_mutex_unlock(&lock);
    }
    else
    {
        pthread_mutex_unlock(&lock);
        sem_wait(&semB);
    }

    printf("Thread ID: %d, Team: B, I have found a spot in a car\n", gettid());
    pthread_barrier_wait(&barrier);

    if (is_captain)
    {
        printf("Thread ID: %d, Team: B, I am the captain and driving the car\n", gettid());
    }
}

int isEven(int x)
{
    return !(x % 2);
}

int divisibleByFour(int x)
{
    int mod = x % 4;
    if (mod == 0)
        return 1;
    else
        return 0;
}