// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "include/app_lib.h"
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

void start_slave(char * path, char * params[]){
    execve(path, params, 0);
    perror("Execve");
    exit(EXIT_FAILURE);
}