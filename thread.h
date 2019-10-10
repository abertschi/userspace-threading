#ifndef THREADS_H_
#define THREADS_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <setjmp.h>

#define LOG_VERBOSE 1
#define STACK_SIZE 65536

enum {
    THREAD_CREATE, // 0
    THREAD_YIELD, // 2
    THREAD_RUN, // 3
    THREAD_TERM // 1
};


struct thread
{
    int64_t id;
    int8_t state;
    void (*fun) (void *);
    void *arg;
    void *sp_base;
    int32_t s_size;
    jmp_buf jmp_buf;
    struct thread *next;
};

struct thread *thread_create(void (*fun)(void *), void *arg);
void thread_add_runqueue(struct thread *t);
void thread_yield(void);
void thread_start_threading(void);

static void dispatch(void);
static void schedule(void);
static void thread_exit(void);

#endif // THREADS_H_
