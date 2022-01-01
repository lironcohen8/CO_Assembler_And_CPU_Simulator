#include "assembler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <inttypes.h>

#define MAX(X, Y)               (((X) > (Y)) ? (X) : (Y))

int g_max_memory_index = 0; /* Holds the max non empty index in the data array */
int g_command_counter = 0;
static int g_label_count = 0;
int g_data_memory[DATA_MEMORY_SIZE]; /* Stores all the '.word' commands */
label_t g_labels_arr[MAX_ASSEMBLY_LINES]; /* Stores all the labels and their indexes in the code */

/* Dictionaries for the opcode and regs decoding */
static const char* opcodes_arr[] = { "add",
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
                                    "halt" };

static const char* regs_arr[] = { "zero",
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
                                 "ra" };

static int get_opcode_num(char* opcode_str)
{
    /*Returns the index of the opcode*/
    for (int i = 0; i < OPCODES_NUM; i++)
    {
        if (strcmp(opcode_str, opcodes_arr[i]) == 0)
        {
            return i;
        }
    }
    return -1;
}

static int get_reg_num(char* reg_str)
{
    /*Returns the index of the register*/
    for (int i = 0; i < CPU_REGS_NUM; i++)
    {
        if (strcmp(reg_str, regs_arr[i]) == 0)
        {
            return i;
        }
    }
    return -1;
}

/* Helper functions for line classification */
static int get_label_num(char* label)
{
    /*Returns the command index the label is pointing to*/
    for (int i = 0; i < g_label_count; i++)
    {
        if (strcmp(label, g_labels_arr[i].label) == 0)
        {
            return g_labels_arr[i].cmd_index;
        }
    }
    return -1;
}

