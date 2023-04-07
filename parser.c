#include "parser.h"

void update_ptr(BYTE **byte_array) {
    /*
    * Decrements 'byte_array' pointer
    */

    (*byte_array) --;
}

BYTE read_bits(BYTE **byte_array, int *bits_read, int to_read) {
    /*
     * Reads 'to_read' number of bits from 'byte_array' starting from
     * 'bits_read'
     * Returns value of bits
     */

    BYTE *bits_ptr = *byte_array;
    BYTE output = 0;

    // Case 1: Target bits are located within the same byte
    if ((*bits_read % BYTE_SIZE) + to_read <= BYTE_SIZE) {
        BYTE shift = *bits_ptr >> (*bits_read % BYTE_SIZE);
        output = shift & ((0x1 << to_read) - 1);
        if ((*bits_read % BYTE_SIZE) + to_read == BYTE_SIZE) {
            update_ptr(byte_array);
        }

    // Case 2: Target bits are spread out over 2 bytes
    //    e.g. [010000(10] [1000)1000] where target is '101000'
    } else {
        BYTE right = *bits_ptr >> (*bits_read % BYTE_SIZE);
        bits_ptr --;
        BYTE left = *bits_ptr << (BYTE_SIZE - (*bits_read % BYTE_SIZE));
        output = (left | right) & ((0x1 << to_read) - 1);
        update_ptr(byte_array);
    }
    *bits_read += to_read;
    return output;
}

uint8_t process_opcode(struct instruction *instruct, BYTE op_num) {
    /*
     * Sets instruction opcode and num_args according to provided operation 
     * number
     * Returns number of arguments associated with operation type
     */

    uint8_t args = 0;
    enum opcode operation;
    switch (op_num) {
        case 0:
            operation = MOV;
            args = 2;
            break;
        case 1:
            operation = CAL;
            args = 1;
            break;
        case 2:
            operation = RET;
            args = 0;
            break;
        case 3:
            operation = REF;
            args = 2;
            break;
        case 4:
            operation = ADD;
            args = 2;
            break;
        case 5:
            operation = PRINT;
            args = 1;
            break;
        case 6:
            operation = NOT;
            args = 1;
            break;
        case 7:
            operation = EQU;
            args = 1;
            break;
    }
    instruct->operation = operation;
    instruct->num_args = args;
    return args;
}

int process_valcode(struct instruction *instruct, BYTE val_code, int arg) {
    /*
     * Sets instruction argument type according to provided value code
     * Returns length of argument in bits
     */

    int to_read = 0;
    enum val_type type;
    switch (val_code) {
        case 0:
            type = VAL;
            to_read = 8;
            break;
        case 1:
            type = REG;
            to_read = 3;
            break;
        case 2:
            type = STK;
            to_read = 5;
            break;
        case 3:
            type = PTR;
            to_read = 5;
            break;
    }
    instruct->type[arg] = type;
    return to_read;
}

int parse(struct function *func_array, BYTE *bit_array, int num_bytes) {
    /*
    * Given bit_array and the size of bit_array in bytes, processes bit_array
    * into Functions and stores in func_array
    * Returns number of functions parsed
    */
    
    int num_func = 0;
    int bits_read = 0;

    while (((num_bytes * BYTE_SIZE) - bits_read) > (BYTE_SIZE - 1)) {
        int to_read = 5;
        struct function new_func;

        // Reads 5 bits to determine number of instructions in function
        BYTE num_instruct = read_bits(&bit_array, &bits_read, to_read);
        new_func.num_instruct = num_instruct;
        int instruct_count = num_instruct;

        while (instruct_count > 0) {
            struct instruction new_instruct;

            // Reads 3 bits to determine instruction opcode one at a time until
            // all opcodes have been processed
            to_read = 3;
            BYTE op_num = read_bits(&bit_array, &bits_read, to_read);
            int args = process_opcode(&new_instruct, op_num);

            while (args > 0) {
                args --;

                // Reads 2 bits to determine value type of instruction argument
                to_read = 2;
                BYTE val_code = read_bits(&bit_array, &bits_read, to_read);
                to_read = process_valcode(&new_instruct, val_code, args);

                // Reads number of bits associated with value type to get value
                BYTE val = read_bits(&bit_array, &bits_read, to_read);
                new_instruct.val[args] = val;
            }
            instruct_count --;
            new_func.instructions[instruct_count] = new_instruct;
        }

        // Reads 3 bits to determine function label
        to_read = 3;
        BYTE func_label = read_bits(&bit_array, &bits_read, to_read);
        new_func.label = func_label;
        *func_array = new_func;

        func_array ++;
        num_func ++;
    }
    return num_func;
}
