#include <stdio.h> 

#define MAX_ERROR 256

int main(int argc, char *argv[]){
    if (argc == 1) {
        char buffer_error[MAX_ERROR];
        snprintf(buffer_error, MAX_ERROR, "Error. Use: %s 'file' 'file' ...", argv[0]);
        printf("%s\n", buffer_error);
        exit(1); //siento que esto esta mal, ademas lo modularizaria
    }
}