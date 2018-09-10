#include "login.h"
#include "crypto.h"
#include <termios.h>

int create_profile();
static int check_hash_matches(char *input, char *hash);
static int read_password(char **input);

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
    if(read_password(&input_password) != 0) {
        fprintf(stderr, "Error reading password\n");
        return 1;
    }
    
    // compare hash with hashed password
    if(check_hash_matches(input_password, hash)) {
        printf("Welcome!\n");
    } else {
        printf("Incorrect password\n");
    }

    return 0;
}

int create_profile() {
    printf("No profile found\nEnter new profile name:");
    char *input = NULL;
    size_t input_len;
    getline(&input, &input_len, stdin);
    char *name = strdup(input);
    input = NULL;
    printf("Enter password:");
    if(read_password(&input) != 0) {
        fprintf(stderr, "Error reading password\n");
        return 1;
    }

    FILE *file_writer = fopen(file_name, "w");
    write_line(name, file_writer);
    write_line(sha256(input), file_writer);
    write_line("\n", file_writer);
    fclose(file_writer);
    printf("Profile created!\n");

    return 0;
}

static int check_hash_matches(char *input, char *hash) {
    return strcmp(sha256(input), hash) == 0 ? 1 : 0;
}

static int read_password(char **input) {
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
    size_t size;
    getline(input, &size, stdin);
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