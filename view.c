// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "include/view.h"
#include "ADTs/shmADT.h"

static void exit_failure(const char *msg);

int main(int argc, char * argv[]){

    int cant_files = 0;
    char * shm_name;
    char * cant_files_str;
    shmADT shm = NULL;

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
        shm = open_shm(shm_name, O_RDWR, S_IRUSR | S_IWUSR);

    } else if(argc == 3){ //me lo mandan por argumentos
        shm = open_shm(argv[1], O_RDWR, S_IRUSR | S_IWUSR);
        cant_files = atoi(argv[2]);

    } else {
        exit_failure("invalid arguments");
    }

    if (shm == NULL) {
        exit_failure("error creating shm");
    }

    if (cant_files < 0) {
        free_shm(shm);
        exit_failure("Error retrieving fileQty");
    }

    char buffer[BUFFER_SIZE] = {0};

    while(cant_files > 0){
        int bytesRead = read_shm(shm, buffer, BUFFER_SIZE);
        if (bytesRead == -1) {
            free_shm(shm);
            exit_failure("Error reading pshm");
        }
        printf("%s", buffer);
        cant_files--;
    }
    
    free_shm(shm);
    return 0;

}

static void exit_failure(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}