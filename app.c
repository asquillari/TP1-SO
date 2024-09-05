// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "include/app.h"
#include "include/app_lib.h"
#include "ADTs/slaveADT.h"

int main(int argc, char *argv[]){
    //esto hay que modilarizarlo todo y hacer capaz una clase
    if (argc == 1) {
        char buffer_error[MAX_ERROR];
        snprintf(buffer_error, MAX_ERROR, "Error. Use: %s 'file' 'file' ...", argv[0]);
        printf("%s\n", buffer_error);
        exit(EXIT_FAILURE); //siento que esto esta mal, ademas lo modularizaria
    }


    int cant_files=argc-1;
    char ** files = &argv[1];

    /*
    int cant_files_read = 0;
    int cant_files_sent = 0;
    int cant_slaves = cant_files < MAX_SLAVES ? cant_files : MAX_SLAVES;
    int master_slave[cant_slaves][2];
    int slave_master[cant_slaves][2];
    char * slave_params[] = { "./slave", NULL }; 
    
    int initial_dist = (cant_files*0.2) / cant_slaves;

    if(initial_dist == 0){
        initial_dist = 1;
    }
    */

    slaveADT sm = initialize_slaves(cant_files, files);

    send_first_files(sm);

    /*create_all_slaves(cant_slaves, master_slave, slave_master, slave_params);
    
    int i;
    //mandamos la primera cantidad de archivos a cada uno
    for(i=0; i<cant_slaves; i++){
        for (int j = 1; j <= initial_dist && cant_files_sent < cant_files; j++){
            cant_files_sent++; 
            send_file(master_slave[i][1], argv[cant_files_sent]);   
        }
    }

    */
    // creamos el file de resultado 
    FILE * result_file = fopen("result.txt", "w"); 
    if (result_file == NULL) {
        perror("Error opening result file");
        exit(EXIT_FAILURE);  
    }

    while(has_next_file(sm)){//con pocos archivos no entra nunca
        char buffer[MAX_SIZE] = {0};
        int bytes_read = read_from_slave(sm, buffer);

        if(bytes_read < 0){
            perror("Read");
            free(sm);
            exit(EXIT_FAILURE);
        }
        fflush(result_file);
        fprintf(result_file, "%s", buffer);
        
        //fijarse que onda el error

    }

    //cerramos el archivo
    fclose(result_file);

    close_pipes(sm);

    free(sm);

    return 0;
    
}

/*
void close_pipes(int (*master_slave)[2], int (*slave_master)[2], int cant_slaves, int exit_status){
    int i;
    for(i=0; i<cant_slaves; i++){
        close(master_slave[i][0]);
        close(master_slave[i][1]);
        close(slave_master[i][0]);
        close(slave_master[i][1]);
    }
    exit(exit_status);
}



void create_all_slaves(int cant_slaves, int (*master_slave)[2], int (*slave_master)[2],char * slave_params[]){
    int i;
    for(i=0; i<cant_slaves; i++){
        int pid;

        create_pipe(master_slave[i]);
        create_pipe(slave_master[i]);

        pid = slave();

        if(pid == 0){
            close(STDIN_FILENO);
            dup2(master_slave[i][0], STDIN_FILENO);
            close(STDOUT_FILENO);
            dup2(slave_master[i][1], STDOUT_FILENO);
            close(master_slave[i][1]);
            close(slave_master[i][0]);
            start_slave("./slave", slave_params);
        }
        close(master_slave[i][0]);
        close(slave_master[i][1]);
    }
}

void send_file(int fd, const char *filename) {
    char input[MAX_SIZE];
    snprintf(input, MAX_SIZE, "%s", filename);  
    write(fd, input, strlen(input));
}

void ready_select(int max_fd, fd_set *read_fds) {
    if (select(max_fd + 1, read_fds, NULL, NULL, NULL) == -1) {
        perror("Select");
        exit(1);
    }
}
*/
