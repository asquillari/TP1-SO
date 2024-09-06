// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "shmADT.h"

#define ERROR -1
#define SHM_SIZE 1024

typedef struct shmCDT {

    const char * shm_name;
    const char * sem_name;

    sem_t * sem_readwrite;

    char * address;
    int write_offset;
    int read_offset;

    int fd;

} shmCDT;

static shmADT new_shm(const char * shm_name, int oflag, mode_t mode, int prot, int creator) {
    if(shm_name == NULL) {
        errno = EINVAL;
        return NULL;
    }

    shmADT new = malloc(sizeof(shmCDT));
    if(new == NULL) {
        return NULL;
    }

    new->shm_name = shm_name;
    new->sem_name = SEM_NAME;

    new->fd = shm_open(new->shm_name, oflag, mode);
    if(new->fd == ERROR) {
        free(new);
        return NULL;
    }

    new->write_offset = 0;
    new->read_offset = 0;

    if (creator){
        if (ftruncate(new->fd, SHM_SIZE) == ERROR) {
            freePshm(new);
            return NULL;
        }
        new->sem_readwrite = sem_open(new->sem_name, O_CREAT, S_IRUSR | S_IWUSR, 1);
    } else {
        new->sem_readwrite = sem_open(new->sem_name, 0);
    }
    
    new->address = mmap(NULL, SHM_SIZE, prot, MAP_SHARED, new->fd, 0);
    if(new->address == MAP_FAILED) {
        freePshm(new);
        return NULL;
    }
    
    return new;
}

shmADT open_shm(const char * shm_name, int oflag, mode_t mode) {
    return new_shm(shm_name, oflag, mode, PROT_READ, 0);
}

shmADT create_shm(const char * shm_name, int oflag, mode_t mode) {
    return new_shm(shm_name, oflag, mode, PROT_READ | PROT_WRITE, 1);
}

int write_shm(shmADT shm) {
    return 0;
}

int read_shm(shmADT shm, char * buffer, size_t cant_bytes) {
    return 0;
}

void free_shm(shmADT shm) {
    if(shm == NULL) {
        return;
    }

    munmap(shm->address, SHM_SIZE);
    close(shm->fd);

    shm_unlink(shm->shm_name);
    sem_unlink(shm->sem_name);

    sem_close(shm->sem_readwrite);

    free(shm);
    return;
}
