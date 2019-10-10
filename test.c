#include <stdio.h>
#include "thread.h"

int tests_run = 0;
#define start_test fprintf(stdout, "running %s \n",  __FUNCTION__)
#define test_assert(message, test) do { if (!(test)) return message; } while (0)
#define test_run(test) do { char *message = test(); tests_run++; \
                                if (message) return message; } while (0)

// -------------------------------
// test_single_thread
// -------------------------------
static int test_single_thread_res = 0;
static void test_single_thread_f1(void *args);

static char *test_single_thread() {
    start_test;
    struct thread *t1 = thread_create(test_single_thread_f1, NULL);
    thread_add_runqueue(t1);
    thread_start_threading();
    printf("%d", test_single_thread_res);
    test_assert("error, test_single_thread_res  != 100", test_single_thread_res == 100);
    return 0;
}

static void test_single_thread_f1(void *args) {
    int i = 0;
    while (i < 100) {
        thread_yield();
        i++;
    }
    test_single_thread_res = i;
}

// -------------------------------
// test_simple_two_threads
// -------------------------------
static int test_simple_two_threads_res = 0;
static void test_simple_two_threads_f1(void *args);

static char *test_simple_two_threads() {
    struct thread *t1 = thread_create(test_simple_two_threads_f1, NULL);
    struct thread *t2 = thread_create(test_simple_two_threads_f1, NULL);
    thread_add_runqueue(t1);
    thread_add_runqueue(t2);
    thread_start_threading();
    test_assert("error, test_simple_two_threads != 200",
                test_simple_two_threads_res == 200);
    return 0;
}

static void test_simple_two_threads_f1(void *args) {
    int i = 0;
    while (i < 100) {
        thread_yield();
        i++;
    }
    test_simple_two_threads_res += i;
}

// -------------------------------
// test_spawn_new
// -------------------------------
static int test_spawn_new_res = 0;
static void test_spawn_new_f1(void *args);

static char *test_spawn_new() {
    start_test;
    int *arg = malloc(sizeof(int));
    *arg = 1;
    struct thread *t1 = thread_create(test_spawn_new_f1, arg);
    thread_add_runqueue(t1);
    thread_start_threading();
    free(arg);

    test_assert("error, != 100", test_spawn_new_res == 100);
    return 0;
}

static void test_spawn_new_f1(void *args) {
    if (*((int *) args)) {
        int *child = malloc(sizeof(int));
        int i = 0;
        while (i < 100) {
            *child = 0;
            thread_add_runqueue(thread_create(test_spawn_new_f1, child));
            i++;
            thread_yield();
        }
        free(child);
    } else {
        test_spawn_new_res++;
    }
}

// -------------------------------
// test_nested
// -------------------------------
static void test_nested_f1(void *args);
static void test_nested_f2(void *args);
static void test_nested_f3(void *args);

static char *test_nested() {
    start_test;
    struct thread *t1 = thread_create(test_nested_f1, NULL);
    thread_add_runqueue(t1);
    thread_start_threading();
    return 0;
}

static void test_nested_f3(void *arg) {
    for (int i = 0; i < 10; i++) {
        printf("thread 3: %d\n", i++);
    }
    thread_yield();
}

static void test_nested_f2(void *arg) {
    for (int i = 0; i < 10; i++) {
        printf("thread 2: %d\n", i++);
        if (i == 10) {
            i = 0;
        }
    }
    thread_yield();
}

static void test_nested_f1(void *arg) {
    struct thread *t2 = thread_create(test_nested_f2, NULL);
    thread_add_runqueue(t2);
    struct thread *t3 = thread_create(test_nested_f3, NULL);
    thread_add_runqueue(t3);
    for (int i = 0; i < 100; i++) {
        printf("thread 1: %d\n", i++);
    }
    thread_yield();
}


// -------------------------------
// main
// -------------------------------
// enable #define LOG_VERBOSE 1 in log.h

static char *all_tests() {
    test_run(test_single_thread);
    test_run(test_simple_two_threads);
    test_run(test_spawn_new);
    test_run(test_nested);
    return 0;
}

int main(int argc, char **argv) {
    char *result = all_tests();
    if (result != 0) {
        fprintf(stderr, "%s\n", result);
    } else {
        fprintf(stderr, "ALL TESTS PASSED\n");
    }
    fprintf(stderr, "Tests run: %d\n", tests_run);

    return result != 0;
}