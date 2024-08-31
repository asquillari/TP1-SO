#include "slave.h"

int main(int argc, char * argv[]){
    
    //read filename 
    char file[MAX_SIZE] = {0};

    //read from stdin
    fgets(file, MAX_SIZE, stdin);
    get_md5(file);
    exit(EXIT_SUCCESS);
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