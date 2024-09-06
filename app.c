// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "include/app.h"
#include "include/app_lib.h"
#include "ADTs/slaveADT.h"

int main(int argc, char *argv[]){
    //esto hay que modilarizarlo todo y hacer capaz una clase
    if (argc == 1) {
        char buffer_error[MAX_ERROR];
        snprintf(buffer_error, MAX_ERROR, "Error. Use: %s 'file' 'file' ...", argv[0]);
        printf("%s\n", buffer_error);
        exit(EXIT_FAILURE); //siento que esto esta mal, ademas lo modularizaria
    }


    int cant_files=argc-1;
    char ** files = &argv[1];

    slaveADT sm = initialize_slaves(cant_files, files);

    send_first_files(sm);

    // creamos el file de resultado 
    FILE * result_file = fopen("result.txt", "w"); 
    if (result_file == NULL) {
        perror("Error opening result file");
        exit(EXIT_FAILURE);  
    }

    while(has_next_file(sm)){
        char buffer[MAX_SIZE] = {0};
        int bytes_read = read_from_slave(sm, buffer);

        if(bytes_read < 0){
            perror("Read");
            free(sm);
            exit(EXIT_FAILURE);
        }
        has_read(sm);
        fprintf(result_file, "%s", buffer);
        
        //fijarse que onda el error

    }

    //cerramos el archivo
    fclose(result_file);

    close_pipes(sm);

    free(sm);

    return 0;
    
}
