#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>

#define MAX_SIZE 256

int main(int argc, char * argv[]){
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

    return 0;
}