static int does_line_contain_label(char* line) {
    /*Returns if the line starts with a label (actually checking if line contains ':',
    so if we have a ':' inside a comment this function will return 1)*/
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

static int is_label(char* imm)
{
    /*This function gets a string and checks if the first char is a letter.
    This is used to determine if an immediate value is a number or a label*/
    return (imm[0] >= 'a' && imm[0] <= 'z') || (imm[0] >= 'A' && imm[0] <= 'Z');
}

/* General helper functions */
static void decode_cmds_to_output_file(FILE* output_file, char* line)
{
    /*This function is used to parse the line*/
    char opcode[MAX_OPCODE_LENGTH], rd[MAX_REG_LENGTH], rs[MAX_REG_LENGTH], rt[MAX_REG_LENGTH], rm[MAX_REG_LENGTH], imm1[32], imm2[32];
    sscanf_s(line, " %[^ $] $%[^,], $%[^,], $%[^,], $%[^,], %[^,], %s ",
        opcode, MAX_OPCODE_LENGTH,
        rd, MAX_REG_LENGTH,
        rs, MAX_REG_LENGTH,
        rt, MAX_REG_LENGTH,
        rm, MAX_REG_LENGTH,
        imm1, 32,
        imm2, 32);

    int opcode_d, rd_d, rs_d, rt_d, rm_d, imm1_d, imm2_d;
    /*Get the opcode and registers numbers from their dictionaries*/
    opcode_d = get_opcode_num(opcode);
    rd_d = get_reg_num(rd);
    rs_d = get_reg_num(rs);
    rt_d = get_reg_num(rt);
    rm_d = get_reg_num(rm);

    /*Both imm1 and imm2 can be either a number or a label so we do the following:
    First we check if they are labels and if so we get the command index they point to.
    If we got a number we change it from string to int using 'strtol'*/
    if (is_label(imm1)) {
        imm1_d = get_label_num(imm1);
    }
    else {
        imm1_d = strtol(imm1, NULL, 0);
    }

    if (is_label(imm2)) {
        imm2_d = get_label_num(imm2);
    }
    else {
        imm2_d = strtol(imm2, NULL, 0);
    }

    /*Takes the 3 LSB bits of imm1/imm2. This is used to prevent nagative numbers to be printed
    with their most significant bit extended*/
    imm1_d = imm1_d & 0xFFF;
    imm2_d = imm2_d & 0xFFF;

    /*Write the command decoding to the output file*/
    fprintf(output_file, "%02X%01X%01X%01X%01X%03X%03X\n", opcode_d, rd_d, rs_d, rt_d, rm_d, imm1_d, imm2_d);
}

static void add_data_to_memory(char* line) {
    /*Used for a '.word' command - will store the second number at the first number index in
    the g_data_memory array*/
    int address, value;
    sscanf_s(line, ".word %i %i", &address, &value); /*%i deducts if it is int or hexa*/

    g_data_memory[address] = value;
    g_max_memory_index = MAX(g_max_memory_index, address); /*Updates the max non empty index at the data_memory array*/
}

static void write_memory_file(FILE* output_data_file) {
    /*Writes the memory data file*/
    for (int i = 0; i <= g_max_memory_index; i++) {
        fprintf(output_data_file, "%08X\n", g_data_memory[i]);
    }
}

/* Line functions */
static void clear_leading_white_spaces(char** line) {
    /*Trims leading spaces in a line by advancing the pointer to the line until the first
    non '\n', '\t' or ' ' char*/
    while (isspace((char)**line)) {
        (*line)++;
    }
}

static int is_line_comment(char* line) {
    /*Checks if the first char is '#', at this point we already trimmed the leading spaces.*/
    return line[0] == '#';
}

static int line_has_label(char* line) {
    /*Checks if this line contains a label and returns the ':' index*/
    int i = 0;
    char c = line[0];

    while (c != '\0') {
        if (c == '#') { /*If we get to a comment anything further then that is not important*/
            break;
        }
        if (c == ':') { /*If we got to a ':' (before '#') then this is where the label ends*/
            return i;
        }
        c = line[++i];
    }

    return -1;
}

static int is_line_word_command(char* line) {
    /*Checks if the first char is '.', at this point we already trimmed the leading spaces.*/
    return line[0] == '.';
}

static int line_has_command(char* line) {
    /*Checks if a line has both a label and a command, at this point we already skipped the label
    and trimmed the leading whitespaces*/
    char first_word[MAX_OPCODE_LENGTH];
    sscanf_s(line, "%s", first_word, MAX_OPCODE_LENGTH);

    return get_opcode_num(first_word) > -1; /*If we get a positive value then the first word is a valid opcode*/
}

/* The main function used to pass over the input file */
static void pass_over_file(int pass_num, FILE* asm_program, FILE* output_file) {
    char* line = (char*)malloc(MAX_LINE_LENGTH);
    if (line == NULL) {
        printf("Error - malloc failed");
        exit(0);
    }
    char* base_line_ptr = line;
    int colon_index;

    while (fgets(line, MAX_LINE_LENGTH, asm_program) != NULL) {
        clear_leading_white_spaces(&line);
        if ((line[0] != '\0') && (is_line_comment(line) == 0)) { /*If the line is empty or just a comment we skip it*/
            colon_index = line_has_label(line); /*If line starts with label then returns ':' index else -1*/
            if (pass_num == 1 && colon_index != -1) {
                line[colon_index] = '\0'; /*Gets only the label itself*/
                label_t tmp_label;
                tmp_label.cmd_index = g_command_counter;
                strcpy_s(tmp_label.label, MAX_LABEL_LENGTH, line);

                g_labels_arr[g_label_count++] = tmp_label;
            }
            line += (colon_index + 1); /*Skips the label (if there is no label it won't do anything)*/

            clear_leading_white_spaces(&line);
            if (is_line_word_command(line)) { /*Checks if the line is a '.word' command*/
                if (pass_num == 1) { /*We save the data to memory only on the first pass*/
                    add_data_to_memory(line);
                }
            }
            else {
                if (line_has_command(line) == 1) { /*If the line have a command we count it*/
                    g_command_counter++;
                    if (pass_num == 2) { /*On the second pass we write the decoded command to the output file*/
                        decode_cmds_to_output_file(output_file, line);
                    }
                }
            }
            line = base_line_ptr; /*We reset the line to point to its original address,
                                    otherwise the calloc for the line might not be enough*/
        }
    }
    g_command_counter = 0;
    line = base_line_ptr; /* Make sure we free what we malloced */
    free(line);
}

int main(int argc, char const* argv[]) {
    FILE* asm_program, * output_cmd_file, * output_data_file;
    fopen_s(&asm_program, argv[1], "r");
    if (asm_program == NULL) {
        printf("Could not open assembly program file\n");
        exit(0);
    }
    fopen_s(&output_cmd_file, argv[2], "w");
    if (output_cmd_file == NULL) {
        printf("Could not open output cmd file\n");
        exit(0);
    }
    fopen_s(&output_data_file, argv[3], "w");
    if (output_data_file == NULL) {
        printf("Could not open output data file\n");
        exit(0);
    }

    pass_over_file(1, asm_program, output_cmd_file);
    rewind(asm_program);
    pass_over_file(2, asm_program, output_cmd_file);
    write_memory_file(output_data_file);
    fclose(asm_program);
    fclose(output_cmd_file);
    fclose(output_data_file);
}