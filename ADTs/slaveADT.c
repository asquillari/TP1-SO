// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "slaveADT.h"

static int create_all_slaves(slaveADT sm);
static int ready_select(int max_fd, fd_set *read_fds);
static int send_file(int fd, const char *filename);
static int close_pipes(slaveADT sm);
static int slave();
static int start_slave(char * path, char * params[]);
static int create_pipe(int * pipe_fd);
static int wait_for_slaves(slaveADT sm);

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
    int * pids;
    fd_set readFds;

} slaveCDT;


slaveADT initialize_slaves(int cant_files, char ** files){
    slaveADT slaves = malloc(sizeof(slaveCDT));
    if (slaves == NULL) {
        return NULL;
    }

    slaves->cant_files = cant_files;
    slaves->files = files;
    slaves->cant_files_sent = 0;
    slaves->cant_files_read = 0;
    slaves->cant_slaves = cant_files < MAX_SLAVES ? cant_files : MAX_SLAVES;

    slaves->pids = malloc(sizeof(int) * slaves->cant_slaves);
    if (slaves->pids == NULL) {
        free(slaves);
        return NULL;
    }

    slaves->pipes = malloc(sizeof(pipesADT) * slaves->cant_slaves);
    if (slaves->pipes == NULL) {
        free(slaves->pids);
        free(slaves);
        return NULL;
    }
    for (int i = 0; i < slaves->cant_slaves; i++) {
        slaves->pipes[i] = malloc(sizeof(pipesCDT));
        if (slaves->pipes[i] == NULL) {
            for (int j = 0; j < i; j++) {
                free(slaves->pipes[j]);
            }
            free(slaves->pipes);
            free(slaves->pids);
            free(slaves);
            return NULL;
        }
        slaves->pipes[i]->master_slave[0] = -1;
        slaves->pipes[i]->master_slave[1] = -1;
        slaves->pipes[i]->slave_master[0] = -1;
        slaves->pipes[i]->slave_master[1] = -1;
    }

    FD_ZERO(&slaves->readFds);

    if(create_all_slaves(slaves) == ERROR){
        free_slave(slaves);
        return NULL;
    }
    return slaves;
}

static int create_all_slaves(slaveADT sm){
    int i;
    for(i=0; i< sm->cant_slaves; i++){
        int pid;

        if(create_pipe(sm->pipes[i]->master_slave) == ERROR){
            return ERROR;
        }

        if(create_pipe(sm->pipes[i]->slave_master) == ERROR){
            return ERROR;
        }

        pid = slave();
        sm->pids[i] = pid;

        if(pid == 0){
            if(close_pipes(sm) == ERROR){
                return ERROR;
            }
            if(close(STDIN_FILENO) == ERROR || close(STDOUT_FILENO) == ERROR){
                return ERROR;
            }
            if( dup2(sm->pipes[i]->master_slave[0], STDIN_FILENO) == ERROR || dup2(sm->pipes[i]->slave_master[1], STDOUT_FILENO) == ERROR){
                return ERROR;
            }
            char * params[] = {"./slave", NULL};
            if (start_slave(SLAVE_PATH, params) == ERROR){
                return ERROR;
            }
        }
        if(close(sm->pipes[i]->master_slave[0]) == ERROR || close(sm->pipes[i]->slave_master[1]) == ERROR){
            return ERROR;
        }
    }
    return 0;
}

int send_first_files(slaveADT sm){
    int i;
    int initial_dist = (sm->cant_files*0.2) / sm->cant_slaves;
    if(initial_dist == 0){
        initial_dist = 1;
    }
    for(i=0; i<sm->cant_slaves; i++){
        for (int j = 1; j <= initial_dist && sm->cant_files_sent < sm->cant_files; j++){
            if(send_file(sm->pipes[i]->master_slave[1], sm->files[sm->cant_files_sent]) == ERROR){
                return ERROR;
            }
            sm->cant_files_sent++;    
        }
    }
    return 0;
}


int read_from_slave(slaveADT sm, char * buffer){
    int max_fd = -1;
    fd_set read_fds;
    ssize_t bytes_read;
    int i;

    FD_ZERO(&read_fds); 

    for(i = 0; i < sm->cant_slaves ; i++) {
        FD_SET(sm->pipes[i]->slave_master[0], &read_fds);
        max_fd = MAX(max_fd,sm->pipes[i]->slave_master[0]);
    }

    if(ready_select(max_fd, &read_fds) == ERROR){
        return ERROR;
    }


    int sindex = -1;
    for(i = 0 ; i< sm->cant_slaves ; i++) {
        if(FD_ISSET(sm->pipes[i]->slave_master[0], &read_fds)){
            sindex = i;
        }
    }    
                      
    if(sindex == -1) {
        return ERROR;
    }

    bytes_read = read(sm->pipes[sindex]->slave_master[0], buffer, MAX_SIZE);
    buffer[bytes_read] = '\0';

    FD_CLR(sm->pipes[sindex]->master_slave[1], &sm->readFds);

    if(sm->cant_files_sent < sm->cant_files){
        if(send_file(sm->pipes[sindex]->master_slave[1], sm->files[sm->cant_files_sent]) == ERROR){
            return ERROR;
        }
        sm->cant_files_sent++;
    }

    return bytes_read;
}

static int ready_select(int max_fd, fd_set *read_fds) {
    if (select(max_fd + 1, read_fds, NULL, NULL, NULL) == ERROR) {
        return ERROR;
    }
    return 0;
}

void has_read(slaveADT sm){
    sm->cant_files_read++;
}

static int send_file(int fd, const char *filename) {
    char input[MAX_SIZE];
    snprintf(input, MAX_SIZE, "%s", filename);  
    if(write(fd, input, strlen(input)) == -1){
        return ERROR;
    }
    return 0;
}

int has_next_file(slaveADT sm){
    if (sm == NULL) {
        return 0;
    }
    return sm->cant_files_read < sm->cant_files; 
}

static int close_pipes(slaveADT sm) {
    int i;
    for (i = 0; i < sm->cant_slaves; i++) {
        if (sm->pipes[i] != NULL) {
            if (sm->pipes[i]->master_slave[1] != -1) {
                if(close(sm->pipes[i]->master_slave[1]) == -1){
                    return ERROR;
                }
            }
            if (sm->pipes[i]->slave_master[0] != -1) {
                if(close(sm->pipes[i]->slave_master[0]) == -1){
                    return ERROR;
                }
            }
        }
    }
    return 0;
}

static int create_pipe(int * pipe_fd){
    if(pipe(pipe_fd) == ERROR){
        return ERROR;
    }
    return 0;
}

static int slave(){
    int pid;
    if((pid=fork()) == -1){
        return ERROR;
    }
    return pid;
}

static int start_slave(char * path, char * params[]){
    execve(path, params, 0);
    return ERROR;
}

static int wait_for_slaves(slaveADT sm){
    for(int i = 0; i < sm->cant_slaves; i++){
        if(waitpid(sm->pids[i], NULL, 0) == ERROR){
            return ERROR;
        }
    }
    return 0;
}

void free_slave(slaveADT sm) {
    if (sm == NULL) {
        return;
    }
    if(close_pipes(sm) == ERROR){
        perror("Error closing pipes");
        exit(EXIT_FAILURE);
    }
    if(wait_for_slaves(sm) == ERROR){
        perror("Error waiting for slaves");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < sm->cant_slaves; i++) {
        if(sm->pipes[i] != NULL){
            free(sm->pipes[i]);
        }
    }
    free(sm->pids);    
    free(sm->pipes);
    free(sm);
}
