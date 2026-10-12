#include "types.h"
#include "stat.h"
#include "user.h"

#define PGSIZE 4096

volatile int global_counter = 0;
int mutex_sem = 0;

// Q2 Part (d): Counter synchronization test with semaphores
void sync_worker(void *arg) {
    int i;
    for (i = 0; i < 100000; i++) {
        sem_wait(mutex_sem);
        global_counter++;
        sem_signal(mutex_sem);
    }
    exit();
}

// Q2 Part (e): Producer-Consumer problem
#define BUFFER_SIZE 5
int buffer[BUFFER_SIZE];
int in = 0;
int out = 0;

int empty_sem = 1;
int full_sem = 2;
int prod_mutex = 3;

void producer(void *arg) {
    int i;
    for (i = 1; i <= 10; i++) {
        sem_wait(empty_sem);
        sem_wait(prod_mutex);
        
        buffer[in] = i;
        printf(1, "Produced item %d at index %d\n", i, in);
        in = (in + 1) % BUFFER_SIZE;
        
        sem_signal(prod_mutex);
        sem_signal(full_sem);
    }
    exit();
}

void consumer(void *arg) {
    int i, item;
    for (i = 1; i <= 10; i++) {
        sem_wait(full_sem);
        sem_wait(prod_mutex);
        
        item = buffer[out];
        printf(1, "Consumed item %d from index %d\n", item, out);
        out = (out + 1) % BUFFER_SIZE;
        
        sem_signal(prod_mutex);
        sem_signal(empty_sem);
    }
    exit();
}

int main(int argc, char *argv[]) {
    printf(1, "--- Running Q2 Part (d): Synchronized Counter Test ---\n");
    sem_init(mutex_sem, 1);
    global_counter = 0;

    void *s1 = malloc(PGSIZE);
    void *s2 = malloc(PGSIZE);
    void *s3 = malloc(PGSIZE);

    clone(sync_worker, 0, s1);
    clone(sync_worker, 0, s2);
    clone(sync_worker, 0, s3);

    join();
    join();
    join();

    printf(1, "Final Synchronized Counter Value (Expected 300,000): %d\n\n", global_counter);

    printf(1, "--- Running Q2 Part (e): Producer-Consumer Test ---\n");
    sem_init(empty_sem, BUFFER_SIZE);
    sem_init(full_sem, 0);
    sem_init(prod_mutex, 1);

    void *p_stack = malloc(PGSIZE);
    void *c_stack = malloc(PGSIZE);

    clone(producer, 0, p_stack);
    clone(consumer, 0, c_stack);

    join();
    join();

    printf(1, "Producer-Consumer Test Completed Successfully.\n");
    exit();
}