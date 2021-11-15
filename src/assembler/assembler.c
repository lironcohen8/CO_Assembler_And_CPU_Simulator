#include "assembler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <inttypes.h>

#define BASE_ARR_SIZE (10)
#define ARR_SIZE_MULTIPLY (2)
#define OPCODES_NUM (22)
#define MAX_OPCODE_LENGTH (5)
#define REGS_NUM (16)
#define MAX_ASSEMBLY_LINES (4096)
#define MAX_MEMORY_SIZE (4096)
#define OUTPUT_INSTR_FILE_NAME "imemin.txt"
#define OUTPUT_DATA_FILE_NAME "dmemin.txt"
#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))

int max_memory_index = 0;
int command_counter = 0;
label_t* labels_arr;
static int label_count = 0;
int dataMemory[MAX_MEMORY_SIZE];

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

static void decode_cmds_to_output_file(FILE *output_file, char *line)
{
    char opcode[10], rd[10], rs[10], rt[10], rm[10], imm1[32], imm2[32];
    sscanf(line, " %[^ $] $%[^,], $%[^,], $%[^,], $%[^,], %[^,], %s ",  opcode, rd, rs, rt, rm, imm1, imm2);

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

    fprintf(output_file, "%02X%01X%01X%01X%01X%03hhX%03hhX\n", opcode_d, rd_d, rs_d, rt_d, rm_d, imm1_d, imm2_d);
}


/*Line functions*/
static void clearLeadingWhitespaces(char** line){
    while (isspace((char)**line)){
        (*line)++;
    }
}

static int isLineComment(char* line){
    return line[0]=='#';
}

static int lineHasLabel(char* line){
    int i = 0;
    char c = line[0];

    while (c!='\0'){
        if (c=='#'){
            break;
        }
        if (c==':'){
            return i;
        }
        c = line[++i];
    }

    return -1;
}

static int isLineWord(char* line){
    return line[0]=='.';
}

static int labelLineHasCommand(char* line){
    char firstWord[MAX_OPCODE_LENGTH];
    sscanf(line,"%s",firstWord);
    return get_opcode_num(firstWord)>-1;
}

static void addDataToMemory(char* line){
    int address, value;
    sscanf(line, ".word %d %d", &address, &value);
    dataMemory[address] = value;
    max_memory_index = MAX(max_memory_index,address);
}

static void pass_over_file(int passNum, FILE* asm_program ,FILE* output_file){
    char* line = (char*)malloc(MAX_LINE_LENGTH);
    char* base_line_ptr = line;
    int colonIndex;
    char* tmp_label_str;
    if (passNum == 1) {
        labels_arr = (label_t *)calloc(BASE_ARR_SIZE, sizeof(label_t));
    }

    while (fgets(line, MAX_LINE_LENGTH, asm_program) != NULL){
        clearLeadingWhitespaces(&line); 
        if ((line[0] != '\n') && (isLineComment(line) == 0)){ /*If the line is empty or just a comment*/
            colonIndex = lineHasLabel(line); /*If line starts with label then returns ':' index else -1*/
            if (passNum == 1 && colonIndex!=-1){
                line[colonIndex] = '\0';
                tmp_label_str = (char *)malloc(colonIndex + 1);
                strcpy(tmp_label_str, line);

                label_t tmp_label = {
                    .label = tmp_label_str,
                    .cmd_index = command_counter}; 
                
                labels_arr[label_count++] = tmp_label;
            }
            line += (colonIndex + 1); /*Skip label*/
            clearLeadingWhitespaces(&line);
            if (line[0]=='.'){ /*Checks if the line is .word*/
                if (passNum == 1) {
                    addDataToMemory(line);
                }
            }
            else {
                if (labelLineHasCommand(line)==1){
                    command_counter++;
                    if (passNum == 2) {
                        decode_cmds_to_output_file(output_file, line);
                    }
                }
            }
            line = base_line_ptr;
        }
    }
}


int main(int argc, char const *argv[]) {
    FILE* asm_program = fopen(argv[1], "r");
    FILE *output_cmd_file = fopen(OUTPUT_INSTR_FILE_NAME, "w");

    pass_over_file(1, asm_program, output_cmd_file);
    rewind(asm_program);
    pass_over_file(2, asm_program, output_cmd_file);
    fclose(asm_program);
    fclose(output_cmd_file);
}