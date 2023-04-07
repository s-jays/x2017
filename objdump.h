#ifndef OBJDUMP_H
#define OBJDUMP_H

#include "objects.h"
#include "parser.h"

int find_symbol(char array[], int size, char target);

void print_func(struct function *func);

#endif