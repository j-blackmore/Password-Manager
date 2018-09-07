#include "file-handler.h"

int check_file_exists(const char *file_name)
{
    if(access(file_name, F_OK) != -1) {
        return 1;
    } else {
        return 0;
    }
}

char *read_line(FILE *file_pointer) 
{
    if(file_pointer == NULL) {
        printf("Cannot read from file\n");
    }

    int buffer_size = 16;
    char line_buffer[buffer_size];
    char *finished_line = malloc(sizeof(char));
    size_t line_len = 0;

    while (fgets(line_buffer, buffer_size, file_pointer)) {

        size_t read_len = strlen(line_buffer);
        line_len += read_len;
        finished_line = realloc(finished_line, sizeof(char) * line_len);
        strcat(finished_line, line_buffer);

        // cut empty space and newline character
        if(read_len != buffer_size-1 || line_buffer[buffer_size-1] == '\n') {
            finished_line[line_len-1] = '\0';
            char *temp = strdup(finished_line);
            free(finished_line);
            finished_line = temp;
            break;
        }
    }

    return finished_line;
}