#ifndef APP_H
#define APP_H


#include "app_lib.h"

#define MAX_ERROR 256
#define MAX_SLAVES 10
#define MAX_SIZE 256
#define MAX(a,b) ((a)>(b)? (a):(b))

void create_all_slaves(int cant_slaves, int (*slaves_fd)[2], char *slave_params[]);
void send_file(int fd, char * filename, int cant_files, int cant_files_sent);
void ready_select(int max_fd, fd_set * read_fds);

#endif

