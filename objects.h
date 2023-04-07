#ifndef OBJ_H
#define OBJ_H

#include <stdint.h>

#define BYTE unsigned char
#define BYTE_SIZE 8
#define SYM_BUF 32

enum val_type {
    VAL,
    REG,
    STK,
    PTR
};

enum opcode {
    MOV,
    CAL,
    RET,
    REF,
    ADD,
    PRINT,
    NOT,
    EQU
};

struct instruction {
    enum opcode operation;
    uint8_t num_args;
    enum val_type type[2];
    uint8_t val[2];
};

struct function {
    BYTE label;
    struct instruction instructions[32];
    uint8_t num_instruct;
};

#endif