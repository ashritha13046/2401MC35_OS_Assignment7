#include "types.h"
#include "stat.h"
#include "user.h"

int main(int argc, char *argv[]) {
    printf(1, "--- Starting Q4 Priority-Based Preemptive Scheduling Test ---\n");

    int pids[4];
    int priorities[4] = {2, 8, 15, 20}; // P3 is lowest priority (20), prone to starvation without aging
    int i;

    for (i = 0; i < 4; i++) {
        int pid = fork();
        if (pid < 0) {
            printf(1, "Fork failed\n");
            exit();
        }
        if (pid == 0) {
            // Child process
            int my_pid = getpid();
            // Assuming system call set_priority(pid, priority) is implemented
            set_priority(my_pid, priorities[i]);
            
            // CPU-bound loop workload
            int volatile val = 0;
            long long j;
            for (j = 0; j < 400000000LL; j++) {
                val += j;
            }
            printf(1, "Child Process PID %d with initial priority %d finished execution.\n", my_pid, priorities[i]);
            exit();
        } else {
            pids[i] = pid;
        }
    }

    // Parent waits for all children to complete
    for (i = 0; i < 4; i++) {
        wait();
    }

    printf(1, "All processes completed. Notice higher priority processes finish earlier, and aging prevents low priority starvation.\n");
    exit();
}