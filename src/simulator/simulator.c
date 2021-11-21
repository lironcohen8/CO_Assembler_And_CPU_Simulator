#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "simulator.h"

// CPU registers (32 bits each)
static int g_regs[REGS_NUM];
// Global program counter
static int g_pc;

static void add_cmd(reg_e, reg_e, reg_e, reg_e);
static void sub_cmd(reg_e, reg_e, reg_e, reg_e);
static void mac_cmd(reg_e, reg_e, reg_e, reg_e);
static void and_cmd(reg_e, reg_e, reg_e, reg_e);
static void or_cmd(reg_e, reg_e, reg_e, reg_e);

/* Array of function pointers used to call the right operation 
cmds_ptr_arr[opcode] is a function pointer to a function performing the 
opcode */
static void (*cmds_ptr_arr[])(reg_e, reg_e, reg_e, reg_e) = {
    add_cmd,
    sub_cmd,
    mac_cmd,
    and_cmd,
    or_cmd};

/* Command function each function corresponds to an opcode*/
static void add_cmd(reg_e rd, reg_e rs, reg_e rt, reg_e rm) {
    if (rd == $IMM1 || rd == $IMM2 || rd == $ZERO) {
        return;
    }
    g_regs[rd] = g_regs[rs] + g_regs[rt] + g_regs[rm];
}

static void sub_cmd(reg_e rd, reg_e rs, reg_e rt, reg_e rm) {
    if (rd == $IMM1 || rd == $IMM2 || rd == $ZERO) {
        return;
    }
    g_regs[rd] = g_regs[rs] - g_regs[rt] - g_regs[rm];
}

static void mac_cmd(reg_e rd, reg_e rs, reg_e rt, reg_e rm) {
    if (rd == $IMM1 || rd == $IMM2 || rd == $ZERO) {
        return;
    }
    g_regs[rd] = g_regs[rs] * g_regs[rt] + g_regs[rm];
}

static void and_cmd(reg_e rd, reg_e rs, reg_e rt, reg_e rm) {
    if (rd == $IMM1 || rd == $IMM2 || rd == $ZERO) {
        return;
    }
    g_regs[rd] = g_regs[rs] & g_regs[rt] & g_regs[rm];
}

static void or_cmd(reg_e rd, reg_e rs, reg_e rt, reg_e rm) {
    if (rd == $IMM1 || rd == $IMM2 || rd == $ZERO) {
        return;
    }
    g_regs[rd] = g_regs[rs] | g_regs[rt] | g_regs[rm];
}

static int sign_extension(int imm) {
    /* Any immediate is stored in 12 bits in SIMP instruction 
    This function assumes num is the shepe of 0x00000###
    Check if the 12th bit is on and extend accordingly */
    if (imm >= 0x800) { // 0x800 is 1000 0000 0000 in binary
        return imm | 0xFFFFF000;
    }
    return imm;
}

/* At each instruction if one of the registers holds is an immediate, set it's value */
static void set_reg_immediate(reg_e reg) {
     if (reg == $IMM1) {
        g_regs[$IMM1] = sign_extension(reg);
    } else if (reg == $IMM2) {
        g_regs[$IMM2] = sign_extension(reg);
    }
}

static void update_immediates(asm_cmd_t* cmd) {
    set_reg_immediate(cmd->rd);
    set_reg_immediate(cmd->rs);
    set_reg_immediate(cmd->rt);
    set_reg_immediate(cmd->rm);
}

static int is_jump_or_branch(opcode_e opcode) {
    return (opcode >= BEQ && opcode <= JAL) || (opcode == RETI);
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



int main(int argc, char const *argv[])
{
    /* code */
    return 0;
}
