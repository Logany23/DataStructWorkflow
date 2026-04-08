#include "scheduler.h"

#include <stdio.h>
#include <string.h>

static void print_help(void) {
    puts("Commands:");
    puts("  add_task <id> <priority> <duration>");
    puts("  add_dep  <from_id> <to_id>");
    puts("  print");
    puts("  run");
    puts("  demo");
    puts("  help");
    puts("  exit");
}

static void load_demo(Scheduler *s) {
    scheduler_add_task(s, "Design", 1, 3);
    scheduler_add_task(s, "Parser", 2, 4);
    scheduler_add_task(s, "Heap", 2, 2);
    scheduler_add_task(s, "Graph", 1, 4);
    scheduler_add_task(s, "Test", 0, 2);

    scheduler_add_dependency(s, "Design", "Parser");
    scheduler_add_dependency(s, "Design", "Graph");
    scheduler_add_dependency(s, "Parser", "Test");
    scheduler_add_dependency(s, "Heap", "Test");
    scheduler_add_dependency(s, "Graph", "Test");
}

int main(void) {
    Scheduler scheduler;
    char cmd[64];

    scheduler_init(&scheduler);
    puts("DataStructWorkflow - C项目（图 + 哈希表 + 队列 + 最小堆 + 栈）");
    print_help();

    while (1) {
        printf("\n> ");
        if (scanf("%63s", cmd) != 1) {
            break;
        }

        if (strcmp(cmd, "add_task") == 0) {
            char id[ID_SIZE];
            int pr, dur;
            if (scanf("%31s %d %d", id, &pr, &dur) == 3 && scheduler_add_task(&scheduler, id, pr, dur)) {
                puts("ok");
            } else {
                puts("add_task failed");
            }
        } else if (strcmp(cmd, "add_dep") == 0) {
            char from[ID_SIZE], to[ID_SIZE];
            if (scanf("%31s %31s", from, to) == 2 && scheduler_add_dependency(&scheduler, from, to)) {
                puts("ok");
            } else {
                puts("add_dep failed");
            }
        } else if (strcmp(cmd, "print") == 0) {
            scheduler_print(&scheduler);
        } else if (strcmp(cmd, "run") == 0) {
            scheduler_run(&scheduler);
        } else if (strcmp(cmd, "demo") == 0) {
            scheduler_init(&scheduler);
            load_demo(&scheduler);
            puts("demo loaded");
        } else if (strcmp(cmd, "help") == 0) {
            print_help();
        } else if (strcmp(cmd, "exit") == 0) {
            break;
        } else {
            puts("unknown command");
        }
    }

    return 0;
}
