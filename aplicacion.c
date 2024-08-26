#include <stdio.h> 

#define MAX_ERROR 256
#define MAX_SLAVES 10

int main(int argc, char *argv[]){
    //esto hay que modilarizarlo todo y hacer capas una clase
    if (argc == 1) {
        char buffer_error[MAX_ERROR];
        snprintf(buffer_error, MAX_ERROR, "Error. Use: %s 'file' 'file' ...", argv[0]);
        printf("%s\n", buffer_error);
        exit(EXIT_FAILURE); //siento que esto esta mal, ademas lo modularizaria
    }

    int cant_files=argc-1;
    int num_workers = cant_files < MAX_SLAVES ? cant_files : MAX_SLAVES;
    int slaves_fd[2][num_slaves];   //vamos atener 2 pipes por cada slave

    int initial_dist = (cant_files*0.2) / num_slaves;

    if(initial_dist == 0){
        initial_dist = 1;
    }
    

}