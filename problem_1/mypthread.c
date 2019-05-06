#include "mypthread.h"


int nodeNum = 0;

int getcount() {
	return nodeNum;
}


mypthread_t* headout() {
	return (head->n);
}
mypthread_t* tailout() {
	return (tail->n);
}


void create_node() {
	mypthread_t data;
	temp = (struct threadNode *) malloc(1 * sizeof(struct threadNode));
	temp->prev = NULL;
	temp->next = NULL;
	nodeNum++;
}


void enqueue(mypthread_t* data) {
	if (head == NULL) {
		create_node();
		head = temp;
		tail = temp;
	} else {
		create_node();
		tail->next = temp;
		temp->prev = tail;
		tail = temp;
		tail->next = head;
		head->prev = tail;
	}
	temp->n = data;
}

mypthread_t* threadsearch(int threadNum) {
	temp = head;
	while (temp != NULL) {
		if (temp->n->tid == threadNum)
			return (temp->n);
		else {
			temp = temp->next;
		}
	}
	printf("Error, %d is missing!", threadNum);
	exit(0);
}

mypthread_t* searchNextThread(int threadNum) {

	temp = head;

	while (temp != NULL) {
		if (temp->n->tid == threadNum) {

			break;
		} else {
			temp = temp->next;
		}
	}
	if (temp->next != NULL) {
		temp = temp->next;
		while (temp != NULL) {
			if (temp->n->state == PS_ACTIVE) {
				return temp->n;
			} else {
				temp = temp->next;
			}
		}

	} else {
		exit(0);
	}

	printf("No thread found after threadNum%d\n", threadNum);
	exit(0);
}

int currentTID = 1;

int numOfThreads = 1;

int mypthread_create(mypthread_t *thread, const mypthread_attr_t *attr,
		void *(*start_routine)(void *), void *arg) {

	if (getcount() == 0) {

		mypthread_t* main_thread = (mypthread_t *) malloc(sizeof(mypthread_t));
		main_thread->tid = numOfThreads++;
		ucontext_t* context = (ucontext_t*) malloc(sizeof(ucontext_t));
		main_thread->ctext = context;
		main_thread->ctext->uc_stack.ss_sp = (char*) malloc(sizeof(char) * 4096);
		main_thread->ctext->uc_stack.ss_size = 4096;
		main_thread->state = PS_ACTIVE;
		enqueue(main_thread);
	}

	ucontext_t* context = (ucontext_t*) malloc(sizeof(ucontext_t));
	thread->ctext = context;
	getcontext(thread->ctext);
	(*thread).ctext->uc_stack.ss_sp = (char*) malloc(sizeof(char) * 4096);
	(*thread).ctext->uc_stack.ss_size = 4096;
	(*thread).state = PS_ACTIVE;
	thread->tid = numOfThreads++;
	makecontext(thread->ctext, (void (*)()) start_routine, 1, arg);
	enqueue(thread);
	return 0;
}

void mypthread_exit(void *retval) {

	mypthread_t* currentThread = threadsearch(currentTID);

	currentThread->state = PS_DEAD;
	free(currentThread->ctext);
	if (currentThread->join_th != 0)
			{
		mypthread_t* join_thread = threadsearch(currentThread->join_th);
		join_thread->state = PS_ACTIVE;
		currentThread->join_th;
	}


	mypthread_t* nextThread = searchNextThread(currentThread->tid);
	if (currentTID == nextThread->tid)
		return;
	currentTID = nextThread->tid;
	setcontext(nextThread->ctext);

}

int mypthread_yield(void) {

	mypthread_t* currentThread = threadsearch(currentTID);

	mypthread_t* nextThread = searchNextThread(currentThread->tid);

	if (currentTID == nextThread->tid)
		return 0;


	currentTID = nextThread->tid;
	swapcontext(currentThread->ctext, nextThread->ctext);
	return 0;
}


int mypthread_join(mypthread_t thread, void **retval) {
	int target_tid = thread.tid;
	mypthread_t* currentThread = threadsearch(currentTID);
	mypthread_t* target_thread = threadsearch(thread.tid);
	if (target_thread->state != PS_ACTIVE) {
		return 0;
	} else {
		currentThread->state = PS_BLOCKED;
		target_thread->join_th = currentTID;
		currentTID = target_tid;
		swapcontext(currentThread->ctext, target_thread->ctext);
	}
	return 0;
}
