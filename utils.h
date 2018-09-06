#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

extern int *integer_to_binary(unsigned long long int integer, int num_of_bits);
extern int *string_to_binary(char *string_to_convert);
extern void print_binary_array(int *binary_array, int array_len);
extern unsigned int binary_to_unsigned_integer(int *binary_array, int num_of_bits);
extern unsigned int right_rotate(unsigned int integer, int bits);
extern unsigned int right_shift(unsigned int integer, int bits);
extern char *unsigned_int_array_to_hex_string(unsigned int *array, int array_len);