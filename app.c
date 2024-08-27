#include <stdio.h> 
#include <stdlib.h>

#define MAX_ERROR 256
#define MAX_SLAVES 10

int main(int argc, char *argv[]){
    //esto hay que modilarizarlo todo y hacer capas una clase
    if (argc == 1) {
        char buffer_error[MAX_ERROR];
        snprintf(buffer_error, MAX_ERROR, "Error. Use: %s 'file' 'file' ...", argv[0]);
        printf("%s\n", buffer_error);
        exit(EXIT_FAILURE); //siento que esto esta mal, ademas lo modularizaria
    }

    int cant_files=argc-1;
    int num_slaves = cant_files < MAX_SLAVES ? cant_files : MAX_SLAVES;
    int slaves_fd[2][num_slaves];   //vamos a tener 2 pipes por cada slave
    char * slave_params[] = { "./slave", NULL };

    int initial_dist = (cant_files*0.2) / num_slaves;

    if(initial_dist == 0){
        initial_dist = 1;
    }

    int i;
    for(i=0; i<num_slaves; i++){
        int pid;
        //Pipe para enviar los archivos
        int pipe_in[2];
        //Pipe para recibir los md5
        int pipe_out[2];

        create_pipe(pipe_in);
        create_pipe(pipe_out);

        //le guardo los fd de los pipes a cada slave
        slaves_fd[0][i] = pipe_in[0];
        slaves_fd[1][i] = pipe_out[1];

        pid = slave();

        if(pid == 0){
            int j;
            for(j = 0; j<i; j++){
                close(slaves_fd[0][j]);
                close(slaves_fd[1][j]);
            }
           set_slave_pipes(pipe_in, pipe_out);
           start_slave("./slave", slave_params);
        }
        close(pipe_in[0]);
        close(pipe_out[1]);
    }
}