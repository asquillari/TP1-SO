
#define MAX_ERROR 256
#define MAX_SLAVES 10

void create_all_slaves(int num_slaves, int ** slaves_fd,char * slave_params[]);
void send_file(int fd, char * filename, int cant_files, int cant_files_sent);