#include <stdint.h>
#include "../reg.h"

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
    reg_e rd       :  4;
    reg_e rs       :  4;
    reg_e rt       :  4;
    reg_e rm       :  4;
    int imm1       : 12;
    int imm2       : 12;
} asm_cmd_t;