#include "types.h"
#include "stat.h"
#include "user.h"

#define N 3 // Number of processes
#define M 3 // Number of resource types

int available[M];
int max_matrix[N][M];
int allocation[N][M];
int need[N][M];

// Function to check deadlock status
int detect_deadlock(int deadlocked_pids[]) {
    int work[M];
    int finish[N];
    int i, j, k;

    for (i = 0; i < M; i++)
        work[i] = available[i];

    for (i = 0; i < N; i++)
        finish[i] = 0;

    int found;
    do {
        found = 0;
        for (i = 0; i < N; i++) {
            if (finish[i] == 0) {
                int can_allocate = 1;
                for (j = 0; j < M; j++) {
                    if (need[i][j] > work[j]) {
                        can_allocate = 0;
                        break;
                    }
                }
                if (can_allocate) {
                    for (k = 0; k < M; k++)
                        work[k] += allocation[i][k];
                    finish[i] = 1;
                    found = 1;
                }
            }
        }
    } while (found);

    int deadlock_count = 0;
    for (i = 0; i < N; i++) {
        if (finish[i] == 0) {
            deadlocked_pids[deadlock_count++] = i;
        }
    }
    return deadlock_count;
}

int is_safe_state(int p_idx, int req[]) {
    int i;
    for (i = 0; i < M; i++) {
        if (req[i] > need[p_idx][i] || req[i] > available[i])
            return 0;
    }

    // Temporarily grant request
    for (i = 0; i < M; i++) {
        available[i] -= req[i];
        allocation[p_idx][i] += req[i];
        need[p_idx][i] -= req[i];
    }

    int dummy[N];
    int deadlocked = detect_deadlock(dummy);

    // Rollback changes
    for (i = 0; i < M; i++) {
        available[i] += req[i];
        allocation[p_idx][i] -= req[i];
        need[p_idx][i] += req[i];
    }

    return (deadlocked == 0);
}

int main(int argc, char *argv[]) {
    printf(1, "--- Starting Q3 Banker's Algorithm & Deadlock Detection Test ---\n");

    // Initialize Test Matrix (Safe State Scenario)
    int init_avail[3] = {3, 3, 2};
    int init_max[3][3] = {{7, 5, 3}, {3, 2, 2}, {9, 0, 2}};
    int init_alloc[3][3] = {{0, 1, 0}, {2, 0, 0}, {3, 0, 2}};

    int i, j;
    for (i = 0; i < M; i++) available[i] = init_avail[i];
    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            max_matrix[i][j] = init_max[i][j];
            allocation[i][j] = init_alloc[i][j];
            need[i][j] = max_matrix[i][j] - allocation[i][j];
        }
    }

    int deadlocked_pids[N];
    int count = detect_deadlock(deadlocked_pids);
    if (count == 0) {
        printf(1, "Initial state is Safe (No deadlock detected).\n");
    } else {
        printf(1, "Deadlock detected among processes!\n");
    }

    // Test Banker's Resource Request
    int request[3] = {1, 0, 2};
    printf(1, "Testing resource request for Process 1: [1, 0, 2]...\n");
    if (is_safe_state(1, request)) {
        printf(1, "Request can be safely granted (Banker's Algorithm approved).\n");
    } else {
        printf(1, "Request denied: leads to unsafe state/deadlock.\n");
    }

    exit();
}