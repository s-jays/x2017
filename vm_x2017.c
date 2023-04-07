#include "vm.h"

uint8_t is_main(struct vm *vm) {
    /*
    * Returns 1 is current function being executed is main(), otherwise
    * returns 0
    */

    uint8_t main_add = get_func(vm, 0);
    if (vm->reg[FUNC_PTR] == main_add) {
        return 1;
    }
    return 0;
}

uint8_t is_ret(struct vm *vm) {
    /*
    * Returns 1 if the current instruction being executed is a RET, otherwise
    * returns 0
    */

    struct function func = vm->code_mem[vm->reg[FUNC_PTR]];
    struct instruction instruct = func.instructions[vm->reg[PROG_CTR]];
    return (instruct.operation == RET);
}

int get_func(struct vm *vm, uint8_t label) {
    /*
    * Returns index location of function with label 'label' within code memory
    */

   int result = NO_VAL;
    int num_func = 0;
    for (int i = 0; i < vm->num_instruct; i ++) {
        if (vm->code_mem[i].label == label) {
            result = i;
            num_func ++;
        }
    }
    if (num_func != 1) {
        return NO_VAL;
    }
    return result;
}

uint8_t is_running(struct vm *vm) {
    /* 
    * Returns 1 if program has reached a return operation in main(), i.e. has
    * reached end of program
    */

   return !((is_main(vm)) && (is_ret(vm)));
};

void increment_pc(struct vm *vm) {
    /*
    * Increments program counter by one index
    */

    vm->reg[PROG_CTR] ++;
}

void increment_sp(struct vm *vm) {
    /*
    * Increments stack pointer by one index
    */

    vm->reg[STK_PTR] ++;
}

void decrement_sp(struct vm *vm) {
    /*
    * Decrements stack pointer by one index
    */

    vm->reg[STK_PTR] --;
}

void push_to_stack(struct vm *vm, BYTE *value) {
    /*
    * Pushes object at 'value' onto stack
    */

    vm->ram[vm->reg[STK_PTR]] = *value;
    increment_sp(vm);
}

void pop_from_stack(struct vm *vm) {
    /*
    * 'Pops' all variables from current stackframe
    */

    vm->reg[STK_PTR] = vm->reg[FRAME_PTR];
} 

void def_new_frame(struct vm *vm) {
    /*
    * Defines starting point of a new stack frame
    */

    if (vm->reg[FRAME_PTR] + SYM_BUF + RET_OFFSET >= RAM_LIMIT) {
        printf("Program error: stack overflow");
        exit(1);
    }
    vm->reg[FRAME_PTR] += SYM_BUF + RET_OFFSET;
    vm->reg[STK_PTR] = vm->reg[FRAME_PTR] - RET_OFFSET;
}

void set_pc(struct vm *vm, uint8_t num) {
    /*
    * Sets program counter to address 'num'
    */

    vm->reg[PROG_CTR] = num;
}

void op_mov(struct vm *vm, struct instruction *instruct) {
    /*
    * Executes MOV operation
    */

    increment_pc(vm);
    uint8_t val = instruct->val[0];
    switch (instruct->type[0]) {
        case VAL:
            break;
        case REG:
            val = vm->reg[val];
            break;
        case STK:
            val = vm->ram[vm->reg[FRAME_PTR] + val];
            break;
        case PTR:
            val = vm->ram[vm->ram[vm->reg[FRAME_PTR] + val]];
            break;
    }
    
    uint8_t destination = instruct->val[1];
    switch (instruct->type[1]) {
        case REG:
            vm->reg[destination] = val;
            break;
        case STK:
            vm->ram[vm->reg[FRAME_PTR] + destination] = val;
            break;
        case PTR:
            vm->ram[vm->ram[vm->reg[FRAME_PTR] + destination]] = val;
            break;
        default:
            printf("Operation could not be executed: "
                   "Unexpected argument type\n");
            exit(1);
    }
}

void op_cal(struct vm *vm, struct instruction *instruct) {
    /*
    * Executes CAL operation
    */   

    if (instruct->type[0] != VAL) {
        printf("Operation could not be executed: Unexpected argument type\n");
        exit(1);
    }
    uint8_t func_address = instruct->val[0];
    int func_index = get_func(vm, func_address);
    if (func_index == NO_VAL) {
        printf("Program could not be executed: Did not have exactly one "
               "function %d\n", func_address);
        exit(1);
    }
    increment_pc(vm);
    def_new_frame(vm);

    // Pushes return addresses onto stack
    push_to_stack(vm, &vm->reg[FUNC_PTR]);
    push_to_stack(vm, &vm->reg[PROG_CTR]);

    vm->reg[FUNC_PTR] = func_index;
    set_pc(vm, DEFAULT_VAL);
}

void op_ret(struct vm *vm) {
    /*
    * Executes RET operation
    */

    increment_pc(vm);
    pop_from_stack(vm);
    vm->reg[FRAME_PTR] = vm->reg[FRAME_PTR] - SYM_BUF - RET_OFFSET;

    // Retrieves function return addresses
    decrement_sp(vm);
    vm->reg[PROG_CTR] = vm->ram[vm->reg[STK_PTR]];
    decrement_sp(vm);
    vm->reg[FUNC_PTR] = vm->ram[vm->reg[STK_PTR]];
}

