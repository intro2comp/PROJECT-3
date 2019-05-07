CC = gcc
CFLAGS = -g
LDFLAGS = -lpthread


all: semaphores condition-vars

semaphores:
	${CC} ${CFLAGS} kindergarten_sem.c -o kindergarten_sem ${LDFLAGS}

condition-vars:
	${CC} ${CFLAGS} kindergarten_cond.c -o kindergarten_cond ${LDFLAGS}

clean:
	rm -vf kindergarten_cond kindergarten_sem