#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>

#define MAX_SIZE 256

int get_md5(char * file, char * result); //la pongo con int por si queremos mandar un error

int main(int argc, char * argv[]){
    
    char *file = NULL;
    size_t len = 0;
    ssize_t nread;
    char md5[MAX_SIZE];
    /*
    // leemos el archivo de stdin
    while ((nread = getline(&file, &len, stdin)) > 0) {
        file[nread - 1] = '\0'; // sacamos el \n
        get_md5(file, md5);
    }//deberiamos hacer algo si aca hay error por getline
    */
    get_md5(argv[1], md5);
    printf("%s\t%d\n", md5, getpid());
    free(file);
    /*
    char command[MAX_SIZE];
    // guardo el comando de md5sum en command
    snprintf(command, MAX_SIZE, "md5sum %s", argv[1]);

    int pipefd[2];
    if(pipe(pipefd) == -1){
        perror("pipe");
        return 1;
    }
    if(fork() == 0){
        //estoy en el hijo que debe dejecutar md5sum
        close(3); //cierro el r-end
        execlp("sh", "sh", "-c", command, (char *) NULL);
        perror("execlp");
        exit(1);    
    } 
    wait(NULL);
    close(4); //cierro el w-end
    char result[MAX_SIZE];
    read(pipefd[0], md5, MAX_SIZE); //leo el pid del hijo 
    //habría que separar el nombre del md5
    printf("MD5: %PID ESCLAVO: esclavo: %d \n", md5, getpid());
    close(3);
    */
    return 0;
}

int get_md5(char * file, char * result){
    char command[MAX_SIZE] = {0};
    snprintf(command, MAX_SIZE, "md5sum %s", file);
    FILE * fp = popen(command, "r");
    if(fp == NULL){
        perror("popen");
        return 1;
    }
    if(fgets(result, MAX_SIZE, fp) != NULL){
        result[strlen(result) - 1] = '\0'; //saco el \n
    }
    pclose(fp);
    return 0;
}