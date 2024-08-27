#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <stdlib.h>

#define MAX_SIZE 256
#define MD5_LEN 32

#define FORMAT_OUTPUT "MD5: %s FILE: %s SLAVE_PID: %d\n"
#define NOT_FILE "NOT A FILE"

static void get_md5(char * file); //la pongo con int por si queremos mandar un error
static int is_file(char * file);

int main(int argc, char * argv[]){
    
    //read filename 
    char * file = NULL;
    size_t len = 0;
    ssize_t read;

    //read from stdin
    while((read = getline(&file, &len, stdin)) != -1){
        file[read - 1] = '\0'; 
        get_md5(file);
    }

    free(file);
    return 0;
}

static void get_md5(char * file){
    if (!is_file(file)){
        fprintf(stdout, FORMAT_OUTPUT, NOT_FILE, file ,getpid());
        return;
    }
    //tengo que hacer aca el result
    char command[MAX_SIZE];
    char md5[MAX_SIZE];

    snprintf(command, MAX_SIZE, "md5sum %s", file);

    FILE * fp = popen(command, "r");
    if(fp == NULL){
        perror("popen");
        return;
    }
    if(fgets(md5, MAX_SIZE, fp) != NULL){
        md5[MD5_LEN] = '\0'; //saco de result el nombre del archivo
    }
    fprintf(stdout, FORMAT_OUTPUT, md5, file, getpid());

    pclose(fp);
    return;
}

static int is_file(char * file){
    struct stat path;
    return stat(file, &path) >= 0 && S_ISREG(path.st_mode);
}