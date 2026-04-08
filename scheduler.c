#include "scheduler.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TABLE_SIZE 521
#define STACK_SIZE MAX_TASKS

struct EdgeNode {
    int to;
    struct EdgeNode *next;
};

typedef struct {
    int buf[MAX_TASKS];
    int head;
    int tail;
} IntQueue;

typedef struct {
    int value[MAX_TASKS];
    int size;
    Task *tasks;
} MinHeap;

typedef struct {
    int value[STACK_SIZE];
    int top;
} IntStack;

static unsigned int hash_str(const char *s) {
    unsigned int h = 5381U;
    while (*s) {
        h = ((h << 5) + h) + (unsigned char)(*s++);
    }
    return h;
}

static void map_init(Scheduler *s) {
    int i;
    for (i = 0; i < TABLE_SIZE; i++) {
        s->hash_keys[i] = -1;
        s->hash_ids[i][0] = '\0';
    }
}

static bool map_put(Scheduler *s, const char *id, int idx) {
    unsigned int base = hash_str(id) % TABLE_SIZE;
    unsigned int i;
    for (i = 0; i < TABLE_SIZE; i++) {
        unsigned int pos = (base + i * i) % TABLE_SIZE;
        if (s->hash_keys[pos] == -1) {
            s->hash_keys[pos] = idx;
            strncpy(s->hash_ids[pos], id, ID_SIZE - 1);
            s->hash_ids[pos][ID_SIZE - 1] = '\0';
            return true;
        }
        if (strcmp(s->hash_ids[pos], id) == 0) {
            return false;
        }
    }
    return false;
}

static int map_get(const Scheduler *s, const char *id) {
    unsigned int base = hash_str(id) % TABLE_SIZE;
    unsigned int i;
    for (i = 0; i < TABLE_SIZE; i++) {
        unsigned int pos = (base + i * i) % TABLE_SIZE;
        if (s->hash_keys[pos] == -1) {
            return -1;
        }
        if (strcmp(s->hash_ids[pos], id) == 0) {
            return s->hash_keys[pos];
        }
    }
    return -1;
}

static void queue_init(IntQueue *q) {
    q->head = 0;
    q->tail = 0;
}

static bool queue_empty(const IntQueue *q) {
    return q->head == q->tail;
}

static bool queue_push(IntQueue *q, int v) {
    int nxt = (q->tail + 1) % MAX_TASKS;
    if (nxt == q->head) {
        return false;
    }
    q->buf[q->tail] = v;
    q->tail = nxt;
    return true;
}

static bool queue_pop(IntQueue *q, int *out) {
    if (queue_empty(q)) {
        return false;
    }
    *out = q->buf[q->head];
    q->head = (q->head + 1) % MAX_TASKS;
    return true;
}

static int heap_cmp(const MinHeap *h, int a, int b) {
    const Task *ta = &h->tasks[a];
    const Task *tb = &h->tasks[b];
    if (ta->priority != tb->priority) {
        return ta->priority - tb->priority;
    }
    return strcmp(ta->id, tb->id);
}

static void heap_swap(int *a, int *b) {
    int t = *a;
    *a = *b;
    *b = t;
}

static void heap_init(MinHeap *h, Task *tasks) {
    h->size = 0;
    h->tasks = tasks;
}

static bool heap_push(MinHeap *h, int idx) {
    int i = h->size;
    if (i >= MAX_TASKS) {
        return false;
    }
    h->value[h->size++] = idx;
    while (i > 0) {
        int p = (i - 1) / 2;
        if (heap_cmp(h, h->value[p], h->value[i]) <= 0) {
            break;
        }
        heap_swap(&h->value[p], &h->value[i]);
        i = p;
    }
    return true;
}

static bool heap_pop(MinHeap *h, int *out) {
    int i = 0;
    if (h->size == 0) {
        return false;
    }
    *out = h->value[0];
    h->size--;
    h->value[0] = h->value[h->size];
    while (1) {
        int l = i * 2 + 1;
        int r = l + 1;
        int m = i;
        if (l < h->size && heap_cmp(h, h->value[l], h->value[m]) < 0) {
            m = l;
        }
        if (r < h->size && heap_cmp(h, h->value[r], h->value[m]) < 0) {
            m = r;
        }
        if (m == i) {
            break;
        }
        heap_swap(&h->value[m], &h->value[i]);
        i = m;
    }
    return true;
}

