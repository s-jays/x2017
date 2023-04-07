#ifndef VM_H
#define VM_H

#include <stdlib.h>
#include "parser.h"
#include "objects.h"

#define NO_VAL -1
#define DEFAULT_VAL 0
#define RAM_LIMIT 256
#define REG_LIMIT 8
#define FRAME_PTR 4
#define STK_PTR 5
#define FUNC_PTR 6
#define PROG_CTR 7
#define RET_OFFSET 2

struct vm {
    BYTE ram[RAM_LIMIT];
    BYTE reg[8];
    struct function code_mem[REG_LIMIT];
    int num_instruct;
};

// Helper functions
uint8_t is_main(struct vm *vm);

uint8_t is_ret(struct vm *vm);

int get_func(struct vm *vm, uint8_t label);

uint8_t is_running(struct vm *vm);

void increment_pc(struct vm *vm);

void increment_sp(struct vm *vm);

void decrement_sp(struct vm *vm);

void push_to_stack(struct vm *vm, BYTE *value);

void pop_from_stack(struct vm *vm);

void def_new_frame(struct vm *vm);

void set_pc(struct vm *vm, uint8_t num);

// Instruction operations
void op_mov(struct vm *vm, struct instruction *instruct);

void op_cal(struct vm *vm, struct instruction *instruct) ;

void op_ret(struct vm *vm);

void op_ref(struct vm *vm, struct instruction *instruct);

void op_add(struct vm *vm, struct instruction *instruct);

void op_print(struct vm *vm, struct instruction *instruct);

void op_not(struct vm *vm, struct instruction *instruct);

void op_equ(struct vm *vm, struct instruction *instruct);

#endif