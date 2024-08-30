
#ifndef APP_LIB_H
#define APP_LIB_H

void create_pipe(int * pipe_fd);
int slave();
void set_slave_pipes(int * pipe_in, int * pipe_out);
void start_slave(char * path, char * params[]);

#endif
