#include <stdio.h> // for standard input/output
#include <stdbool.h> // required to use bool in C
#include <pthread.h> // for threads
#include <stdlib.h> // for atoi() function
#include <semaphore.h> // for semaphores
#include <unistd.h> // for sleep function
#include <time.h> // for clock_t (execution timing)

#define KRED  "\x1B[31m" // RED Text Color
#define KGRN  "\x1B[32m" // GREEN Text Color
#define KCYN  "\x1B[36m" // CYAN Text Color
#define KMAG  "\x1B[35m" // MAGENTA Text Color
#define RESET  "\x1B[0m" // DEFAULT Text Color

sem_t t;
sem_t c;

float R;
float curr_R;

sem_t r_mutex;
sem_t t_mutex;

int inf = 100000000;

void go_home()
{
    printf("%u is going home now.\n", (int) pthread_self());
    pthread_exit(0); // person leaves
}

void teacher_enter()
{
    sem_post(&t); // increment the number of teachers, if another was waiting to leave, it will be released
    printf(KMAG "Teacher %s%u just arrived.\n", RESET, (int) pthread_self());
}

void teach()
{
    printf(KMAG "Teacher %s%u is teaching.\n", RESET, (int) pthread_self());
    sleep(10); // teachers teach for 10 seconds
}

void teacher_exit()
{
    int curr_teach; // current number of teachers
    sem_getvalue(&t, &curr_teach);

    int curr_child; // current number of children
    sem_getvalue(&c, &curr_child);

    float temp_R;

    if ( (curr_teach - 1) != 0 ){
        temp_R = curr_child/(curr_teach-1);

    } else {
        if (curr_child == 0){
            temp_R = 0;

        } else {
            temp_R = inf;

        }
    }

    // if when the teacher leaves the ratio will still be met, the teacher can leave freely
    if (temp_R <= R){

        sem_wait(&t); // decrement number of teachers
        printf(KMAG "Teacher %s%u just left.\n", RESET, (int) pthread_self());

    } else { // else, teacher can't leave until another joins

        printf(KMAG "Teacher %s%u wants to leave but can't.\n" 
                "   Will go back to work and then try again later.\n", RESET, (int) pthread_self());

        teach(); // teacher is teaching again
        teacher_exit(); // teacher attempts to leave again
    }
}

void child_enter()
{
    sem_post(&c); // increment the number of children
    printf(KGRN "Child %s%u just arrived.\n", RESET, (int) pthread_self());
}

void child_exit()
{
    sem_wait(&c); // decrement number of children
    printf(KGRN "Child %s%u just left.\n", RESET, (int) pthread_self());
}

void learn()
{
    printf(KGRN "Child %s%u is learning.\n", RESET, (int) pthread_self());
    sleep(10); // children learn for 10 seconds
}

void parent_enter()
{
    printf(KCYN "Parent %s%u just arrived.\n", RESET, (int) pthread_self());
}

void verify_compliance()
{
    int curr_teach; // current number of teachers
    sem_getvalue(&t, &curr_teach);

    int curr_child; // current number of children
    sem_getvalue(&c, &curr_child);

    if ( curr_teach!= 0 ) {
    curr_R = curr_child/curr_teach;
    } else {
        if (curr_child == 0){
            curr_R = 0;
        } else {
        curr_R == inf;

        }
    }

    sem_post(&r_mutex); // unlock the resources

    if (curr_R <= R) { // if the compliance is verified
        printf(KCYN "Parent %s%u has verified compliance!\n", RESET, (int) pthread_self());

    } else {
        printf(KCYN "Parent %s%u is unsatisfied because the regulation is not met!\n", RESET, (int) pthread_self());
    }
}

void parent_exit()
{
    printf(KCYN "Parent %s%u just left.\n", RESET, (int) pthread_self());
}

void Teacher()
{
    for (;;) {
        teacher_enter();

        /* CRITICAL SECTION */

        teach();
        sem_wait(&t_mutex); // locking so that only one teacher can leave at a time
        teacher_exit();
        sem_post(&t_mutex);

        go_home();
    }
}

void Child()
{
    for (;;) {
        child_enter();

        /* CRITICAL SECTION */

        learn();
        child_exit();

        go_home();
    }
}

void Parent()
{
    for (;;) {
        parent_enter();

        // ... critical section ... //

        verify_compliance();
        parent_exit();

        go_home();
    }
}

int main(int argc, char *argv[])
{
    /*
    argv[1] = # of teachers
    argv[2] = # of children
    argv[3] = # of parents
    argv[4] = R (ratio of children per teacher)
    */

    // Guard for invalid input arguments
    if (argc != 5) {
        fprintf(stderr,
            KRED "Error with arguments.\n"
            "Must be formatted: %s (# of Teachers) (# of Children) (# of Parents) (Ratio)\n" RESET,
            argv[0]);
        return 0;
    }

    clock_t begin = clock(); // Begin execution timer

    sem_init(&c, 0, 0); // initialize semaphore for children to 0
    sem_init(&t, 0, 0); // initialize semaphore for teachers to 0
    sem_init(&r_mutex, 0, 1); // initialize semaphore for teachers to 0
    sem_init(&t_mutex, 0, 1); // initialize semaphore for teachers to 0

    int count_teacher = atoi(argv[1]);
    int count_child = atoi(argv[2]);
    int count_parent = atoi(argv[3]);

    R = atoi(argv[4]); // arbitrary number for ratio required

    // Threads Instantiated

    pthread_t child_tid[count_child];
    pthread_t teacher_tid[count_teacher];
    pthread_t parent_tid[count_parent];

    pthread_attr_t attr;  

    pthread_attr_init(&attr); // Get default attributes

    // Thread creation

    for(int i = 0;i<count_teacher;i++) {
        pthread_create(&teacher_tid[i],&attr,(void*) Teacher, NULL);
    }

    sleep(1);

    for(int i = 0;i<count_child;i++) {
        pthread_create(&child_tid[i],&attr,(void *) Child, NULL);
    } 

    for(int i = 0; i<count_parent;i++) {
        pthread_create(&parent_tid[i],&attr,(void*) Parent, NULL);
        sleep(3);
    }

    // Thread joining

    for(int i = 0;i<count_teacher;i++) {
        pthread_join(teacher_tid[i],NULL);
    }

    for(int i = 0;i<count_child;i++) {
        pthread_join(child_tid[i], NULL);
    }

    for(int i = 0; i<count_parent;i++) {
        pthread_join(parent_tid[i], NULL);
    }

    sem_destroy(&c);
    sem_destroy(&t);
    sem_destroy(&r_mutex);
    sem_destroy(&t_mutex);

    clock_t end = clock(); // End execution timer
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

    printf("\nExecution time: %f\n", time_spent);

    return 0;
}