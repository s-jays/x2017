#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>
#include "objects.h"

#define BUF 637 // Maximum number of bytes in a valid x2017 file

void update_ptr(BYTE **byte_array);

BYTE read_bits(BYTE **byte_array, int *bits_read, int to_read);

uint8_t process_opcode(struct instruction *instruct, BYTE op_num);

int process_valcode(struct instruction *instruct, BYTE val_code, int arg);

int parse(struct function *func_array, BYTE *bit_array, int num_bytes);

#endif