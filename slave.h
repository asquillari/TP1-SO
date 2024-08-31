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

static void get_md5(char * file);
static int is_file(char * file);