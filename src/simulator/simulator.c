#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "simulator.h"

// CPU registers (32 bits each)
static int g_regs[REGS_NUM];

/* Array of function pointers used to call the right operation 
cmds_ptr_arr[opcode] is a function pointer to a function performing the 
opcode */
static void (*cmds_ptr_arr[])(reg_e, reg_e, reg_e, reg_e) = {
    add_cmd,
    sub_cmd,
    mac_cmd,
    and_cmd,
    or_cmd};

/* Execute a command using the functions pointers array */
static void exec_cmd(asm_cmd_t* cmd) {
    cmds_ptr_arr[cmd->opcode](cmd->rd, cmd->rs, cmd->rt, cmd->rm);
}

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

int main(int argc, char const *argv[])
{
    /* code */
    return 0;
}
