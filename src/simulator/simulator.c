#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "simulator.h"

/* CPU registers (32 bits each) */
static int g_cpu_regs[CPU_REGS_NUM];
/* IO registers */
static int g_io_regs[IO_REGS_NUM];
/* Data memory */
static int g_dmem[DATA_MEMORY_SIZE];
/* Global program counter */
static int g_pc;
/* Global flag indicating program is running */
static int g_is_running;

static void add_cmd(cpu_reg_e, cpu_reg_e, cpu_reg_e, cpu_reg_e);
static void sub_cmd(cpu_reg_e, cpu_reg_e, cpu_reg_e, cpu_reg_e);
static void mac_cmd(cpu_reg_e, cpu_reg_e, cpu_reg_e, cpu_reg_e);
static void and_cmd(cpu_reg_e, cpu_reg_e, cpu_reg_e, cpu_reg_e);
static void or_cmd(cpu_reg_e, cpu_reg_e, cpu_reg_e, cpu_reg_e);
static void xor_cmd(cpu_reg_e, cpu_reg_e, cpu_reg_e, cpu_reg_e);
static void sll_cmd(cpu_reg_e, cpu_reg_e, cpu_reg_e, cpu_reg_e);
static void sra_cmd(cpu_reg_e, cpu_reg_e, cpu_reg_e, cpu_reg_e);
static void srl_cmd(cpu_reg_e, cpu_reg_e, cpu_reg_e, cpu_reg_e);
static void beq_cmd(cpu_reg_e, cpu_reg_e, cpu_reg_e, cpu_reg_e);
static void bne_cmd(cpu_reg_e, cpu_reg_e, cpu_reg_e, cpu_reg_e);
static void blt_cmd(cpu_reg_e, cpu_reg_e, cpu_reg_e, cpu_reg_e);
static void bgt_cmd(cpu_reg_e, cpu_reg_e, cpu_reg_e, cpu_reg_e);
static void ble_cmd(cpu_reg_e, cpu_reg_e, cpu_reg_e, cpu_reg_e);
static void bge_cmd(cpu_reg_e, cpu_reg_e, cpu_reg_e, cpu_reg_e);
static void jal_cmd(cpu_reg_e, cpu_reg_e, cpu_reg_e, cpu_reg_e);
static void lw_cmd(cpu_reg_e, cpu_reg_e, cpu_reg_e, cpu_reg_e);
static void sw_cmd(cpu_reg_e, cpu_reg_e, cpu_reg_e, cpu_reg_e);
static void reti_cmd(cpu_reg_e, cpu_reg_e, cpu_reg_e, cpu_reg_e);
static void in_cmd(cpu_reg_e, cpu_reg_e, cpu_reg_e, cpu_reg_e);
static void out_cmd(cpu_reg_e, cpu_reg_e, cpu_reg_e, cpu_reg_e);
static void halt_cmd(cpu_reg_e, cpu_reg_e, cpu_reg_e, cpu_reg_e);


/* Array of function pointers used to call the right operation 
cmds_ptr_arr[opcode] is a function pointer to a function performing the 
opcode */
static void (*cmds_ptr_arr[])(cpu_reg_e, cpu_reg_e, cpu_reg_e, cpu_reg_e) = {
    add_cmd,
    sub_cmd,
    mac_cmd,
    and_cmd,
    or_cmd,
    xor_cmd,
    sll_cmd,
    sra_cmd,
    srl_cmd,
    beq_cmd,
    bne_cmd,
    blt_cmd,
    bgt_cmd,
    ble_cmd,
    bge_cmd,
    jal_cmd,
    lw_cmd,
    sw_cmd,
    reti_cmd,
    in_cmd,
    out_cmd,
    halt_cmd
    };

/* Command function each function corresponds to an opcode*/
static void add_cmd(cpu_reg_e rd, cpu_reg_e rs, cpu_reg_e rt, cpu_reg_e rm) {
    if (rd == $IMM1 || rd == $IMM2 || rd == $ZERO) {
        return;
    }
    g_cpu_regs[rd] = g_cpu_regs[rs] + g_cpu_regs[rt] + g_cpu_regs[rm];
}

static void sub_cmd(cpu_reg_e rd, cpu_reg_e rs, cpu_reg_e rt, cpu_reg_e rm) {
    if (rd == $IMM1 || rd == $IMM2 || rd == $ZERO) {
        return;
    }
    g_cpu_regs[rd] = g_cpu_regs[rs] - g_cpu_regs[rt] - g_cpu_regs[rm];
}

static void mac_cmd(cpu_reg_e rd, cpu_reg_e rs, cpu_reg_e rt, cpu_reg_e rm) {
    if (rd == $IMM1 || rd == $IMM2 || rd == $ZERO) {
        return;
    }
    g_cpu_regs[rd] = g_cpu_regs[rs] * g_cpu_regs[rt] + g_cpu_regs[rm];
}

