#include "objdump.h"

int find_symbol(char array[], int size, char target) {
    /*
     * Finds and returns index of 'target' in 'array'
     * Returns -1 if 'target' cannot be found
     */ 

    for (int i = 0; i < size; i ++) {
        if (array[i] == target) {
            return i;
        }
    }
    return -1;
}

char map_symbol(int index) {
    /*
     * Maps symbols to letters in order of appearance
     */ 

    char symbols[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 
                      'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V',
                      'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f'};
    return symbols[index];
}

void print_func(struct function *func) {
    /*
     * Prints function labels and commands
     */ 

    printf("FUNC LABEL %d\n", func->label);
    char symbol_arr[SYM_BUF] = {0};
    int arr_size = 0;

    // Loops through each instruction in the function
    for (int i = 0; i < func->num_instruct; i ++) {
        enum opcode operation = func->instructions[i].operation;
        char const *print_type;

        switch (operation) {
            case MOV:
                print_type = "MOV";
                break;
            case CAL:
                print_type = "CAL";
                break;
            case RET:
                print_type = "RET";
                break;
            case REF:
                print_type = "REF";
                break;
            case ADD:
                print_type = "ADD";
                break;
            case PRINT:
                print_type = "PRINT";
                break;
            case NOT:
                print_type = "NOT";
                break;
            case EQU:
                print_type = "EQU";
                break;
        }
        printf("    %s", print_type);
        
        // Loops through instruction arguments
        for (int j = func->instructions[i].num_args - 1; j >= 0 ; j --) {
            enum val_type type = func->instructions[i].type[j];
            char const *print_type;

            switch (type) {
                case VAL:
                    print_type = "VAL";
                    break;
                case REG:
                    print_type = "REG";
                    break;
                case STK:
                    print_type = "STK";
                    break;
                case PTR:
                    print_type = "PTR";
                    break;
            }
            printf(" %s", print_type);

            int value = func->instructions[i].val[j];
            if ((type == STK) || (type == PTR)) {
                int index = find_symbol(symbol_arr, arr_size, value);
                
                // If symbol has not been encountered yet, record it in symbol
                // array
                if (index == -1) {
                    symbol_arr[arr_size] = value;
                    index = arr_size;
                    arr_size ++;
                }
                char symbol = map_symbol(index);
                printf(" %c", symbol);
            } else {
                printf(" %d", value);
            }
        }
        printf("\n");
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

    struct function func_array[8] = {0};
    struct function *func_ptr = func_array;
    BYTE *bit_ptr = &f_bits[num_bytes - 1];
    int num_func = parse(func_ptr, bit_ptr, num_bytes);
    
    for (int i = num_func; i > 0; i --) {
        print_func(&func_array[i - 1]);
    }
    return 0;
}
