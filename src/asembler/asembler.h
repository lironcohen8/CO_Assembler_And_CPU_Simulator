#include <stdint.h>
#include "reg.h"

#define MAX_LINE_LENGTH (500)
#define MAX_LABEL_LENGTH (50)

typedef struct {
    opcode_e opcode:  8;
    reg_e rd       :  4;
    reg_e rs       :  4;
    reg_e rt       :  4;
    reg_e rm       :  4;
    uint16_t imm1  : 12;
    uint16_t imm2  : 12;
} asm_line_t;

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
    HALT
} opcode_e;

