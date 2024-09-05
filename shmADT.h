#ifndef _shmADT_H
#define _shmADT_H

#include <stdio.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>

typedef struct shmCDT * shmADT;


shmADT new_shm(const char * shm_name);
int write_shm(shmADT shm);
int read_shm(shmADT shm);
void free_shm(shmADT shm);

#endif