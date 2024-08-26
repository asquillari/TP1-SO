#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>

#define MAX_SIZE 256
#define MD5_LEN 32

#define FORMAT_OUTPUT "MD5: %s FILE: %s SLAVE_PID: %d\n"

int get_md5(char * file, char * result); //la pongo con int por si queremos mandar un error

int main(int argc, char * argv[]){
    
    char md5[MAX_SIZE];

    get_md5(argv[1], md5);
    fprintf(stdout, FORMAT_OUTPUT, md5, argv[1], getpid());

    return 0;
}

int get_md5(char * file, char * result){
    char command[MAX_SIZE];
    snprintf(command, MAX_SIZE, "md5sum %s", file);
    FILE * fp = popen(command, "r");
    if(fp == NULL){
        perror("popen");
        return 1;
    }
    if(fgets(result, MAX_SIZE, fp) != NULL){
        result[MD5_LEN] = '\0'; //saco de result el nombre del archivo
    }
    pclose(fp);
    return 0;
}