static void and_cmd(cpu_reg_e rd, cpu_reg_e rs, cpu_reg_e rt, cpu_reg_e rm) {
    if (rd == $IMM1 || rd == $IMM2 || rd == $ZERO) {
        return;
    }
    g_cpu_regs[rd] = g_cpu_regs[rs] & g_cpu_regs[rt] & g_cpu_regs[rm];
}

static void or_cmd(cpu_reg_e rd, cpu_reg_e rs, cpu_reg_e rt, cpu_reg_e rm) {
    if (rd == $IMM1 || rd == $IMM2 || rd == $ZERO) {
        return;
    }
    g_cpu_regs[rd] = g_cpu_regs[rs] | g_cpu_regs[rt] | g_cpu_regs[rm];
}

static void xor_cmd(cpu_reg_e rd, cpu_reg_e rs, cpu_reg_e rt, cpu_reg_e rm) {
    if (rd == $IMM1 || rd == $IMM2 || rd == $ZERO) {
        return;
    }
    g_cpu_regs[rd] = g_cpu_regs[rs] ^ g_cpu_regs[rt] ^ g_cpu_regs[rm];
}

static void sll_cmd(cpu_reg_e rd, cpu_reg_e rs, cpu_reg_e rt, cpu_reg_e rm) {
    if (rd == $IMM1 || rd == $IMM2 || rd == $ZERO) {
        return;
    }
    g_cpu_regs[rd] = g_cpu_regs[rs] << g_cpu_regs[rt];
}

static void sra_cmd(cpu_reg_e rd, cpu_reg_e rs, cpu_reg_e rt, cpu_reg_e rm) {
    if (rd == $IMM1 || rd == $IMM2 || rd == $ZERO) {
        return;
    }
    /* Arithmetic shift with sign extension is done sutomaticly by C */
    g_cpu_regs[rd] = g_cpu_regs[rs] >> g_cpu_regs[rt];
}

static void srl_cmd(cpu_reg_e rd, cpu_reg_e rs, cpu_reg_e rt, cpu_reg_e rm) {
    if (rd == $IMM1 || rd == $IMM2 || rd == $ZERO) {
        return;
    }
    /* Casting as unsigned should perform logical shift
    (adding zeros in MSB's) */
    g_cpu_regs[rd] = (unsigned) g_cpu_regs[rs] >> g_cpu_regs[rt];
}

static void beq_cmd(cpu_reg_e rd, cpu_reg_e rs, cpu_reg_e rt, cpu_reg_e rm) {
    if (g_cpu_regs[rs] == g_cpu_regs[rt]) {
        /* Branch if equal - set global pc to 12 LSB's of rm */
        g_pc = g_cpu_regs[rm] & 0x00000FFF;
    } else {
        /* Not equal - continue */
        g_pc++;
    }
}

static void bne_cmd(cpu_reg_e rd, cpu_reg_e rs, cpu_reg_e rt, cpu_reg_e rm) {
    if (g_cpu_regs[rs] != g_cpu_regs[rt]) {
        /* Branch if not equal - set global pc to 12 LSB's of rm */
        g_pc = g_cpu_regs[rm] & 0x00000FFF;
    } else {
        /* Not equal - continue */
        g_pc++;
    }
}

static void blt_cmd(cpu_reg_e rd, cpu_reg_e rs, cpu_reg_e rt, cpu_reg_e rm) {
    if (g_cpu_regs[rs] < g_cpu_regs[rt]) {
        /* Branch if less than- set global pc to 12 LSB's of rm */
        g_pc = g_cpu_regs[rm] & 0x00000FFF;
    } else {
        /* Not equal - continue */
        g_pc++;
    }
}

static void bgt_cmd(cpu_reg_e rd, cpu_reg_e rs, cpu_reg_e rt, cpu_reg_e rm) {
    if (g_cpu_regs[rs] > g_cpu_regs[rt]) {
        /* Branch if greater than- set global pc to 12 LSB's of rm */
        g_pc = g_cpu_regs[rm] & 0x00000FFF;
    } else {
        /* Not equal - continue */
        g_pc++;
    }
}

static void ble_cmd(cpu_reg_e rd, cpu_reg_e rs, cpu_reg_e rt, cpu_reg_e rm) {
    if (g_cpu_regs[rs] <= g_cpu_regs[rt]) {
        /* Branch if less or equal than- set global pc to 12 LSB's of rm */
        g_pc = g_cpu_regs[rm] & 0x00000FFF;
    } else {
        /* Not equal - continue */
        g_pc++;
    }
}

static void bge_cmd(cpu_reg_e rd, cpu_reg_e rs, cpu_reg_e rt, cpu_reg_e rm) {
    if (g_cpu_regs[rs] >= g_cpu_regs[rt]) {
        /* Branch if greater or equal than- set global pc to 12 LSB's of rm */
        g_pc = g_cpu_regs[rm] & 0x00000FFF;
    } else {
        /* Not equal - continue */
        g_pc++;
    }
}

