// This is a personal academic project. Deead_from_slaar PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "ADTs/slaveADT.h"
#include "ADTs/shmADT.h"

static void exit_failure(const char *msg);

int main(int argc, char *argv[]){
    //esto hay que modilarizarlo todo y hacer capaz una clase
    if (argc == 1) {
        char buffer_error[MAX_ERROR];
        snprintf(buffer_error, MAX_ERROR, "Error. Use: %s 'file' 'file' ...", argv[0]);
        exit_failure(buffer_error); 
    }

    int cant_files=argc-1;
    char ** files = &argv[1];

    shmADT shm = create_shm("CONNECT_SHM");
    if (shm == NULL) {
        exit_failure("Error creating shared memory");
    }

    printf("%s\n%d\n", "CONNECT_SHM", cant_files);
    fflush(stdout);
    
    slaveADT sm = initialize_slaves(cant_files, files);
    if (sm == NULL) {
        destroy_shm(shm);
        exit_failure("Error creating slave manager");
    }

    sleep(2);

    if(send_first_files(sm) < 0){
        free_slave(sm);
        destroy_shm(shm);
        exit_failure("Error sending files to slaves");
    }

    FILE * result_file = fopen("result.txt", "w"); 
    if (result_file == NULL) {
        destroy_shm(shm);
        free_slave(sm);
        exit_failure("Error opening result file");  
    }

    while(has_next_file(sm)){
        char buffer[MAX_SIZE] = {0};
        int bytes_read = read_from_slave(sm, buffer);
        if(bytes_read < 0){
            free_slave(sm);
            destroy_shm(shm);
            exit_failure("Read");
        }
        has_read(sm);
        if(fprintf(result_file, "%s", buffer) == 0){
            free_slave(sm);
            destroy_shm(shm);
            exit_failure("Write");
        }
        
        if(write_shm(shm, buffer, MAX_SIZE) < 0){
            free_slave(sm);
            destroy_shm(shm);
            exit_failure("Write");
        }
        
    }

    fclose(result_file);
    free_slave(sm);
    destroy_shm(shm);

    return 0;
    
}

static void exit_failure(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}
