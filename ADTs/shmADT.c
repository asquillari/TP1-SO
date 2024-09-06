// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "shmADT.h"

#define ERROR -1
#define SHM_SIZE 1024
#define END_OF_LINE '\n'

static shmADT new_shm(const char * shm_name, int oflag, mode_t mode, int prot, int creator);

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
            free_shm(new);
            return NULL;
        }
        new->sem_readwrite = sem_open(new->sem_name, O_CREAT, S_IRUSR | S_IWUSR, 1);
    } else {
        new->sem_readwrite = sem_open(new->sem_name, 0);
    }
    
    new->address = mmap(NULL, SHM_SIZE, prot, MAP_SHARED, new->fd, 0);
    if(new->address == MAP_FAILED) {
        free_shm(new);
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

int write_shm(shmADT shm, const char * buffer, size_t cant_bytes) {
    if(shm == NULL || cant_bytes == 0 || buffer == NULL) {
        return ERROR;
    }

    size_t bytes_written = 0;

    while (bytes_written < cant_bytes && buffer[bytes_written] != '\0') {
        shm->address[shm->write_offset++] = buffer[bytes_written];

        if (buffer[bytes_written] == END_OF_LINE) {
            bytes_written++;
            break;
        }

        bytes_written++;
    }

    sem_post(shm->sem_readwrite); 

    return bytes_written;
}

int read_shm(shmADT shm, char * buffer, size_t cant_bytes) {
    if(shm == NULL || cant_bytes == 0) {
        return ERROR;
    }

    sem_wait(shm->sem_readwrite);
    size_t bytes_read = 0;
    while (bytes_read < cant_bytes) {
        buffer[bytes_read] = shm->address[shm->read_offset++];

        if (buffer[bytes_read] == END_OF_LINE) {
            bytes_read++;
            break;  
        }

        bytes_read++;
    }

    if (bytes_read < cant_bytes) {
        buffer[bytes_read] = '\0';
    }

    return bytes_read;
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
