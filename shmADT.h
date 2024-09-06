#ifndef _shmADT_H
#define _shmADT_H

#include <stdio.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <semaphore.h>

#define SEM_NAME "/semaphore_rdwr"

typedef struct shmCDT * shmADT;


static shmADT new_shm(const char * shm_name, int oflag, mode_t mode, int prot, int creator);
shmADT open_shm(const char * shm_name, int oflag, mode_t mode);
shmADT create_shm(const char * shm_name, int oflag, mode_t mode);
int write_shm(shmADT shm);
int read_shm(shmADT shm, char * buffer, size_t cant_bytes);
void free_shm(shmADT shm);

#endif