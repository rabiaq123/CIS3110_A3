#include <stdio.h>     /* Input/Output */
#include <stdlib.h>    /* General Utilities */
#include <string.h>
#include <unistd.h>    /* Symbolic Constants */
#include <sys/types.h> /* Primitive System Data Types */
#include <sys/wait.h>  /* Wait for Process Termination */
#include <limits.h>    /* Constants Specifying Limits on Cmd-line Args */
#include <time.h>
#include <ctype.h>
#include <math.h>


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

Process *mem_list = NULL;
char memory[128] = {'\0'}; //memory is initially empty


/**
 * remove process from wait queue before storing in memory
 * @param p_head head in queue (first node in Process list) - will be changed once process is removed.
 * @return Process node to be removed from queue; should be freed by calling method if applicable
 */
Process *remove_from_queue(Process **p_head) {
    Process *temp = *p_head;

    //move head ptr to the next node... if no next node exists, list head = NULL
    *p_head = (*p_head)->next;

    temp->next = NULL; //should not point to next node in queue after being added to memory
    return temp;
}


/**
 * insert current process to back of Process list given head node of process.
 * @param p Process node being inserted in Process list
 * @param p_head head in queue (ptr to ptr to modify p_head outside of func)
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
 * @param p_head head in queue (ptr to ptr to modify p_head outside of func)
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
    if (token != NULL) {
        p_size = atoi(token);
        new_process = create_process(p_id, p_size);
        add_to_queue(new_process, p_head);
    }
}


/**
 * parse file 
 * @param filename name of file from command line 
 * @param p_head head in queue (first node in Process list)
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
    }

    return 1;
}


/**
 * remove process from memory list
 * @return Process node to be removed from memory; should be freed by calling method if applicable
 */
Process *remove_from_mem_list() {
    if (mem_list == NULL) return NULL;

    Process *temp = mem_list;

    /*
    move head ptr to the next node... if no next node exists, list head = NULL
    we don't need a loop as we don't need to traverse to the end of the list
    */
    mem_list = mem_list->next;

    temp->next = NULL; //ensure node is not still connected to the next memory list node
    return temp;
}


/**
 * insert current process to back of memory list.
 * @param p Process node being loaded into memory
 */
void add_to_mem_list(Process *p) {
    Process *itr;

    if (mem_list == NULL) { //start of the list
        mem_list = p;
        return;
    }

    itr = mem_list;
    while (itr->next != NULL) itr = itr->next;
    if (itr->next == NULL) itr->next = p;
}


/**
 * retrieve last node in mem list
 * @return process id of the recently added process
 */
char get_pid() {
    Process *itr;

    if (mem_list == NULL) return '\0'; //start of the list

    itr = mem_list;
    while (itr->next != NULL) itr = itr->next;

    return itr->id[0];
}


/**
 * update memory array  by adding new process id to it
 * @param p new process being added to memory
 * @param idx start index of the hole
 */
void add_to_mem_array(Process *p, int idx) {
    for (int j = 0; j < p->size; j++) {
        memory[idx++] = p->id[0];
    }
}


/**
 * "add" process that arrived earliest in queue into memory 
 * and remove it from the queue
 * @param p_head head in queue (first node in Process list)
 * @return int representing whether process could be added into memory. 
 * if not (return value 0), use algo to fill most holes possible.
 */
int add_to_memory(Process **p_head) {
    if (*p_head == NULL) return 0; //no more processes in wait queue to add to memory

    int start = 0, hole_size = 0;
    int hole = 1, hole_idx = 0;
    Process *p;

    //iterate through memory to find holes
    for (int i = 0; i < 128; i++) {
        //look for hole in memory
        hole = 0;
        hole_size = 0;
        hole_idx = i;
        while (memory[hole_idx] == '\0' && hole_idx < 128) { //loop until end of hole or end of memory space reached
            hole = 1;
            hole_size++;
            if (hole_size == 1) start = i;
            hole_idx++;
        }
        //attempt to fill hole with process
        if (hole) {
            if (hole_size >= (*p_head)->size) {
                //remove process from queue
                p = remove_from_queue(p_head);
                //load process that arrived earliest in queue into memory
                add_to_mem_array(p, start);
                add_to_mem_list(p);
                return 1;
            }
        } 
    }

    return -1; //proccesses in wait queue but no hole large enough to fit them
}


