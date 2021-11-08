#include "assembler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BASE_ARR_SIZE (10)
#define ARR_SIZE_MULTIPLY (2)
#define OPCODES_NUM (22)
#define REGS_NUM (16)
#define OUTPUT_INSTR_FILE_NAME "imemin.txt"
#define OUTPUT_DATA_FILE_NAME "dmemin.txt"

label_t *labels_arr;
static int label_count = 0;

static const char *opcodes_arr[] = {"add",
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

static const char *regs_arr[] = {"zero",
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

static int get_opcode_num(char *opcode_str)
{
    for (int i = 0; i < OPCODES_NUM; i++)
    {
        if (strcmp(opcode_str, opcodes_arr[i]) == 0)
        {
            return i;
        }
    }
    return -1;
}

static int get_reg_num(char *reg_str)
{
    for (int i = 0; i < OPCODES_NUM; i++)
    {
        if (strcmp(reg_str, regs_arr[i]) == 0)
        {
            return i;
        }
    }
    return -1;
}

static int is_label(char *imm)
{
    return (imm[0] >= 'a' && imm[0] <= 'z') || (imm[0] >= 'A' && imm[0] <= 'Z');
}

static int get_label_num(char *label)
{
    for (int i = 0; i < label_count; i++)
    {
        if (strcmp(label, labels_arr[i].label) == 0)
        {
            return labels_arr[i].cmd_index;
        }
    }
}

static int does_line_contain_label(char *line) {
    for (int i = 0; i < MAX_LABEL_LENGTH; i++) {
        if (line[i] == ':') {
            return 1;
        }
        if (line[i] == '\0') {
            return 0;
        }
    }
    return 0;
}

static void decode_mips_to_hex(FILE *output_file, char *line)
{
    char *opcode, rd, rs, rt, rm, imm1, imm2;
    sscanf(line, "%s $%s, $%s, $%s, $%s, $%s, $%s", &opcode, &rd, &rs, &rt, &rm, &imm1, &imm2);

    int opcode_d, rd_d, rs_d, rt_d, rm_d, imm1_d, imm2_d;
    opcode_d = get_opcode_num(opcode);
    rd_d = get_reg_num(rd);
    rs_d = get_reg_num(rs);
    rt_d = get_reg_num(rt);
    rm_d = get_reg_num(rm);

    if (is_label(imm1))
    {
        imm1_d = get_label_num(imm1);
    }
    else
    {
        imm1_d = atoi(imm1);
    }

    if (is_label(imm2))
    {
        imm2_d = get_label_num(imm2);
    }
    else
    {
        imm2_d = atoi(imm2);
    }

    fprintf(output_file, "%02X%01X%01X%01X%01X%03X%03X\n", opcode_d, rd_d, rs_d, rt_d, rm_d, imm1_d, imm2_d);
}

int main(int argc, char const *argv[])
{
    char const *filename = argv[1];
    char buffer[MAX_LINE_LENGTH];
    FILE *asm_program = fopen(filename, "r");
    int line_counter = -1;
    int curr_arr_len = BASE_ARR_SIZE;
    labels_arr = (label_t *)calloc(BASE_ARR_SIZE, sizeof(label_t));
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
                    .cmd_index = line_counter--}; /* label lines don't count as instruction lines */

                if (label_count == curr_arr_len)
                {
                    curr_arr_len *= ARR_SIZE_MULTIPLY;
                    labels_arr = (label_t *)realloc(labels_arr, curr_arr_len * sizeof(label_t));
                }
                labels_arr[label_count++] = tmp_label;
                break;
            }
        }
    }

    labels_arr = (label_t *)realloc(labels_arr, label_count * sizeof(label_t));
    printf("hi");

    FILE *output_instr_file = fopen(OUTPUT_INSTR_FILE_NAME, "w");
    while (fgets(buffer, MAX_LINE_LENGTH, asm_program) != NULL)
    {
        if (does_line_contain_label(buffer) == 0) {
            decode_mips_to_hex(output_instr_file, buffer);
        }
    }
    fclose(asm_program);
    fclose(output_instr_file);
}