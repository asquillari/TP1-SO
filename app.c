// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "include/app.h"
#include "include/app_lib.h"
#include "ADTs/slaveADT.h"
#include "ADTs/shmADT.h"

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

    shmADT shm = create_shm("CONNECT_SHM", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if (shm == NULL) {
        perror("Error creating shared memory");
        exit(EXIT_FAILURE);
    }

    printf("%s\n%d\n", "CONNECT_SHM", cant_files);

    slaveADT sm = initialize_slaves(cant_files, files);
    if (sm == NULL) {
        perror("Error creating slave manager");
        free_shm(shm);
        exit(EXIT_FAILURE);
    }

    sleep(2);

    send_first_files(sm);

    // creamos el file de resultado 
    FILE * result_file = fopen("result.txt", "w"); 
    if (result_file == NULL) {
        free_shm(shm);
        free_slave(sm);
        perror("Error opening result file");
        exit(EXIT_FAILURE);  
    }

    while(has_next_file(sm)){
        char buffer[MAX_SIZE] = {0};
        int bytes_read = read_from_slave(sm, buffer);
        if(bytes_read < 0){
            perror("Read");
            free_slave(sm);
            free_shm(shm);
            exit(EXIT_FAILURE);
        }
        has_read(sm);
        if(fprintf(result_file, "%s", buffer) == 0){
            perror("Write");
            free_slave(sm);
            free_shm(shm);
            exit(EXIT_FAILURE);
        }
        
        if(write_shm(shm, buffer, MAX_SIZE) < 0){
            perror("Write");
            free_slave(sm);
            free_shm(shm);
            exit(EXIT_FAILURE);
        }
        
        //fijarse que onda el error
    }

    fclose(result_file);
    close_pipes(sm);
    free_slave(sm);
    free_shm(shm);

    return 0;
    
}
