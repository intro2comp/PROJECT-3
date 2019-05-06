#ifndef H_MYPTHREAD
#define H_MYPTHREAD

#include <ucontext.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

enum mypthread_state {
	PS_ACTIVE,
	PS_BLOCKED,
	PS_DEAD
};

typedef struct {
	int tid;
	ucontext_t* ctext;
	enum mypthread_state state;
	int join_th;
} mypthread_t;

typedef struct {
} mypthread_attr_t;

int mypthread_create(mypthread_t *thread, const mypthread_attr_t *attr,
			void *(*start_routine) (void *), void *arg);

void mypthread_exit(void *retval);

int mypthread_yield(void);

int mypthread_join(mypthread_t thread, void **retval);

struct threadNode {
	struct threadNode *prev;
	mypthread_t *n;
	struct threadNode *next;
}*head, *tail, *temp, *seachNode;


/* Don't touch anything after this line.
 *
 * This is included just to make the mtsort.c program compatible
 * with both your ULT implementation as well as the system pthreads
 * implementation. The key idea is that mutexes are essentially
 * useless in a cooperative implementation, but are necessary in
 * a preemptive implementation.
 */

typedef int mypthread_mutex_t;
typedef int mypthread_mutexattr_t;

static inline int mypthread_mutex_init(mypthread_mutex_t *mutex,
        	const mypthread_mutexattr_t *attr) { return 0; }

static inline int mypthread_mutex_destroy(mypthread_mutex_t *mutex) { return 0; }

static inline int mypthread_mutex_lock(mypthread_mutex_t *mutex) { return 0; }

static inline int mypthread_mutex_trylock(mypthread_mutex_t *mutex) { return 0; }

static inline int mypthread_mutex_unlock(mypthread_mutex_t *mutex) { return 0; }

#endif
