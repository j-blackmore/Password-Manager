#include "login.h"

const char *file_name = ".userprofile";
int create_profile();

int login() {
    if(check_file_exists(file_name) == 0) {
        create_profile();
        return 1;
    }

    // profile exists so get name and request decrypt password 
    FILE *file_reader = fopen(file_name, "r");
    char *profile_name = read_line(file_reader);
    printf("%s's profile found,\n", profile_name);
    printf("Enter decrypt key:");
    // get decrypt key, and check that it matches
    
    printf("\n");

    return 1;
}

int create_profile() {
    return 0;
}