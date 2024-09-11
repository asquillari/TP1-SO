// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "shmADT.h"

#define SEM_NAME_BUFFER "/semaphore_buffer"
#define SEM_NAME_MUTEX "/semaphore_mutex"

typedef struct shmCDT {

    const char * shm_name;
    const char * sem_name_buffer;
    const char * sem_name_mutex;

    sem_t * sem_buffer;
    sem_t * sem_mutex;

    char * address;
    int write_offset;
    int read_offset;

    int fd;

} shmCDT;

static void handle_error(const char * msg);

shmADT open_shm(const char * shm_name) {
    if(shm_name == NULL) {
        return NULL;
    }
    shmADT shm = malloc(sizeof(shmCDT));
    if(shm == NULL) {
        return NULL;
    }

    shm->shm_name = shm_name;
    shm->sem_name_buffer = SEM_NAME_BUFFER;
    shm->sem_name_mutex = SEM_NAME_MUTEX;
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
    if((shm->sem_buffer = sem_open(shm->sem_name_buffer, 0, S_IWUSR | S_IRUSR, 0)) == SEM_FAILED) {
        close_shm(shm);
        return NULL;
    }
    if((shm->sem_mutex = sem_open(shm->sem_name_mutex, 0, S_IWUSR | S_IRUSR, 1)) == SEM_FAILED) {
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
    shm->sem_name_buffer = SEM_NAME_BUFFER;
    shm->sem_name_mutex = SEM_NAME_MUTEX;
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
    if((shm->sem_buffer = sem_open(shm->sem_name_buffer, O_CREAT, S_IWUSR | S_IRUSR, 0)) == SEM_FAILED) {
        destroy_shm(shm);
        return NULL;
    }
    if((shm->sem_mutex = sem_open(shm->sem_name_mutex, O_CREAT, S_IWUSR | S_IRUSR, 1)) == SEM_FAILED) {
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
    sem_wait(shm->sem_mutex);
    while (bytes_written < cant_bytes && buffer[bytes_written] != '\0') {
        shm->address[shm->write_offset++] = buffer[bytes_written];

        if (buffer[bytes_written] == END_OF_LINE) {
            bytes_written++;
            break;
        }

        bytes_written++;
    }
    sem_post(shm->sem_mutex);
    sem_post(shm->sem_buffer); 

    return bytes_written;
}

int read_shm(shmADT shm, char * buffer, size_t cant_bytes) {
    if(shm == NULL || cant_bytes == 0) {
        return ERROR;
    }

    sem_wait(shm->sem_buffer);
    sem_wait(shm->sem_mutex);
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
    sem_post(shm->sem_mutex);

    return bytes_read;
}

void close_shm(shmADT shm) {
    if(shm == NULL) {
        return;
    }

    if(munmap(shm->address, SHM_SIZE) < 0) {
        handle_error("munmap");
    }

    if(sem_close(shm->sem_buffer) < 0) {
        handle_error("sem_close");
    }

    if(sem_close(shm->sem_mutex) < 0) {
        handle_error("sem_close");
    }

    if(close(shm->fd) < 0) {
        handle_error("close");
    }
    
    free(shm);
}

void destroy_shm(shmADT shm) {
    if(shm == NULL) {
        return;
    }

    munmap(shm->address, SHM_SIZE);
    shm_unlink(shm->shm_name);
    if (shm->sem_buffer != NULL) {
        sem_close(shm->sem_buffer);
        sem_unlink(shm->sem_name_buffer);
    }

    if (shm->sem_mutex != NULL) {
        sem_close(shm->sem_mutex);
        sem_unlink(shm->sem_name_mutex);
    }
    
    free(shm);
}

static void handle_error(const char * msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}
