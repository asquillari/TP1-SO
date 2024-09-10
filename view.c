// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "include/view.h"
#include "ADTs/shmADT.h"

static void exit_failure(const char *msg);

int main(int argc, char * argv[]){

     int cant_files = 0;
    char * shm_name = NULL;
    char * cant_files_str = NULL;
    shmADT shm = NULL;

    if (argc == 1) { 
        size_t len = 0;

        if (getline(&shm_name, &len, stdin) == -1) {
            exit_failure("Error al leer shm_name desde stdin");
        }
        shm_name[strcspn(shm_name, "\n")] = '\0'; 

        len = 0;
        if (getline(&cant_files_str, &len, stdin) == -1) {
            exit_failure("Error al leer cant_files desde stdin");
        }
        cant_files_str[strcspn(cant_files_str, "\n")] = '\0'; 
        cant_files = atoi(cant_files_str);

        if (shm_name == NULL || cant_files_str == NULL) {
            exit_failure("No se pudo leer desde stdin");
        }
        
        shm = open_shm(shm_name);

    } else if(argc == 3){ 
        shm = open_shm(argv[1]);
        cant_files = atoi(argv[2]);

    } else {
        exit_failure("invalid arguments");
    }

    if (shm == NULL) {
        exit_failure("error creating shm");
    }

    if (cant_files < 0) {
        close_shm(shm);
        exit_failure("error retrieving cant_files");
    }

    char buffer[BUFFER_SIZE] = {0};

    while(cant_files > 0){
        int bytesRead = read_shm(shm, buffer, BUFFER_SIZE);
        if (bytesRead == -1) {
            close_shm(shm);
            exit_failure("Error reading shm");
        }
        printf("%s", buffer);
        cant_files--;
    }
    putchar('\n');
    
    close_shm(shm);
    free(shm_name);
    free(cant_files_str);
    return 0;

}

static void exit_failure(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}