// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "shmADT.h"

#define ERROR -1
#define MAX_SHM_SIZE 1024

typedef struct shmCDT {

    const char * shm_name;
    //faltan semaforos

    char * address;
    int write_offset;
    int read_offset;

    size_t size;
    int fd;

} shmCDT;

shmADT new_shm(const char * shm_name, int oflag, mode_t mode) {
    if(shm_name == NULL) {
        errno = EINVAL;
        return NULL;
    }

    shmADT new = malloc(sizeof(shmCDT));
    if(new == NULL) {
        return NULL;
    }

    new->shm_name = shm_name;

    new->fd = shm_open(new->shm_name, oflag, mode);
    if(new->fd == ERROR) {
        free(new);
        return NULL;
    }

    new->write_offset = 0;
    new->read_offset = 0;
    new->size = MAX_SHM_SIZE;

    if (ftruncate(new->fd, new->size) == ERROR) {
        freePshm(new);
        return NULL;
    }

    int prot = PROT_READ;
    if (oflag & O_RDWR) {
        prot = prot | PROT_WRITE; 
    }

    new->address = mmap(NULL, new->size, prot, MAP_SHARED, new->fd, 0);
    if(new->address == MAP_FAILED) {
        freePshm(new);
        return NULL;
    }
    
    return new;
}

shmADT open_shm(const char * shm_name, int oflag, mode_t mode) {
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
