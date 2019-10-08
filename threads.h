#ifndef THREADS_H_
#define THREADS_H_

// TODO: necessary includes, if any

// TODO: necessary defines, if any

// TODO: definition of struct thread

struct thread *thread_create(void (*f)(void *), void *arg);
void thread_add_runqueue(struct thread *t);
void thread_yield(void);
void dispatch(void);
void schedule(void);
void thread_exit(void);
void thread_start_threading(void);

#endif // THREADS_H_
