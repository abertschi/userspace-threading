#ifndef THREADS_H_
#define THREADS_H_

#include <stdio.h>
#include <stdlib.h>

#define STACK_SIZE 8192

struct thread
{
    void (*fun) (void *);
    void *arg;
    void *stack_reg;
    void *base_reg;
    struct thread *next_thread;
};

struct thread *thread_create(void (*fun)(void *), void *arg);
void thread_add_runqueue(struct thread *t);
void thread_yield(void);
void dispatch(void);
void schedule(void);
void thread_exit(void);
void thread_start_threading(void);

#endif // THREADS_H_
