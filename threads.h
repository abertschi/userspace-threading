#ifndef THREADS_H_
#define THREADS_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include<setjmp.h>

#define STACK_SIZE 65536

extern struct thread *current_thread;
extern struct thread *prev_thread;


struct thread
{
    int id;
    void (*fun) (void *);
    void *arg;
    bool init_run;
    void *stack_reg;
    void *base_reg;
    struct thread *next_thread;
    jmp_buf jmp_buf;
};

struct thread *thread_create(void (*fun)(void *), void *arg);
void thread_add_runqueue(struct thread *t);
void thread_yield(void);
void dispatch(void);
void schedule(void);
void thread_exit(void);
void thread_start_threading(void);

#endif // THREADS_H_
