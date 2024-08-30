#include <stdio.h> 
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "app.h"
#include "app_lib.h"
#include <sys/select.h>

int main(int argc, char *argv[]){
    //esto hay que modilarizarlo todo y hacer capas una clase
    if (argc == 1) {
        char buffer_error[MAX_ERROR];
        snprintf(buffer_error, MAX_ERROR, "Error. Use: %s 'file' 'file' ...", argv[0]);
        printf("%s\n", buffer_error);
        exit(EXIT_FAILURE); //siento que esto esta mal, ademas lo modularizaria
    }

    int cant_files=argc-1;
    int cant_files_read = 0;
    int cant_files_sent = 0;
    int cant_slaves = cant_files < MAX_SLAVES ? cant_files : MAX_SLAVES;
    int slaves_fd[2][cant_slaves];   //vamos a tener 2 pipes por cada slave
    char * slave_params[] = { "./slave", NULL };

    int initial_dist = (cant_files*0.2) / cant_slaves;

    if(initial_dist == 0){
        initial_dist = 1;
    }

    create_all_slaves(cant_slaves, slaves_fd, slave_params);

    int i;
    //mandamos la primera cantidad de archivos a cada uno
    for(i=0; i<cant_slaves; i++){
        for (int j = 0; j < initial_dist; j++){
            send_file(slaves_fd[1][i], argv[cant_files_sent], cant_files, cant_files_sent);//chequear si esta llamada funciona o si es 0
            cant_files_sent++;
        }
    }

    // creamos el file de resultado 
    FILE * result_file = fopen("result.txt", "w"); //hay que agregar manejo de error
    
    fd_set read_fds;
    int max_fd = 0;

    while(cant_files_read <= cant_files){
        FD_ZERO(&read_fds); //limpio el set
        for(i = 0; i<cant_slaves; i++) {
            // agregamos los fds al set
            FD_SET(slaves_fd[0][i], &read_fds);
            max_fd = MAX(max_fd,slaves_fd[0][i]);
        }

        ready_select(max_fd, &read_fds);

        for(i = 0; i<cant_slaves; i++) {
            if(FD_ISSET(slaves_fd[0][i], &read_fds)){
                char buffer[MAX_SIZE];
                int read_bytes = read(slaves_fd[0][i], buffer, MAX_SIZE);//manejo de error
                printf("buffer: %s\n", buffer);
                buffer[read_bytes] = '\0';          
                cant_files_read++;
                if(cant_files_sent < cant_files){
                    send_file(slaves_fd[1][i], argv[cant_files_sent+1], cant_files, cant_files_sent);
                    cant_files_sent++;
                }
                fprintf(result_file, "%s", buffer);
            }
        }


    }

    //cerramos el archivo
    fclose(result_file);

    return 0;
    
}

void create_all_slaves(int cant_slaves, int (*slaves_fd)[2],char * slave_params[]){
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

void send_file(int fd, char * filename, int cant_files, int cant_files_sent){
    //checks if there are no more files to send
    if (cant_files_sent == cant_files){
        return;
    }
    dprintf(fd, "%s\n",filename);
}

void ready_select(int max_fd, fd_set * read_fds) {
    if(select(max_fd + 1, read_fds, NULL, NULL, NULL) == -1) {
        perror("Select");
        exit(1); //despues hay que mejorar nuestro manejo de errores
    }
}