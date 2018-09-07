#include "login.h"
#include "crypto.h"
#include <termios.h>

int create_profile();
static int check_hash_matches(char *input, char *hash);
static int read_password(char **input, size_t *size);

const char *file_name = ".userprofile";

int login() {
    if(check_file_exists(file_name) == 0) {
        create_profile();
        return 1;
    }

    // profile exists so get name and hash
    FILE *file_reader = fopen(file_name, "r");
    char *profile_name = read_line(file_reader);
    char *hash = read_line(file_reader);
    fclose(file_reader);
    
    // request password
    printf("%s's profile found\n", profile_name);
    printf("Enter password:");
    char *input_password = NULL;
    size_t input_len;
    if(read_password(&input_password, &input_len) != 0) {
        // error
        fprintf(stderr, "Error reading password\n");
    }
    
    // compare hash with hashed password
    if(check_hash_matches(input_password, hash)) {
        printf("Welcome!\n");
    } else {
        printf("Incorrect password\n");
    }

    return 1;
}

int create_profile() {
    return 0;
}

static int check_hash_matches(char *input, char *hash) {
    return strcmp(sha256(input), hash) == 0 ? 1 : 0;
}

static int read_password(char **input, size_t *size) {
    // get terminal info
    struct termios termInfo;
    if(tcgetattr(STDERR_FILENO, &termInfo) != 0) {
        return 1;
    }

    // turn off terminal echoing
    termInfo.c_lflag &= ~ECHO;
    if(tcsetattr(STDIN_FILENO, TCSANOW, &termInfo) != 0) {
        return 1;
    } 

    // read password and replace newline with end of string char
    getline(input, size, stdin);
    *input[strlen(*input)-1] = '\0';

    // turn on terminal echoing
    termInfo.c_lflag |= ECHO;
    if(tcsetattr(STDIN_FILENO, TCSANOW, &termInfo) != 0) {
        return 1;
    }

    // newline because enter was not output
    printf("\n");
    return 0;
}