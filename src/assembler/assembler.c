#include "assembler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BASE_ARR_SIZE (10)
#define ARR_SIZE_MULTIPLY (2)
#define OPCODES_NUM (22)
#define REGS_NUM (16)

static const char *opcodes[] = {"add",
                                "sub", 
                                "mac", 
                                "and", 
                                "or", 
                                "xor",
                                "sll",
                                "sra",
                                "srl", 
                                "beq", 
                                "bne", 
                                "blt", 
                                "bgt", 
                                "ble", 
                                "bge", 
                                "jal", 
                                "lw", 
                                "sw", 
                                "reti", 
                                "in", 
                                "out", 
                                "halt"};

static const char *regs[] = {"zero",
                            "imm1", 
                            "imm2", 
                            "v0", 
                            "a0", 
                            "a1",
                            "a2",
                            "t0",
                            "t1", 
                            "t2", 
                            "s0", 
                            "s1", 
                            "s2", 
                            "gp", 
                            "sp", 
                            "ra"};

static int get_opcode_num(char* opcode_str) {
    for (int i = 0; i < OPCODES_NUM; i++)
    {
        if (strcmp(opcode_str, opcodes[i]) == 0)
        {
            return i;
        }
    }
    return -1;
}

static int get_reg_num(char* reg_str) {
    for (int i = 0; i < OPCODES_NUM; i++)
    {
        if (strcmp(reg_str, regs[i]) == 0)
        {
            return i;
        }
    }
    return -1;
}

int main(int argc, char const *argv[])
{
    char const *filename = argv[1];
    char buffer[MAX_LINE_LENGTH];
    FILE *asm_program = fopen(filename, "r");
    int line_counter = -1;
    int label_counter = 0, curr_arr_len = BASE_ARR_SIZE;
    label_t *labels_list = (label_t *)calloc(BASE_ARR_SIZE, sizeof(label_t));
    char *tmp_label_str;

    while (fgets(buffer, MAX_LINE_LENGTH, asm_program) != NULL)
    {
        line_counter++;
        for (int i = 0; i < MAX_LABEL_LENGTH + 1; i++)
        {
            if (buffer[i] == ':')
            {
                buffer[i] = '\0';
                tmp_label_str = (char *)malloc(i + 1);
                strcpy(tmp_label_str, buffer);

                label_t tmp_label = {
                    .label = tmp_label_str,
                    .op_index = line_counter--}; /* label lines don't count as instruction lines */

                if (label_counter == curr_arr_len)
                {
                    curr_arr_len *= ARR_SIZE_MULTIPLY;
                    labels_list = (label_t *)realloc(labels_list, curr_arr_len * sizeof(label_t));
                }
                labels_list[label_counter++] = tmp_label;
                break;
            }
        }
    }

    labels_list = (label_t *)realloc(labels_list, label_counter * sizeof(label_t));
    printf("hi");
}