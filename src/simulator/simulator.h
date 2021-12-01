#include <stdint.h>
#include "../reg.h"

#define DATA_MEMORY_SIZE            (4096)
#define MAX_ASSEMBLY_LINES          (4096)
#define INSTRUCTION_LINE_LEN        (12)
#define INPUT_INSTR_FILE_NAME       "imemin.txt"
#define INPUT_DATA_FILE_NAME        "dmemin.txt"

#define False                       (0)
#define True                        (1)

typedef enum {
    ADD,
    SUB,
    MAC,
    AND,
    OR,
    XOR,
    SLL,
    SRA,
    SRL,
    BEQ,
    BNE,
    BLT,
    BGT,
    BLE,
    BGE,
    JAL,
    LW,
    SW,
    RETI,
    IN,
    OUT,
    HALT,
    OPCODES_NUM
} opcode_e;

typedef struct {
    opcode_e opcode:  8;
    cpu_reg_e rd       :  4;
    cpu_reg_e rs       :  4;
    cpu_reg_e rt       :  4;
    cpu_reg_e rm       :  4;
    int imm1       : 12;
    int imm2       : 12;
} asm_cmd_t;