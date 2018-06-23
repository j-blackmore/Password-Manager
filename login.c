#include "login.h"

const char *file_name = ".userprofile";

int login() {
    if(check_file_exists(file_name) != 0) {
        
    } else {
        printf("Welcome! Please enter a username:");

        printf("\n");
    }
    return 0;
}