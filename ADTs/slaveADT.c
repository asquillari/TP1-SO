// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "slaveADT.h"

static void create_all_slaves(slaveADT sm);
static int ready_select(int max_fd, fd_set *read_fds);
static int send_file(int fd, const char *filename);
static void close_pipes(slaveADT sm);
static int slave();
static void start_slave(char * path, char * params[]);
static int create_pipe(int * pipe_fd);

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

    slaves->pipes = malloc(sizeof(pipesADT) * slaves->cant_slaves);
    if (slaves->pipes == NULL) {
        return NULL;
    }
    for (int i = 0; i < slaves->cant_slaves; i++) {
        slaves->pipes[i] = malloc(sizeof(pipesCDT));
        if (slaves->pipes[i] == NULL) {
            return NULL;
        }
    }

    FD_ZERO(&slaves->readFds);

    if(create_all_slaves(slaves) == ERROR){
        return NULL;
    }
    return slaves;
}

static int create_all_slaves(slaveADT sm){
    int i;
    for(i=0; i< sm->cant_slaves; i++){
        int pid;

        if(create_pipe(sm->pipes[i]->master_slave) == NULL){
            return ERROR;
        }

        if(create_pipe(sm->pipes[i]->slave_master) == NULL){
            return ERROR;
        }

        pid = slave();

        if(pid == 0){
            close_pipes(sm);
            close(STDIN_FILENO);
            dup2(sm->pipes[i]->master_slave[0], STDIN_FILENO);
            close(STDOUT_FILENO);
            dup2(sm->pipes[i]->slave_master[1], STDOUT_FILENO);
            close(sm->pipes[i]->master_slave[1]);
            close(sm->pipes[i]->slave_master[0]);
            char * params[] = {"./slave", NULL};
            start_slave(SLAVE_PATH, params);            //para que chota hace esto
        }
        close(sm->pipes[i]->master_slave[0]);
        close(sm->pipes[i]->slave_master[1]);
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
            if(send_file(sm->pipes[i]->master_slave[1], sm->files[sm->cant_files_sent]) == NULL){
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

    if(ready_select(max_fd, &read_fds) == NULL){
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
        if(send_file(sm->pipes[sindex]->master_slave[1], sm->files[sm->cant_files_sent]) == NULL){
            return ERROR;
        }
        sm->cant_files_sent++;
    }

    return bytes_read;
}

static int ready_select(int max_fd, fd_set *read_fds) {
    if (select(max_fd + 1, read_fds, NULL, NULL, NULL) == -1) {
        return NULL;
    }
}

void has_read(slaveADT sm){
    sm->cant_files_read++;
}

static int send_file(int fd, const char *filename) {
    char input[MAX_SIZE];
    snprintf(input, MAX_SIZE, "%s", filename);  
    if(write(fd, input, strlen(input)) == -1){
        return NULL;
    }
}

int has_next_file(slaveADT sm){
    if (sm == NULL) {
        return 0;
    }
    return sm->cant_files_read < sm->cant_files; 
}

static void close_pipes(slaveADT sm){
    int i;
    for(i = 0; i < sm->cant_slaves; i++){
        if(sm->pipes[i] != NULL){
            close(sm->pipes[i]->master_slave[1]);
            close(sm->pipes[i]->slave_master[0]);
        }
    }
    return;
}

static int create_pipe(int * pipe_fd){
    if(pipe(pipe_fd) == -1){
        return NULL;
    }
}

static int slave(){
    int pid;
    if((pid=fork()) == -1){
        return NULL;
    }
    return pid;
}

static void start_slave(char * path, char * params[]){
    execve(path, params, 0);
    perror("Execve");
    exit(EXIT_FAILURE);
}

void free_slave(slaveADT sm) {
    if (sm == NULL) {
        return;
    }
    close_pipes(sm);
    for (int i = 0; i < sm->cant_slaves; i++) {
        if(sm->pipes[i] != NULL){
            free(sm->pipes[i]);
        }
    }
    
    free(sm->pipes);
    free(sm);
}
