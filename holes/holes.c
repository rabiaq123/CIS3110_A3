#include <stdio.h>     /* Input/Output */
#include <stdlib.h>    /* General Utilities */
#include <string.h>
#include <stdbool.h>
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

char memory[128] = {'\0'}; //empty


//parse file - parse each line (one process) to get the process ID and its size
//store each line in one Process struct
//once one line has been parsed, add Process node to list of Processes
    //if Process list has not been defined, create new list
//have mem array be empty (use '-' or '\0' to denote) 
//load one process at a time in the order they were given in the file into memory, store in array by id
//if no space to store in memory, swap a process out until you have enough space
    //note a process can be swapped out no more than 3 times (after that assume the process has been run to completion)
    //the first process to be swapped out should be the one which has been in memory the longest
    //once it's swapped out, it goes to the end of the queue of processes waiting to be loaded into memory


/**
 * add current process to Process list given head node of process.
 * Note: modifying the ptr itself requires a double ptr, while modifying something the ptr is pointing to doesn't.
 * @param p Process node being inserted in Process list
 * @param p_head start of the Process list (ptr to ptr to modify p_head outside of func)
 */
void add_to_list(Process *p, Process **p_head) {
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
    add_to_list(new_process, p_head);
}


/**
 * parse file 
 * @param filename name of file from command line 
 * @param p_head Process list head (first node in Process list)
 * @return boolean representing whether file could be parsed
 */
bool parse_file(char *filename, Process **p_head) {
    FILE *fptr = fopen(filename, "r");
    if (!fptr) return false;

    char line[10] = {'\0'};

    //read a line until EOF reached
    while (fgets(line, 10, fptr) != NULL) {
        parse_line(line, p_head);
    }

    // reads text until newline is encountered
    // fscanf(fptr, "%[^\n]", c);

}


int main(int argc, char *argv[]) {
    //check for invalid command-line arguments
    if (argc != 3) {
        printf("Invalid input.\nEnter (1) filename and (2) the allocation strategy.\n");
        return -1;
    }

    char filename[200] = {'\0'}; 
    char algo[10] = {'\0'};
    Process *p_head = NULL;

    //parse filename and allocation strategy
    strcpy(filename, argv[1]);
    strcpy(algo, argv[2]);

    //parse file
    if (!parse_file(filename, &p_head)) {
        printf("File parsing failed.\n");
        return -1;
    }

    return 0;
}
