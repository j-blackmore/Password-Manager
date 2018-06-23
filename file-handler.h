#define _POSIX_C_SOURCE 201806L

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern int check_file_exists(const char* file_name);
extern char *read_line(FILE *file_pointer);