static void jal_cmd(cpu_reg_e rd, cpu_reg_e rs, cpu_reg_e rt, cpu_reg_e rm) {
    /* Save next instruction address */
    g_cpu_regs[rd] = g_pc + 1;
    /* Jump */
    g_pc = g_cpu_regs[rm] & 0x00000FFF;
}

static void lw_cmd(cpu_reg_e rd, cpu_reg_e rs, cpu_reg_e rt, cpu_reg_e rm) {
    if (rd == $IMM1 || rd == $IMM2 || rd == $ZERO) {
        return;
    }
    g_cpu_regs[rd] = g_dmem[g_cpu_regs[rs] + g_cpu_regs[rt]] + g_cpu_regs[rm];
}

static void sw_cmd(cpu_reg_e rd, cpu_reg_e rs, cpu_reg_e rt, cpu_reg_e rm) {
    g_dmem[g_cpu_regs[rs] + g_cpu_regs[rt]] = g_cpu_regs[rm] + g_cpu_regs[rd];
}

static void reti_cmd(cpu_reg_e rd, cpu_reg_e rs, cpu_reg_e rt, cpu_reg_e rm) {
    g_pc = g_io_regs[irqreturn];
}

static void in_cmd(cpu_reg_e rd, cpu_reg_e rs, cpu_reg_e rt, cpu_reg_e rm) {
    if (rd == $IMM1 || rd == $IMM2 || rd == $ZERO) {
        return;
    }
    g_cpu_regs[rd] = g_io_regs[g_cpu_regs[rs] + g_cpu_regs[rt]];
}

static void out_cmd(cpu_reg_e rd, cpu_reg_e rs, cpu_reg_e rt, cpu_reg_e rm) {
    g_io_regs[g_cpu_regs[rs] + g_cpu_regs[rt]] = g_cpu_regs[rm];
}

static void halt_cmd(cpu_reg_e rd, cpu_reg_e rs, cpu_reg_e rt, cpu_reg_e rm) {
    g_is_running = False;
}

static int sign_extension_imm(int imm) {
    /* Any immediate is stored in 12 bits in SIMP instruction 
    This function assumes num is the shepe of 0x00000###
    Check if the 12th bit is on and extend accordingly */
    if (imm >= 0x800) { // 0x800 is 1000 0000 0000 in binary
        return imm | 0xFFFFF000;
    }
    return imm;
}

/* At each instruction if one of the registers holds is an immediate, set it's value */
static void set_reg_immediate(cpu_reg_e reg, asm_cmd_t* cmd) {
     if (reg == $IMM1) {
        g_cpu_regs[$IMM1] = sign_extension_imm(cmd->imm1);
    } else if (reg == $IMM2) {
        g_cpu_regs[$IMM2] = sign_extension_imm(cmd->imm2);
    }
}

static void update_immediates(asm_cmd_t* cmd) {
    set_reg_immediate(cmd->rd, cmd);
    set_reg_immediate(cmd->rs, cmd);
    set_reg_immediate(cmd->rt, cmd);
    set_reg_immediate(cmd->rm, cmd);
}

static int is_jump_or_branch(opcode_e opcode) {
    return (opcode >= BEQ && opcode <= JAL) || (opcode == RETI);
}

static void parse_line_to_cmd(char* line, asm_cmd_t* cmd) {
    /* Each command is 48 bits so 64 bits are required */
    unsigned long long raw;
    sscanf(line, "%llX", &raw);

    /* construct the command object */
    cmd->imm2 = raw & 0xFFF;
    cmd->imm1 = (raw >> 12) & 0xFFF;
    cmd->rm = (raw >> 24) & 0xF;
    cmd->rt = (raw >> 28) & 0xF;
    cmd->rs = (raw >> 32) & 0xF;
    cmd->rd = (raw >> 36) & 0xF;
    cmd->opcode = (raw >> 40) & 0xFF;
}

/* Execute a command using the functions pointers array */
static void exec_cmd(asm_cmd_t* cmd) {
    /* Check for immediates and update accordingly */
    update_immediates(cmd);
    /* Execute the command */
    cmds_ptr_arr[cmd->opcode](cmd->rd, cmd->rs, cmd->rt, cmd->rm);
    /* If the command is not branch or jump than advance PC */
    if (!is_jump_or_branch(cmd->opcode)) {
        g_pc++;
    }
}

void build_instructions_arr(FILE* instr_file, asm_cmd_t** asm_arr) {
   char line_buffer[INSTRUCTION_LINE_LEN];
   int instructions_count = 0;
   while (fgets(line_buffer, INSTRUCTION_LINE_LEN, instr_file) != NULL) {
       
   }
       
}


int main(int argc, char const *argv[])
{
    FILE* input_cmd_file = fopen(INPUT_INSTR_FILE_NAME, "w");


    int x;
    asm_cmd_t cmd;
    parse_line_to_cmd("00EE20000FFD", &cmd);
    exec_cmd(&cmd);
    sscanf("A0\n", "%X", &x);
    // int x = -3;
    int y = x>>1;
    int z = (unsigned) x>>1;
    printf("%d %d \n", y, z);
    return 0;
}
