#include "login.h"
#include "crypto.h"

int create_profile();
static int check_hash_matches(char *input, char *hash);

const char *file_name = ".userprofile";

int login() {
    if(check_file_exists(file_name) == 0) {
        create_profile();
        return 1;
    }

    // profile exists so get name
    FILE *file_reader = fopen(file_name, "r");
    char *profile_name = read_line(file_reader);
    printf("%s's profile found\n", profile_name);
    
    // request password
    printf("Enter password:");
    char *input_password = NULL;
    size_t input_len;
    getline(&input_password, &input_len, stdin);
    // replace newline char with end of string
    input_password[strlen(input_password)-1] = '\0';
    
    // compare hash with hashed password
    char *hash = read_line(file_reader);
    if(check_hash_matches(input_password, hash)) {
        printf("Welcome!\n");
    } else {
        printf("Incorrect password\n");
    }

    fclose(file_reader);
    return 1;
}

int create_profile() {
    return 0;
}

static int check_hash_matches(char *input, char *hash) {
    return strcmp(sha256(input), hash) == 0 ? 1 : 0;
}