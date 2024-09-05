#ifndef _shmADT_H
#define _shmADT_H

#include <stdio.h>

typedef struct shmCDT * shmADT;


shmADT new_shm(const char * shm_name);
int write_shm(shmADT shm);
int read_shm(shmADT shm);
void free_shm(shmADT shm);

#endif