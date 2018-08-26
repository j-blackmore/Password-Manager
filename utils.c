#include "utils.h"

int *integer_to_binary(unsigned long long int integer, int num_of_bits) {
    int *binary = malloc(sizeof(int) * num_of_bits);
    int i;
    for(i = 0; i < num_of_bits; i++) {
        binary[i] = 0;
    }

    for(i = 0; i < num_of_bits; i++) {
        binary[i] = (int) (integer >> (num_of_bits-1-i));
        if(binary[i]) integer = integer ^ (1 << (num_of_bits-1-i));
    }

    return binary;
}

int *string_to_binary(char *string_to_convert) {
    int string_len = strlen(string_to_convert);

    int i, j; 
    int *binary = malloc(sizeof(int) * string_len * 8);
    for(i = 0; i < string_len; i++) {
        int ascii_val = string_to_convert[i];

        for(j = 0; j < 8; j++) {
            binary[i*8 + j] = ascii_val >> (7-j);
            if(binary[i*8 + j]) ascii_val = ascii_val ^ (1 << (7-j));
        }
    }

    return binary;
}

void print_binary_array(int *binary_array, int array_len) {
    int i;
    for(i = 0; i < array_len; i++){
        printf("%d", binary_array[i]);
    }
    printf("\n");
}

int binary_to_integer(int *binary_array, int num_of_bits) {
    int i, integer = 0;
    for(i = 0; i < num_of_bits; i++) {
        if(binary_array[0]) integer += (int) pow(2, num_of_bits-1-i);
    }

    return integer;
}