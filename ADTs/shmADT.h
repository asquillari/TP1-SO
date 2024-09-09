#ifndef _shmADT_H
#define _shmADT_H

#include <stdio.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <stdlib.h>
#include <unistd.h>

#define SEM_NAME_RDWR "/semaphore_rdwr"
#define SEM_NAME_MUTEX "/semaphore_mutex"

typedef struct shmCDT * shmADT;


shmADT open_shm(const char * shm_name);
shmADT create_shm(const char * shm_name);
int write_shm(shmADT shm, const char * buffer, size_t cant_bytes);
int read_shm(shmADT shm, char * buffer, size_t cant_bytes);
void close_shm(shmADT shm);
void destroy_shm(shmADT shm);

#endif