// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "shmADT.h"


typedef struct shmCDT {
    const char * shm_name;
    

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
