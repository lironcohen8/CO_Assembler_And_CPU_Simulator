#include <stdint.h>
#include "../reg.h"

#define DATA_MEMORY_SIZE            (4096)
#define MAX_ASSEMBLY_LINES          (4096)
#define INSTRUCTION_LINE_LEN        (12)
#define DATA_LINE_LEN               (8)
#define INPUT_INSTR_FILE_NAME       "imemin.txt"
#define INPUT_DATA_FILE_NAME        "dmemin.txt"

#define MONITOR_DIM                 (256)

#define DISK_SECTOR_SIZE            (512)
#define DISK_BUFFER_SIZE            DISK_SECTOR_SIZE / 4
#define DISK_SECTOR_NUM             (128)
#define DISK_HANDLING_TIME          (1024)

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
    opcode_e opcode    :  8;
    cpu_reg_e rd       :  4;
    cpu_reg_e rs       :  4;
    cpu_reg_e rt       :  4;
    cpu_reg_e rm       :  4;
    int imm1           : 12;
    int imm2           : 12;
    unsigned long long raw_cmd;
} asm_cmd_t;

typedef struct {
    unsigned char data[DISK_SECTOR_NUM][DISK_SECTOR_SIZE];
    unsigned int time_in_cmd;
} disk_t;