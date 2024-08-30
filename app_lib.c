#include "app_lib.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


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

void set_slave_pipes(int * pipe_in, int * pipe_out){
    close(pipe_in[1]);
    dup2(pipe_in[0], STDIN_FILENO);
    close(pipe_out[0]);
    dup2(pipe_out[1], STDOUT_FILENO);
    close(pipe_in[0]);
    close(pipe_out[1]);
}

void start_slave(char * path, char * params[]){
    execve(path, params, 0);
    perror("Execve");
    exit(EXIT_FAILURE);
}