static void stack_init(IntStack *s) {
    s->top = -1;
}

static bool stack_push(IntStack *s, int x) {
    if (s->top + 1 >= STACK_SIZE) {
        return false;
    }
    s->value[++s->top] = x;
    return true;
}

static bool stack_pop(IntStack *s, int *out) {
    if (s->top < 0) {
        return false;
    }
    *out = s->value[s->top--];
    return true;
}

void scheduler_init(Scheduler *s) {
    int i;
    s->task_count = 0;
    for (i = 0; i < MAX_TASKS; i++) {
        s->tasks[i].used = false;
        s->adj[i] = NULL;
        s->indegree[i] = 0;
    }
    map_init(s);
}

bool scheduler_add_task(Scheduler *s, const char *id, int priority, int duration) {
    int idx;
    if (!id || priority < 0 || duration <= 0) {
        return false;
    }
    if (s->task_count >= MAX_TASKS) {
        return false;
    }
    if (map_get(s, id) != -1) {
        return false;
    }

    idx = s->task_count++;
    strncpy(s->tasks[idx].id, id, ID_SIZE - 1);
    s->tasks[idx].id[ID_SIZE - 1] = '\0';
    s->tasks[idx].priority = priority;
    s->tasks[idx].duration = duration;
    s->tasks[idx].used = true;
    if (!map_put(s, id, idx)) {
        return false;
    }
    return true;
}

bool scheduler_add_dependency(Scheduler *s, const char *from_id, const char *to_id) {
    int from = map_get(s, from_id);
    int to = map_get(s, to_id);
    struct EdgeNode *node;
    if (from < 0 || to < 0 || from == to) {
        return false;
    }
    node = (struct EdgeNode *)malloc(sizeof(struct EdgeNode));
    if (!node) {
        return false;
    }
    node->to = to;
    node->next = s->adj[from];
    s->adj[from] = node;
    s->indegree[to]++;
    return true;
}

bool scheduler_run(Scheduler *s) {
    int indeg[MAX_TASKS];
    IntQueue q;
    MinHeap heap;
    IntStack done;
    int i, count = 0, now = 0, idx;

    for (i = 0; i < s->task_count; i++) {
        indeg[i] = s->indegree[i];
    }

    queue_init(&q);
    for (i = 0; i < s->task_count; i++) {
        if (indeg[i] == 0) {
            queue_push(&q, i);
        }
    }

    heap_init(&heap, s->tasks);
    while (!queue_empty(&q)) {
        int v;
        queue_pop(&q, &v);
        heap_push(&heap, v);
    }

    stack_init(&done);

    puts("\n=== Schedule Start ===");
    while (heap_pop(&heap, &idx)) {
        struct EdgeNode *p = s->adj[idx];
        printf("[t=%d] run %-12s priority=%d duration=%d\n",
               now, s->tasks[idx].id, s->tasks[idx].priority, s->tasks[idx].duration);
        now += s->tasks[idx].duration;
        count++;
        stack_push(&done, idx);

        while (p) {
            indeg[p->to]--;
            if (indeg[p->to] == 0) {
                heap_push(&heap, p->to);
            }
            p = p->next;
        }
    }

    if (count != s->task_count) {
        puts("检测到依赖环，无法完成拓扑调度。");
        return false;
    }

    puts("\n=== Reverse Completion (Stack) ===");
    while (stack_pop(&done, &idx)) {
        printf("%s ", s->tasks[idx].id);
    }
    puts("\n=== Schedule End ===");
    return true;
}

void scheduler_print(Scheduler *s) {
    int i;
    puts("\nTasks:");
    for (i = 0; i < s->task_count; i++) {
        printf("- %-12s priority=%d duration=%d indegree=%d\n",
               s->tasks[i].id, s->tasks[i].priority, s->tasks[i].duration, s->indegree[i]);
    }
}
