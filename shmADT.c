// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "shmADT.h"


typedef struct shmCDT {

    const char * shm_name;
    //faltan semaforos

    char * address;
    int write_offset;
    int read_offset;

    size_t size;
    int fd;

} shmCDT;

shmADT new_shm(const char * shm_name) {
    return NULL;
}

int write_shm(shmADT shm) {
    return 0;
}

int read_shm(shmADT shm) {
    return 0;
}

void free_shm(shmADT shm) {
    return;
}
