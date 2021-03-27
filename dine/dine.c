#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

/**
 * Name: Rabia Qureshi
 * Student #: 1046427
 * E-mail: rqureshi@uoguelph.ca
 */


//info related to philosopher
typedef struct Philosopher {
	int id;
	pthread_t thread_id;
} phil;

//info related to chopstick
typedef struct Chopstick {
	int id;
	pthread_mutex_t mutex;
} chopstick;

//for storing all philosophers' and chopsticks' info
int num_phils = 0;
int num_eats = 0;
phil **phil_list;
chopstick **cs_list;


/**
 * start routine for pthread_create() method
 */
void *dine(void *num) {
    int phil_id = *(int *)num;
    int right_cs, left_cs;

    //calculating IDs of right and left chopsticks
    right_cs = (phil_id + 1) % num_phils;
    left_cs = (phil_id + (num_phils - 1)) % num_phils;

    printf("Philosopher %d thinking\n", phil_id + 1);
    sleep(rand() % 5); //thinks for any amount time between 0-4 time units

    //repeat until philosopher has eaten max number of times
    for (int i = 0; i < num_eats; i++) {

        //taking chopsticks
        pthread_mutex_lock(&cs_list[right_cs]->mutex);
        pthread_mutex_lock(&cs_list[left_cs]->mutex);

        //philosopher is now eating
        printf("Philosopher %d eating\n", phil_id + 1);
        sleep(rand() % 5); //eats for any amount of time between 0-4 time units

        //letting go of chopsticks
        pthread_mutex_unlock(&cs_list[right_cs]->mutex);
        pthread_mutex_unlock(&cs_list[left_cs]->mutex);
        printf("Philosopher %d thinking\n", phil_id + 1);
        sleep(rand() % 5+i); //thinks for any amount time between (0-4)+i time units
    }
}


int main(int argc, char *argv[]) {
    //check for invalid command-line arguments
    if (argc != 3) {
        printf("Invalid input.\nEnter (1) num philosophers and (2) num eats for each.\n");
        return -1;
    }

    //convert input to int
    num_phils = atoi(argv[1]);
    num_eats = atoi(argv[2]);
    int error;

    //validating command-line arguments
    if (num_phils <= 2 || num_eats < 1 || num_eats > 1000) {
        printf("Invalid input.\nNeed more than 2 philosophers and between 1 and 1000 num eats for each.\n");
        return -1;
    }

    //initialize list of philosophers at dining table
    phil_list = malloc(num_phils * sizeof(phil *));
    for (int i = 0; i < num_phils; i++) {
        phil_list[i] = malloc(sizeof(phil));
        phil_list[i]->id = i;
    }

    //initialize list of chopsticks at dining table
    cs_list = malloc(num_phils * sizeof(chopstick *));
    for (int i = 0; i < num_phils; i++) {
        cs_list[i] = malloc(sizeof(chopstick));
        cs_list[i]->id = i;
        pthread_mutex_init(&cs_list[i]->mutex, NULL);
    }

    //create threads and begin simulation
    for (int i = 0; i < num_phils; i++) {
        error = pthread_create(&phil_list[i]->thread_id, NULL, dine, &phil_list[i]->id);
        if (error) {
            printf("Error occurred while executing pthread_create(). Error Code: %d\n", error);
            return -1;
        }
    }

    //join threads back to the main process
    for (int i = 0; i < num_phils; i++) {
        pthread_join(phil_list[i]->thread_id, NULL);
    }

    //delete threads
    pthread_exit(NULL);

    //free dynamically allocated memory
    for (int i = 0; i < num_phils; i++) free(phil_list[i]);
    for (int i = 0; i < num_phils; i++) free(cs_list[i]);
    free(phil_list);
    free(cs_list);

    return 0;
}
