#include <stdio.h> 
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "app.h"

int main(int argc, char *argv[]){
    //esto hay que modilarizarlo todo y hacer capas una clase
    if (argc == 1) {
        char buffer_error[MAX_ERROR];
        snprintf(buffer_error, MAX_ERROR, "Error. Use: %s 'file' 'file' ...", argv[0]);
        printf("%s\n", buffer_error);
        exit(EXIT_FAILURE); //siento que esto esta mal, ademas lo modularizaria
    }

    int cant_files=argc-1;
    int cant_slaves = cant_files < MAX_SLAVES ? cant_files : MAX_SLAVES;
    int slaves_fd[2][cant_slaves];   //vamos a tener 2 pipes por cada slave
    char * slave_params[] = { "./slave", NULL };

    int initial_dist = (cant_files*0.2) / cant_slaves;

    if(initial_dist == 0){
        initial_dist = 1;
    }

    create_all_slaves(cant_slaves, &slaves_fd, slave_params);

    int result_fd = open("result.txt", O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if(result_fd == -1){
        perror("Error opening result file");
        exit(EXIT_FAILURE);
    }

    int i;
    //mandamos la primera cantidad de archivos a cada uno
    for(i=0; i<cant_slaves; i++){
        for (int j = 0; i < initial_dist; i++)
        {
            send_file();
            cant_files--;
        }
    }

    while(cant_files != 0){
        for (i = 0; i < cant_slaves; i++){
            send_file();
            cant_files--;
        }
    }
    
}

void create_all_slaves(int cant_slaves, int ** slaves_fd,char * slave_params[]){
    int i;
    for(i=0; i<cant_slaves; i++){
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