void op_ref(struct vm *vm, struct instruction *instruct) {
    /*
    * Executes REF operation
    */

    increment_pc(vm);

    uint8_t symbol = instruct->val[0];
    switch (instruct->type[0]) {
        case STK:
            symbol = vm->reg[FRAME_PTR] + symbol;
            break;
        case PTR:
            symbol = vm->ram[vm->reg[FRAME_PTR] + symbol];
            break;
        default:
            printf("Operation could not be executed: Unexpected argument type\n");
            exit(1);
    }

    uint8_t destination = instruct->val[1];
    switch (instruct->type[1]) {
        case REG:
            vm->reg[destination] = symbol;
            break;
        case STK:
            vm->ram[vm->reg[FRAME_PTR] + destination] = symbol;
            break;
        case PTR:
            vm->ram[vm->ram[vm->reg[FRAME_PTR] + destination]] = symbol;
            break;
        default:
            printf("Operation could not be executed: Unexpected argument "
                   "type\n");
            exit(1);
    }
}

void op_add(struct vm *vm, struct instruction *instruct) {
    /*
    * Executes ADD operation
    */

    increment_pc(vm);

    uint8_t reg_one = instruct->val[1];
    uint8_t reg_two = instruct->val[0];
    vm->reg[reg_one] = vm->reg[reg_one] + vm->reg[reg_two];
}

void op_print(struct vm *vm, struct instruction *instruct) {
    /*
    * Executes PRINT operation
    */

    increment_pc(vm);

    uint8_t arg_type = instruct->type[0];
    uint8_t arg_val = instruct->val[0];
    switch (arg_type) {
        case VAL:
            printf("%d\n", arg_val);
            break;
        case REG:
            printf("%d\n", vm->reg[arg_val]);
            break;
        case STK:
            printf("%d\n", vm->ram[vm->reg[FRAME_PTR] + arg_val]);
            break;
        case PTR:
            printf("%d\n", vm->ram[vm->ram[vm->reg[FRAME_PTR] + arg_val]]);
            break;
    }
}

void op_not(struct vm *vm, struct instruction *instruct) {
    /*
    * Executes NOT operation
    */

    increment_pc(vm);

    uint8_t reg_add = instruct->val[0];
    vm->reg[reg_add] = ~(vm->reg[reg_add]);
}

void op_equ(struct vm *vm, struct instruction *instruct) {
    /*
    * Executes EQU operation
    */
   
    increment_pc(vm);

    uint8_t reg_add = instruct->val[0];
    if (vm->reg[reg_add] == 0) {
        vm->reg[reg_add] = 1;
    } else {
        vm->reg[reg_add] = 0;
    }
}

int main(int argc, char **argv) {
    // Handles file errors and parses file
    if (argc != 2) {
        printf("Error: Please provide <filename> as command line argument\n");
        return 1;
    }

    FILE *bin_file = fopen(argv[1], "rb");
    if (bin_file == NULL) {
        perror("Error: File could not be opened");
        return 1;
    }

    fseek(bin_file, 0, SEEK_END);
    int num_bytes = ftell(bin_file);
    if (num_bytes == 0) {
        printf("Error: File cannot be empty\n");
        return 1;
    }
    fseek(bin_file, 0, SEEK_SET);

    BYTE f_bits[BUF] = {0};
    fread(f_bits, 1, num_bytes, bin_file);
    
    fclose(bin_file);

    // Sets up virtual machine's program code and initialises registers
    struct vm vm;
    struct vm *vm_ptr = &vm;
    struct function *func_ptr = vm_ptr->code_mem;
    BYTE *bit_ptr = &f_bits[num_bytes - 1];
    vm.num_instruct = parse(func_ptr, bit_ptr, num_bytes);

    int main_address = get_func(vm_ptr, 0);
    if (main_address == NO_VAL) {
        printf("Program could not be executed: Did not have exactly one main()"
               "\n");
        return 1;
    }

    vm.reg[FRAME_PTR] = DEFAULT_VAL;
    vm.reg[STK_PTR] = DEFAULT_VAL;
    vm.reg[FUNC_PTR] = main_address;
    vm.reg[PROG_CTR] = DEFAULT_VAL;

    // Executes program until end of main is reached
    while (is_running(vm_ptr)) {
        struct function *current_func = &vm.code_mem \
                                        [vm.reg[FUNC_PTR]];
        struct instruction *current_instruct = &current_func->instructions \
                                               [vm.reg[PROG_CTR]];
        switch (current_instruct->operation) {
            case MOV:
                op_mov(vm_ptr, current_instruct);
                break;
            case CAL:
                op_cal(vm_ptr, current_instruct);
                break;
            case RET:
                op_ret(vm_ptr);
                break;
            case REF:
                op_ref(vm_ptr, current_instruct);
                break;
            case ADD:
                op_add(vm_ptr, current_instruct);
                break;
            case PRINT:
                op_print(vm_ptr, current_instruct);
                break;
            case NOT:
                op_not(vm_ptr, current_instruct);
                break;
            case EQU:
                op_equ(vm_ptr, current_instruct);
                break;
        }
    }
    return 0;
}
