#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

#define TEACH_TIME 2
#define LEARN_TIME 2
#define VERIFY_TIME 2

#define N1 3
#define N2 9
#define N3 3

#define KRED  "\x1B[31m" // RED Text Color
#define RESET  "\x1B[0m" // DEFAULT Text Color

pthread_cond_t child_cv, teacher_cv;

pthread_mutex_t child_ticket_lock, teacher_ticket_lock;

int child_ticket, teacher_ticket;

void go_home() { pthread_exit(0); }

/* Teacher */
/* Get R children */
void teacher_enter()  // Ticket should probably be argument
{
    int ticket;

    pthread_mutex_lock(&teacher_ticket_lock);
    (teacher_ticket++) % N1;  // We need to define N in here
    ticket = teacher_ticket % N1;
    pthread_mutex_unlock(&teacher_ticket_lock);

    pthread_cond_broadcast(&teacher_cv);
    
    pthread_mutex_lock(&child_ticket_lock);
    while (child_ticket < ticket)
        pthread_cond_wait(&child_cv, &child_ticket_lock);
    pthread_mutex_unlock(&child_ticket_lock);
    
    /* https://www.youtube.com/watch?v=bBCB0y9Mgns */
}

void teach() { sleep(TEACH_TIME); }

/* Release R children */
void teacher_exit()  // Ticket should probably be argument
{
    // Need to check if children have exited
    // So get lock to child array
}

/* Child */
void child_enter()  // Ticket should probably be argument
{
    int ticket;

    pthread_mutex_lock(&child_ticket_lock);
    (child_ticket++) % N2;
    ticket = child_ticket % N2;
    pthread_mutex_unlock(&child_ticket_lock);

    pthread_cond_broadcast(&child_cv);

    pthread_mutex_lock(&teacher_ticket_lock);
    while (teacher_ticket < ticket)
        pthread_cond_wait(&teacher_cv, &teacher_ticket_lock);
    pthread_mutex_unlock(&teacher_ticket_lock);
}

void learn() { sleep(LEARN_TIME); }

void child_exit()  // Ticket argument here too
{
    // Remove self from array
}

/* Parent */
void parent_enter()  
{

}

void verify_compliance()  // Get lock to child array for reading only (readers/writers)
/* This should run some kind of check */
{
    sleep(VERIFY_TIME);
}

void parent_exit() 
{

}

void *Teacher(void *arg)
{
    for (;;) {
        teacher_enter();
        //crit
        teach();
        teacher_exit();
        go_home();
    }
}

void *Child(void *arg)
{
    for (;;) {
        child_enter();
        //crit
        learn();
        child_exit();
        go_home();
    }
}

void *Parent(void *arg)
{
    for (;;) {
        parent_enter();
        //crit
        verify_compliance();
        parent_exit();
        go_home();
    }
}

int main(int argc, char *argv[])
{
    // Guard for invalid input arguments
    if (argc != 3) {
        fprintf(stderr,
            KRED "Error with arguments.\n"
            "Must be formatted: %s N R\n" RESET,
            argv[0]);
        return 0;
    }

    for (int i = 0; i < argv[1]; i++) {
        pthread_create(/**/);
    }
}