/**
 * remove contents of swapped out process from memory
 * @param p process to be swapped out
 */
void remove_from_mem_array(Process *p) {
    int i = 0;
    while (memory[i++] != p->id[0]) {} //iterate until memory[i] = id of process to be swapped out
    --i;
    while (memory[i] == p->id[0]) memory[i++] = '\0';
}


/**
 * swap process out of memory and load into queue
 * @param p_head head in queue (first node in Process list)
 */
void swap_out(Process **p_head) {
    Process *p;

    p = remove_from_mem_list();
    remove_from_mem_array(p); //set corresponding mem array elements to '\0'
    p->times_swapped++;
    if (p->times_swapped < 3) { 
        add_to_queue(p, p_head);
    } else { //assume process has been run to completion
        free(p);
        p = NULL;
    }
}

/**
 * get num array elements in use by processes in memory
 * @return num occupied elements in memory array
 */
int get_mem_usage() {
    int in_use = 0;

    for (int a = 0; a < 128; a++) {
        if (memory[a] != '\0') in_use++;
    }

    return in_use;
}


/**
 * get number of holes in memory array
 * @return num holes in memory
 */
int get_num_holes() {
    int no_hole = 1; //if set to 1, no hole present
    int num_holes = 0;

    for (int i = 0; i < 128; i++) {
        if (no_hole) {
            if (memory[i] == '\0') no_hole = 0;
        } else if (memory[i] != '\0') {
            no_hole = 1;
            num_holes++;
        }
    }
    if (no_hole == 0) num_holes++;

    return num_holes;
}


/**
 * get number of processes stored in memory array
 * @return num processes in memory
 */
int get_num_procs() {
    int diff_proc = 0;
    char id1, id2;
    int num_procs = 0;

    if (mem_list != NULL) {
        num_procs = 1; //there is at least one process in memory
        Process *itr = mem_list;
        while (itr->next != NULL) {
            num_procs++;
            itr = itr->next;
        }
    }

    return num_procs;
}


void print_mem_list() {
    Process *itr = mem_list;
    int line_split = 0, i = 0;

    printf("mem list (traversing from top to bottom):\n");
    if (itr == NULL) {
        printf("EMPTY!!!\n");
        return;
    }
    while (itr->next != NULL) {
        line_split++;
        if (itr->id[0] != '\0') printf("%d. %c ", ++i, itr->id[0]);
        else printf("%d. 0 ", ++i);
        if (line_split % 4 == 0) printf("\n");
        itr = itr->next;
    }
    printf("\n");
}


void print_memory() {
    printf("memory array (traversing from start to end):\n");
    for (int i = 0; i < 128; i++) {
        if (memory[i] != '\0') printf("%d. %c ", i+1, memory[i]);
        else printf("%d. 0 ", i+1);
        if (i % 4 == 0) printf("\n");
    }
    printf("\n");
}


/**
 * first fit algorithm for filling processes into holes in memory
 * @param p_head head in queue (first node in Process list)
 */
void firstFit(Process **p_head) {
    int p_loaded = 0; //acts as a boolean
    int num_p_loads = 0; //num times processes were loaded into memory
    char p_id = '\0';
    int num_holes = 0, num_procs = 0; //in memory
    float cur_usage = 0, total_usage = 0; //memory usage

    /*
    loop until all processes are loaded in memory and/or 
    no more processes could be added to memory without swapping others out
    */
    p_loaded = add_to_memory(p_head);
    while (p_loaded != 0) {
        num_p_loads++;
        while (p_loaded == -1) {
            swap_out(p_head); //swap a process out of memory and into queue
            p_loaded = add_to_memory(p_head); //load first process in queue (that could not be loaded initially) into memory
            // printf("pid: %d\n", p_loaded);
            // print_mem_list();
            // print_memory();
        }

        // printf("pid: %d\n", p_loaded);
        // print_mem_list();
        // print_memory();
        
        p_id = get_pid();
        num_holes = get_num_holes();
        num_procs = get_num_procs();
        cur_usage = (get_mem_usage()/128.0f) * 100;
        total_usage += (get_mem_usage()/128.0f) * 100;

        printf("%c loaded, #processes = %d, #holes = %d, %%memusage = %.1f, cumulative %%mem = %.1f\n",
                p_id, num_procs, num_holes, (cur_usage), (total_usage/num_p_loads));
        p_loaded = add_to_memory(p_head);
    }
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
