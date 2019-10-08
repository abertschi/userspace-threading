#include <memory.h>
#include "threads.h"

#define FAIL_IF_NULL(ptr) {\
    if (!ptr){ \
        fprintf(stderr, "Fatal: failed to allocate memory\n"); \
        abort();}}\


struct thread *current_thread = NULL;

struct thread *
thread_create(void (*fun)(void *), void *arg) {
    struct thread* new_th = (struct thread *) malloc(sizeof(struct thread));
    FAIL_IF_NULL(new_th);
    new_th->arg = arg;
    new_th->fun = fun;
    const void* stack = malloc(STACK_SIZE);
    FAIL_IF_NULL(stack)
    memset(stack, 0, STACK_SIZE);
    new_th->stack_reg = stack;
    new_th->base_reg = stack;
    new_th->next_thread = NULL;
    return new_th;

    //    struct threads *elem = (struct thread *) malloc(sizeof(struct thread));
//    FAIL_IF_NULL(elem);
//    elem->next = NULL;
//    elem->thread = new_th;
//
//    struct threads *last = thread_head;
//    struct threads *walk = thread_head;
//    while (walk) {
//        last = walk;
//        walk = walk->next;
//    }
//    last = elem;

}

void
thread_add_runqueue(struct thread *t) {
    if (!current_thread) {
        current_thread = t;
        t->next_thread = t;
    }
     else {
        struct thread *prev = current_thread;
        current_thread = t;
        prev->next_thread = t;
        t->next_thread = prev->next_thread;
     }
}

void
thread_yield(void) {

}

void
dispatch(void) {

}

void
schedule(void) {


}

void
thread_exit(void) {

}

void
thread_start_threading(void) {

}

