#ifndef APP_H
#define APP_H

#include <stdio.h> 
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <sys/select.h>
#include <signal.h>

#include "app_lib.h"

#define MAX_ERROR 256
#define MAX_SLAVES 3
#define MAX_SIZE 256
#define MAX(a,b) ((a)>(b)? (a):(b))

void create_all_slaves(int cant_slaves, int (*master_slave)[2], int (*slave_master)[2],char * slave_params[]);
void send_file(int fd, char * filename);
void ready_select(int max_fd, fd_set read_fds);
void close_pipes(int (*master_slave)[2], int (*slave_master)[2], int cant_slaves, int exit_status);

#endif

