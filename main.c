#include <stdio.h>
#include "threads.h"

void f3(void *arg) {
    for(int i = 0; i < 10; i ++) {
        printf("thread 3: %d\n", i++);
        thread_yield();
    }
}

void f2(void *arg) {
    for(int i = 0; i < 10; i ++) {
        printf("thread 2: %d\n", i++);
        if (i == 10) {
            i = 0;
        }
        thread_yield();
    }
}

void f1(void *arg) {
    struct thread *t2 = thread_create(f2, NULL);
    thread_add_runqueue(t2);
    struct thread *t3 = thread_create(f3, NULL);
    thread_add_runqueue(t3);
    for(int i = 0; i < 100; i ++) {
        printf("thread 1: %d\n", i++);
        thread_yield();
    }
}

int simple_test_f1(void) {
    printf("start from thread.\n");
    int i = 0;
    while (i < 1) {
        printf("hello from thread. %d\n", i);
        thread_yield();
        i++;
    }
    return 0;
}

void simple_test() {
    struct thread *t1 = thread_create(simple_test_f1, NULL);
    thread_add_runqueue(t1);
    struct thread *t2 = thread_create(simple_test_f1, NULL);
    thread_add_runqueue(t2);
    thread_start_threading();
    printf("\nexited\n");
}

void simple_test2() {
    struct thread *t1 = thread_create(f1, NULL);
    thread_add_runqueue(t1);
    thread_start_threading();
    printf("\nexited\n");
}

int main(int argc, char **argv) {
    simple_test2();
    return 0;
}
