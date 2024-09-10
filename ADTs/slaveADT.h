#ifndef SLAVEADT_H
#define SLAVEADT_H

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/select.h>
#include <string.h>

#define READ 0
#define WRITE 1
//#define EXIT_FAILURE -1

#define MAX_SIZE 256
#define MAX_SLAVES 3
#define SLAVE_PATH "./slave"

#define MAX(a,b) ((a)>(b)? (a):(b))


typedef struct slaveCDT * slaveADT;
typedef struct pipesCDT * pipesADT;

slaveADT initialize_slaves(int cant_files, char ** files);
void send_first_files(slaveADT sm);
int read_from_slave(slaveADT sm, char * buffer);
void has_read(slaveADT sm);
int has_next_file(slaveADT sm);
void create_pipe(int * pipe_fd);
int slave();
void start_slave(char * path, char * params[]);
void free_slave(slaveADT sm);

#endif
