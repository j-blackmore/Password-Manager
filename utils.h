#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

extern int *integer_to_binary(unsigned long long int integer, int num_of_bits);
extern int *string_to_binary(char *string_to_convert);
extern void print_binary_array(int *binary_array, int array_len);
extern int binary_to_integer(int *binary_array, int num_of_bits);