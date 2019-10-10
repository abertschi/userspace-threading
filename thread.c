#include <memory.h>
#include "thread.h"
#include "log.h"

#define FAIL_IF_NULL(ptr) {\
    if (!ptr){ \
        fprintf(stderr, "Fatal: failed to allocate memory\n"); \
        abort();}}\

struct thread *current_thread = NULL;

struct thread *prev_thread = NULL;

int64_t new_thread_id = 0;

jmp_buf main_env;

struct thread *
thread_create(void (*fun)(void *), void *arg) {
    struct thread *new_th = (struct thread *) malloc(sizeof(struct thread));
    FAIL_IF_NULL(new_th);
    new_th->id = new_thread_id++;
    new_th->arg = arg;
    new_th->fun = fun;
    new_th->next = NULL;
    new_th->state = THREAD_CREATE;
    new_th->s_size = STACK_SIZE;
    new_th->sp_base = malloc(STACK_SIZE);
    FAIL_IF_NULL(new_th->sp_base)
    memset(new_th->sp_base, 0, STACK_SIZE);

    LL_LOG("thread with id %d created.", new_th->id);
    return new_th;
}

void
thread_add_runqueue(struct thread *t) {
    LL_LOG("adding thread %d to run queue...", t->id);

    if (!current_thread) {
        current_thread = t;
        prev_thread = t;
        t->next = t;
    } else {
        struct thread *prev_prev = prev_thread;
        prev_prev->next = t;
        t->next = current_thread;
        prev_thread = t;
    }
}

void
thread_yield(void) {
    LL_LOG("thread %d yields control...", current_thread->id);
    current_thread->state = THREAD_YIELD;
    if (setjmp(current_thread->jmp_buf)) {
        return;
    } else {
        longjmp(main_env, 1);
    }
}

void
dispatch(void) {
    LL_LOG("dispatching a thread...");
    if (current_thread->state == THREAD_CREATE) {
        // stack grows from high to low
        current_thread->state = THREAD_RUN;
        __asm__ __volatile__
        ("mov %0, %%rbp"::"r" (current_thread->sp_base + current_thread->s_size):);
        __asm__ __volatile__
        ("mov %0, %%rsp"::"r" (current_thread->sp_base + current_thread->s_size):);

        LL_LOG("start executing thread %d on new stack at %p...",
               current_thread->id, current_thread->sp_base);
        current_thread->fun(current_thread->arg);
        current_thread->state = THREAD_TERM;

        // escape back to main stack
        longjmp(main_env, 1);
    } else {
        LL_LOG("resuming execution of thread %d...", current_thread->id);
        current_thread->state = THREAD_RUN;
        longjmp(current_thread->jmp_buf, 1);
    }
}

void
schedule(void) {
    if (!current_thread) {
        fprintf(stderr, "No threads scheduled to run\n");
        abort();
    }
    prev_thread = current_thread;
    current_thread = current_thread->next;
}

void
thread_exit(void) {
    if (current_thread == current_thread->next) {
        free(current_thread->sp_base);
        free(current_thread);
        current_thread = NULL;
        prev_thread = NULL;
    } else {
        struct thread *thr_exit = current_thread;
        current_thread = current_thread->next;
        prev_thread->next = current_thread;
        free(thr_exit->sp_base);
        free(thr_exit);
    }
}

void
thread_start_threading(void) {
    LL_LOG("start threading...");

    if (!current_thread) {
        fprintf(stderr, "No threads scheduled to run\n");
        return;
    }
    while (current_thread) {
        if (!setjmp(main_env)) {
            if (current_thread->state == THREAD_TERM) {
                thread_exit();
                continue;
            }
            schedule();
            dispatch();
        }
    }
    LL_LOG("all threads executed.");
}

