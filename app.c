// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "app.h"
#include "app_lib.h"

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
    int master_slave[cant_slaves][2];
    int slave_master[cant_slaves][2];
    char * slave_params[] = { "./slave", NULL }; 

    int initial_dist = (cant_files*0.2) / cant_slaves;

    if(initial_dist == 0){
        initial_dist = 1;
    }

    create_all_slaves(cant_slaves, master_slave, slave_master, slave_params);
    
    int i;
    //mandamos la primera cantidad de archivos a cada uno
    for(i=0; i<cant_slaves; i++){
        for (int j = 1; j <= initial_dist && cant_files_sent < cant_files; j++){
            cant_files_sent++; 
            send_file(master_slave[i][1], argv[cant_files_sent]);   
        }
    }


    // creamos el file de resultado 
    FILE * result_file = fopen("result.txt", "w"); 
    if (result_file == NULL) {
        perror("Error opening result file");
        exit(EXIT_FAILURE);  
    }

    int max_fd = -1;
    fd_set read_fds;


    while(cant_files_read < cant_files){
        FD_ZERO(&read_fds); //limpio el set
        for(i = 0; i<cant_slaves; i++) {
            // agregamos los fds al set
            FD_SET(slave_master[i][0], &read_fds);
            max_fd = MAX(max_fd,slave_master[i][0]);
        }
        ready_select(max_fd, &read_fds);

        for(i = 0; i<cant_slaves; i++) {
            if(FD_ISSET(slave_master[i][0], &read_fds)){
                char buffer[MAX_SIZE];
                ssize_t bytes_read;
                bytes_read = read(slave_master[i][0], buffer, MAX_SIZE);
                if(bytes_read < 0){
                    perror("Read");
                    exit(EXIT_FAILURE);
                }
                cant_files_read++;
                if(cant_files_sent < cant_files){
                    send_file(master_slave[i][1], argv[cant_files_sent+1]);
                    cant_files_sent++;
                }
                buffer[bytes_read] = '\0';
                fprintf(result_file, "%s", buffer);
            }
        }


    }

    //cerramos el archivo
    fclose(result_file);

    close_pipes(master_slave, slave_master, cant_slaves, EXIT_SUCCESS);

    return 0;
    
}

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
    snprintf(input, MAX_SIZE, "%s\n", filename);  
    write(fd, input, strlen(input));
}

void ready_select(int max_fd, fd_set *read_fds) {
    if (select(max_fd + 1, read_fds, NULL, NULL, NULL) == -1) {
        perror("Select");
        exit(1);
    }
}
