#include <memory.h>
#include "threads.h"
#include "log.h"

#define FAIL_IF_NULL(ptr) {\
    if (!ptr){ \
        fprintf(stderr, "Fatal: failed to allocate memory\n"); \
        abort();}}\

struct thread *current_thread = NULL;
struct thread *prev_thread = NULL;
int new_thread_id = 0;
jmp_buf main_thread;

struct thread *
thread_create(void (*fun)(void *), void *arg) {
    LL_LOG("creating a new thread...");
    struct thread *new_th = (struct thread *) malloc(sizeof(struct thread));
    FAIL_IF_NULL(new_th);
    new_th->id = new_thread_id++;
    new_th->arg = arg;
    new_th->fun = fun;
    new_th->next_thread = NULL;
    new_th->init_run = true;

    void *stack = malloc(STACK_SIZE);
    FAIL_IF_NULL(stack)
    memset(stack, 0, STACK_SIZE);

    new_th->stack_reg = stack + STACK_SIZE;
    new_th->base_reg = stack;

    LL_LOG("thread with id %d created.", new_th->id);
    return new_th;
}

void
thread_add_runqueue(struct thread *t) {
    LL_LOG("adding thread %d to run queue...", t->id);
    if (!current_thread) {
        current_thread = t;
        t->next_thread = t;
    } else {
        struct thread *prev_prev = prev_thread;
        prev_prev->next_thread = t;
        t->next_thread = current_thread;
        prev_thread = t;
    }
}

void
thread_yield(void) {
    LL_LOG("yielding a thread...");
    if (setjmp(current_thread->jmp_buf)) {
        return;
    } else {
        longjmp(main_thread, 1);
    }
}

void
dispatch(void) {
    LL_LOG("dispatching a thread...");
    if (current_thread->init_run) {
        current_thread->init_run = false;
        LL_LOG("initial dispatch of thread %d", current_thread->id);
        LL_LOG("executing thread %d", current_thread->id);

        __asm__ __volatile__
        ("mov %0, %%rbp"::"r" (current_thread->stack_reg):);
        __asm__ __volatile__
        ("# overwrite base and stack pointer \n\t"
         "mov %0, %%rsp"::"r" (current_thread->stack_reg ) :);

        current_thread->fun(current_thread->arg);
        thread_exit();
        longjmp(main_thread, 1);
    } else {
        LL_LOG("thread %d previously run, continuing execution.", current_thread->id);
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
    current_thread = current_thread->next_thread;
}

void
thread_exit(void) {
    if (current_thread == current_thread->next_thread) {
        free(current_thread->base_reg);
        free(current_thread);
        current_thread = NULL;
        prev_thread = NULL;
    } else {
        struct thread *thr_exit = current_thread;
        current_thread = current_thread->next_thread;
        prev_thread->next_thread = current_thread;
        free(thr_exit->base_reg);
        free(thr_exit);
    }
}

void
thread_start_threading(void) {
    if (!current_thread) {
        fprintf(stderr, "No threads scheduled to run\n");
        return;
    }
    while (current_thread) {
        if (!setjmp(main_thread)) {
            schedule();
            dispatch();
        }
    }
    LL_LOG("all threads executed.");
}

