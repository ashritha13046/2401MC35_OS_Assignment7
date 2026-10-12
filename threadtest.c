#include "types.h"
#include "stat.h"
#include "user.h"

#define PGSIZE 4096

volatile int global_counter = 0;

void worker(void *arg) {
    int i;
    for (i = 0; i < 100000; i++) {
        global_counter++;
    }
    exit();
}

int main(int argc, char *argv[]) {
    void *stack1 = malloc(PGSIZE);
    void *stack2 = malloc(PGSIZE);
    void *stack3 = malloc(PGSIZE);

    printf(1, "Starting Q1 Thread Test (Race Condition expected without synchronization)...\n");

    clone(worker, 0, stack1);
    clone(worker, 0, stack2);
    clone(worker, 0, stack3);

    join();
    join();
    join();

    printf(1, "Final counter value (Expected < 300,000 due to race condition): %d\n", global_counter);
    exit();
}