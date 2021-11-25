#include <stdint.h>
#include "../reg.h"

#define DATA_MEMORY_SIZE         (4096)

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