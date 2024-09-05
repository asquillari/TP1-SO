// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "include/view.h"
#include "shmADT.h"

int main(int argc, char * argv[]){

    int cant_files = 0;
    char * shm_name;
    char * cant_files_str;
    //shmADT shm = NULL;

    if(argc == 1){ //me lo mandan por stdin el nombre
        
        shm_name = NULL;
        size_t len = 0;
        getline(&shm_name, &len, stdin);

        len = 0;
        getline(&cant_files_str, &len, stdin);
        cant_files = atoi(cant_files_str);

        if (shm_name != NULL && cant_files_str != NULL) {
            shm_name[strcspn(shm_name, "\n")] = '\0';
            cant_files_str[strcspn(cant_files_str, "\n")] = '\0';
        } else {
            exit_failure("couldnt read from stdin");
        }
        shmADT shm = new_shm(shm_name);

    } else if(argc == 3){ //me lo mandan por argumentos
        shmADT shm = new_shm(argv[1]);
        cant_files = atoi(argv[2]);

    } else {
        exit_failure("invalid arguments");
    }

    //leer y escribir en stdin

}

static void exit_failure(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}