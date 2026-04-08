#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdbool.h>

#define MAX_TASKS 256
#define ID_SIZE 32

typedef struct {
    char id[ID_SIZE];
    int priority;   /* smaller value = higher priority */
    int duration;
    bool used;
} Task;

typedef struct EdgeNode EdgeNode;

typedef struct Scheduler {
    Task tasks[MAX_TASKS];
    int task_count;

    EdgeNode *adj[MAX_TASKS];
    int indegree[MAX_TASKS];

    int hash_keys[521];
    char hash_ids[521][ID_SIZE];
} Scheduler;

void scheduler_init(Scheduler *s);
bool scheduler_add_task(Scheduler *s, const char *id, int priority, int duration);
bool scheduler_add_dependency(Scheduler *s, const char *from_id, const char *to_id);
bool scheduler_run(Scheduler *s);
void scheduler_print(Scheduler *s);

#endif
