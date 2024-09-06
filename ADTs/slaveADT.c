// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "slaveADT.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/select.h>
#include <string.h>

static void create_all_slaves(slaveADT sm);
static void ready_select(int max_fd, fd_set *read_fds);
static void send_file(int fd, const char *filename);

typedef struct pipesCDT{
    int master_slave[2];
    int slave_master[2];
} pipesCDT;


typedef struct slaveCDT {
    // Files to process
    int cant_files;
    int cant_files_sent;
    int cant_files_read;
    char ** files;

    // Slave info
    int cant_slaves;
    pipesADT * pipes;
    fd_set readFds;

} slaveCDT;



typedef struct slave * slave_t;


slaveADT initialize_slaves(int cant_files, char ** files){
    slaveADT slaves = malloc(sizeof(slaveCDT));
    if (slaves == NULL) {
        perror("Malloc");
        exit(EXIT_FAILURE);
    }

    slaves->cant_files = cant_files;
    slaves->files = files;
    slaves->cant_files_sent = 0;
    slaves->cant_files_read = 0;
    slaves->cant_slaves = cant_files < MAX_SLAVES ? cant_files : MAX_SLAVES;

    slaves->pipes = malloc(sizeof(pipesADT) * slaves->cant_slaves);
    if (slaves->pipes == NULL) {
        perror("Malloc");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < slaves->cant_slaves; i++) {
        slaves->pipes[i] = malloc(sizeof(pipesCDT));
        if (slaves->pipes[i] == NULL) {
            perror("Malloc pipe element");
            exit(EXIT_FAILURE);
        }
    }

    FD_ZERO(&slaves->readFds);

    create_all_slaves(slaves);
    return slaves;
}


static void create_all_slaves(slaveADT sm){
    int i;
    for(i=0; i< sm->cant_slaves; i++){
        int pid;

        create_pipe(sm->pipes[i]->master_slave);
        create_pipe(sm->pipes[i]->slave_master);

        pid = slave();

        if(pid == 0){
            close(STDIN_FILENO);
            dup2(sm->pipes[i]->master_slave[0], STDIN_FILENO);
            close(STDOUT_FILENO);
            dup2(sm->pipes[i]->slave_master[1], STDOUT_FILENO);
            close(sm->pipes[i]->master_slave[1]);
            close(sm->pipes[i]->slave_master[0]);
            start_slave(SLAVE_PATH, NULL);
        }
        close(sm->pipes[i]->master_slave[0]);
        close(sm->pipes[i]->slave_master[1]);
    }
    return;
}

void send_first_files(slaveADT sm){
    int i;
    int initial_dist = (sm->cant_files*0.2) / sm->cant_slaves;
    if(initial_dist == 0){
        initial_dist = 1;
    }
    for(i=0; i<sm->cant_slaves; i++){
        for (int j = 1; j <= initial_dist && sm->cant_files_sent < sm->cant_files; j++){
            send_file(sm->pipes[i]->master_slave[1], sm->files[sm->cant_files_sent]);
            sm->cant_files_sent++;    
        }
    }
    return;
}


int read_from_slave(slaveADT sm, char * buffer){
    int max_fd = -1;
    fd_set read_fds;
    ssize_t bytes_read;
    int i;

    FD_ZERO(&read_fds); //limpio el set

    for(i = 0; i < sm->cant_slaves ; i++) {
        // agregamos los fds al set
        FD_SET(sm->pipes[i]->slave_master[0], &read_fds);
        max_fd = MAX(max_fd,sm->pipes[i]->slave_master[0]);
    }

    ready_select(max_fd, &read_fds);

    int sindex = -1;
    for(i = 0 ; i< sm->cant_slaves ; i++) {
        if(FD_ISSET(sm->pipes[i]->slave_master[0], &read_fds)){
            sindex = i;
        }
    }    
                      
    if(sindex == -1) {
        return EXIT_FAILURE;
    }

    bytes_read = read(sm->pipes[sindex]->slave_master[0], buffer, MAX_SIZE);
    buffer[bytes_read] = '\0';

    FD_CLR(sm->pipes[sindex]->master_slave[1], &sm->readFds);

    if(sm->cant_files_sent < sm->cant_files){
        send_file(sm->pipes[sindex]->master_slave[1], sm->files[sm->cant_files_sent]);
        sm->cant_files_sent++;
    }

    return bytes_read;
}

static void ready_select(int max_fd, fd_set *read_fds) {
    if (select(max_fd + 1, read_fds, NULL, NULL, NULL) == -1) {
        perror("Select");
        exit(1);
    }
}

void has_read(slaveADT sm){
    sm->cant_files_read++;
}

static void send_file(int fd, const char *filename) {
    char input[MAX_SIZE];
    snprintf(input, MAX_SIZE, "%s", filename);  
    write(fd, input, strlen(input));
}

int has_next_file(slaveADT sm){
    if (sm == NULL) {
        return 0;
    }
    return sm->cant_files_read < sm->cant_files; 
}

void close_pipes(slaveADT sm){
    int i;
    for(i=0; i<sm->cant_slaves; i++){
        close(sm->pipes[i]->master_slave[0]);
        close(sm->pipes[i]->master_slave[1]);
        close(sm->pipes[i]->slave_master[0]);
        close(sm->pipes[i]->slave_master[1]);
    }
    exit(EXIT_SUCCESS);
}

void create_pipe(int * pipe_fd){
    if(pipe(pipe_fd) == -1){
        perror("Pipe");
        exit(EXIT_FAILURE);
    }
}

int slave(){
    int pid;
    if((pid=fork()) == -1){
        perror("Fork");
        exit(EXIT_FAILURE);
    }
    return pid;
}

void start_slave(char * path, char * params[]){
    execve(path, params, 0);
    perror("Execve");
    exit(EXIT_FAILURE);
}

void free_slave(slaveADT sm){
    for(int i = 0; i < sm->cant_slaves; i++){
        free(sm->pipes[i]);
    }
    free(sm->pipes);
    for(int i = 0; i < sm->cant_files; i++){
        free(sm->files[i]);
    }
    free(sm->files);
    free(sm);
}
