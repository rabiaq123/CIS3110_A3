#include <stdio.h>     /* Input/Output */
#include <stdlib.h>    /* General Utilities */
#include <string.h>
#include <unistd.h>    /* Symbolic Constants */
#include <sys/types.h> /* Primitive System Data Types */
#include <sys/wait.h>  /* Wait for Process Termination */
#include <limits.h>    /* Constants Specifying Limits on Cmd-line Args */
#include <time.h>
#include <ctype.h>


/**
 * Name: Rabia Qureshi
 * Student #: 1046427
 * E-mail: rqureshi@uoguelph.ca
 */

typedef struct Process {
    char id[2];
    int size; //cannot be over 128MB
    int times_swapped; //process cannot be swapped out more than three times
    struct Process *next;
} Process;

char memory[128] = {'\0'}; //memory is initially empty
int queue_len = 0; //length of Process list (queue)


//parse file - parse each line (one process) to get the process ID and its size
//store each line in one Process struct
//once one line has been parsed, add Process node to list of Processes
    //if Process list has not been defined, create new list
//have mem array be empty (use '-' or '\0' to denote) 
//load one process at a time in the order they were given in the file into memory, store in array by id
    //remove the process from the Process list when the process is loaded into memory, as the list represents the queue
//if no space to store in memory, swap a process out of memory until you have enough space
    //the first process to be swapped out should be the one which has been in memory the longest
        //the process that's been in memory longest will be the one with the smallest/lowest id
    //note a process can be swapped out no more than 3 times (after that assume the process has been run to completion)
    //once it's swapped out, it goes to the end of the queue of processes waiting to be loaded into memory


/**
 * add current process to Process list given head node of process.
 * Note: modifying the ptr itself requires a double ptr, while modifying something the ptr is pointing to doesn't.
 * @param p Process node being inserted in Process list
 * @param p_head start of the Process list (ptr to ptr to modify p_head outside of func)
 */
void add_to_queue(Process *p, Process **p_head) {
    Process *itr; //temp

    if (*p_head == NULL) { //start of the list
        *p_head = p;
        return;
    }

    itr = *p_head;
    while (itr->next != NULL) itr = itr->next; //traverse list
    if (itr->next == NULL) itr->next = p; //insert node at back
}


/**
 * initialize new Process node given its id and size parsed from line in file
 * @param id process id
 * @param size process size
 * @return new Process node
 */ 
Process *create_process(char id[2], int size) {
    Process *p = malloc(sizeof(Process));
    
    strcpy(p->id, id);
    p->size = size;
    p->times_swapped = 0;
    p->next = NULL;

    return p;
}


/**
 * parse one line in the file into Process node and add node to list of Processes
 * @param line string representing line from file
 * @param p_head start of the Process list (ptr to ptr to modify p_head outside of func)
 */
void parse_line(char line[10], Process **p_head) {
    char p_id[2] = {'\0'};
    int p_size;
    char line_buffer[10] = {'\0'};
    char *token = NULL;
    Process *new_process;

    //parse process id
    strcpy(line_buffer, line);
    token = strtok(line_buffer, " "); //split input into tokens separated by spaces
    strcpy(p_id, token);
    //parse process size
    token = strtok(NULL, " "); //parse remainder of line
    p_size = atoi(token);

    new_process = create_process(p_id, p_size);
    add_to_queue(new_process, p_head);
}


/**
 * parse file 
 * @param filename name of file from command line 
 * @param p_head Process list head (first node in Process list)
 * @return int representing whether file could be parsed (0 if not)
 */
int parse_file(char *filename, Process **p_head) {
    FILE *fptr;
    char line[10] = {'\0'};

    fptr = fopen(filename, "r");
    if (!fptr) return 0; //file parsing unsuccessful

    //read a line until EOF reached
    while (fgets(line, 10, fptr) != NULL) {
        parse_line(line, p_head);
        queue_len++;
    }

    return 1;
}


/**
 * remove process from wait queue before storing in memory
 * @param p_head Process list head (first node in Process list) - will be changed once process is removed.
 * NOTE:    alter parts of the head pointer (e.g. size): use p_head 
 *          alter the head pointer itself: use *p_head
 */
void remove_from_queue(Process **p_head) {
    Process *temp = *p_head;

    //move head ptr to the next node
    if ((*p_head)->next != NULL) {
        *p_head = (*p_head)->next;
    } else { //no more items will be in list after head node is freed
        free(*p_head);
        *p_head = NULL;
    }

    free(temp);
}


/**
 * "add" one process into memory 
 * and remove it from the Process list (queue)
 * @param p_head Process list head (first node in Process list)
 * @return int representing whether process could be added into memory. 
 * if not, use algo to fill most holes possible.
 */
int add_to_memory(Process **p_head) {
    if (*p_head == NULL) return 0; //no more processes in wait queue

    //load process that arrived earliest in queue into memory
    for (int i = 0; i < (*p_head)->size; i++) memory[i] = (*p_head)->id[0];
    //remove process from queue
    remove_from_queue(p_head);
    queue_len--;

    return 1;
}


/**
 * first fit algorithm for filling processes into holes in memory
 * @param p_head Process list head (first node in Process list)
 */
void firstFit(Process **p_head) {
    /*
    loop until no more proccesses waiting to be loaded into memory and/or 
    no more processes could be added to memory without swapping others out
    */
    while (queue_len > 0 && (add_to_memory(p_head))) {}

}


int main(int argc, char *argv[]) {
    //check for invalid command-line arguments
    if (argc != 3) {
        printf("Invalid input.\nEnter (1) filename and (2) the allocation strategy.\n");
        return -1;
    }

    char filename[200] = {'\0'}; 
    char algo[10] = {'\0'};
    Process *p_head = NULL; //head of Process list containing all processes from file

    //parse filename and allocation strategy
    strcpy(filename, argv[1]);
    strcpy(algo, argv[2]);

    //parse file and add processes to queue
    if (!parse_file(filename, &p_head)) {
        printf("File parsing failed.\n");
        return -1;
    }

    //load processes into memory
    if (strcmp(algo, "first") == 0) firstFit(&p_head);

    return 0;
}
