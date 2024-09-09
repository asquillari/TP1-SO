// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "shmADT.h"

#define ERROR -1
#define SHM_SIZE 1024
#define END_OF_LINE '\n'


typedef struct shmCDT {

    const char * shm_name;
    const char * sem_name;

    sem_t * sem_readwrite;

    char * address;
    int write_offset;
    int read_offset;

    int fd;

} shmCDT;


shmADT open_shm(const char * shm_name) {
    if(shm_name == NULL) {
        return NULL;
    }
    shmADT shm = malloc(sizeof(shmCDT));
    if(shm == NULL) {
        return NULL;
    }

    shm->shm_name = shm_name;
    shm->sem_name = SEM_NAME;
    shm->write_offset = 0;
    shm->read_offset = 0;

    if((shm->fd = shm_open(shm->shm_name, O_RDWR, S_IWUSR | S_IRUSR)) == ERROR) {
        close_shm(shm);
        return NULL;
    }
    if((shm->address = mmap(NULL, SHM_SIZE, PROT_READ, MAP_SHARED, shm->fd, 0)) == MAP_FAILED) {
        close_shm(shm);
        return NULL;
    }
    if((shm->sem_readwrite = sem_open(shm->sem_name, 0, S_IWUSR | S_IRUSR, 0)) == SEM_FAILED) {
        close_shm(shm);
        return NULL;
    }

    return shm;
}

shmADT create_shm(const char * shm_name) {
    if(shm_name == NULL) {
        return NULL;
    }
    shmADT shm = malloc(sizeof(shmCDT));
    if(shm == NULL) {
        return NULL;
    }

    shm->shm_name = shm_name;
    shm->sem_name = SEM_NAME;
    shm->write_offset = 0;
    shm->read_offset = 0;

    if((shm->fd = shm_open(shm->shm_name, O_CREAT | O_RDWR, S_IWUSR | S_IRUSR)) == ERROR) {
        destroy_shm(shm);
        return NULL;
    }
    if(ftruncate(shm->fd, SHM_SIZE) == ERROR) {
        destroy_shm(shm);
        return NULL;
    }
    if((shm->address = mmap(NULL, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm->fd, 0)) == MAP_FAILED) {
        destroy_shm(shm);
        return NULL;
    }
    if((shm->sem_readwrite = sem_open(shm->sem_name, O_CREAT, S_IWUSR | S_IRUSR, 0)) == SEM_FAILED) {
        destroy_shm(shm);
        return NULL;
    }

    return shm;
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

        if (shm->address[shm->read_offset] == END_OF_LINE) {
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

void close_shm(shmADT shm) {
    if(shm == NULL) {
        return;
    }

    munmap(shm->address, SHM_SIZE);
    sem_close(shm->sem_readwrite);
    close(shm->fd);
    free(shm);
}

void destroy_shm(shmADT shm) {
    if(shm == NULL) {
        return;
    }

    munmap(shm->address, SHM_SIZE);
    shm_unlink(shm->shm_name);
    sem_unlink(shm->sem_name);
    free(shm);
}
