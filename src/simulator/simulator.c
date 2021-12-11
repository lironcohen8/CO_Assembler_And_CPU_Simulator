#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "simulator.h"

/* CPU registers (32 bits each) */
static int g_cpu_regs[CPU_REGS_NUM];
/* IO registers */
static int g_io_regs[IO_REGS_NUM];
/* Global flag indicating running in interrupt handler */
static int g_in_handler = False;
/* Gloabal monitor buffer */
static unsigned char g_monitor[MONITOR_DIM * MONITOR_DIM];
/* Data memory */
static int g_dmem[DATA_MEMORY_SIZE];
/* Global program counter */
static int g_pc = 0;
/* Global flag indicating program is running */
static int g_is_running;
/* Disk object */
static disk_t g_disk;

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
    g_in_handler = False;
    g_pc = g_io_regs[irqreturn];
}

static void in_cmd(cpu_reg_e rd, cpu_reg_e rs, cpu_reg_e rt, cpu_reg_e rm) {
    if (rd == $IMM1 || rd == $IMM2 || rd == $ZERO) {
        return;
    }
    if (g_io_regs[g_cpu_regs[rs] + g_cpu_regs[rt]] == monitorcmd) {
        g_cpu_regs[rd] = 0;
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

static void update_immediates(asm_cmd_t* cmd) {
    g_cpu_regs[$IMM1] = sign_extension_imm(cmd->imm1);
    g_cpu_regs[$IMM2] = sign_extension_imm(cmd->imm2);
}

static int is_jump_or_branch(opcode_e opcode) {
    return (opcode >= BEQ && opcode <= JAL) || (opcode == RETI);
}

static int is_irq() {
    return (g_io_regs[irq0enable] && g_io_regs[irq0status]) ||
           (g_io_regs[irq1enable] && g_io_regs[irq1status]) ||
           (g_io_regs[irq2enable] && g_io_regs[irq2status]);
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
    /* $imm1 and $imm2 should hold immediets value */
    update_immediates(cmd);
    /* Execute the command */
    cmds_ptr_arr[cmd->opcode](cmd->rd, cmd->rs, cmd->rt, cmd->rm);
}

static void load_instructions(FILE* instr_file, asm_cmd_t** cmd_arr) {
   char line_buffer[INSTRUCTION_LINE_LEN + 2];
   int instructions_count = 0;
   asm_cmd_t curr_cmd;
   /* stops when either (n-1) characters are read, or /n is read
   We want to read the /n char so it won't get in to the next line */
   while (fgets(line_buffer, INSTRUCTION_LINE_LEN + 2, instr_file) != NULL) {
       parse_line_to_cmd(line_buffer, &curr_cmd);
       (*cmd_arr)[instructions_count++] = curr_cmd;
   }
   (*cmd_arr) = (asm_cmd_t*)realloc(*cmd_arr, instructions_count * sizeof(asm_cmd_t));
}

// TODO VERIFY FORUM ABOUT ORDER
static void update_timer() {
    if (g_io_regs[timerenable] == True) {
        if (g_io_regs[timercurrent] == g_io_regs[timermax]) {
            g_io_regs[irq0status] = True;
            g_io_regs[timercurrent] = 0;
        } else {
            g_io_regs[timercurrent]++;
        }
    }
}

static void update_monitor() {
    if (g_io_regs[monitorcmd] == True) {
        g_monitor[g_io_regs[monitoraddr]] = g_io_regs[monitordata];
    }
}

static void update_disk() {
    if (g_io_regs[diskstatus] == False && g_io_regs[diskcmd] != 0) {
        /* Mark disk as busy */
        unsigned int sector = g_io_regs[disksector];
        unsigned int buffer_addr = g_io_regs[diskbuffer];
        g_io_regs[diskstatus] = True;
        if (g_io_regs[diskcmd] == 1) {
            /* Read command
            Copy from disk to memory */
            memcpy(&g_dmem[buffer_addr], g_disk.data[sector], DISK_SECTOR_SIZE);
        } else {
            /* Assuming legal command => Here is write command 
            Copy from memory to disk */
            memcpy(g_disk.data[sector], &g_dmem[buffer_addr], DISK_SECTOR_SIZE);
        }
    } else {
        if (g_io_regs[diskstatus] == True) {
            g_disk.time_in_cmd++;
            if (g_disk.time_in_cmd == DISK_HANDLING_TIME) {
                /* Finished operation */
                g_io_regs[diskstatus] = False;
                g_io_regs[diskcmd] = False;
                g_io_regs[irq1status] = True;
                g_disk.time_in_cmd = 0;
            }
        }
    }
}

static void load_data_memory(FILE* data_input_file) {
    char line_buffer[DATA_LINE_LEN + 2];
    int line_count = 0;
    /* stops when either (n-1) characters are read, or /n is read
    We want to read the /n char so it won't get in to the next line */
    while (fgets(line_buffer, DATA_LINE_LEN + 2, data_input_file) != NULL) {
        sscanf(line_buffer, "%X", &g_dmem[line_count++]);
    }
}

static void exec_instructions(asm_cmd_t* instructions_arr) {
    g_is_running = True;
    asm_cmd_t* curr_cmd;
    while (g_is_running) {
        update_timer();
        /* Check for interrupts */
        if (g_in_handler == False && is_irq()) {
            /* Now in interrupt handler */
            g_in_handler = True;
            /* Save return address */
            g_io_regs[irqreturn] = g_pc;
            /* Junp th handler */
            g_pc = g_io_regs[irqhandler];
        }
        /* Fetch current command to execute */
        curr_cmd = &instructions_arr[g_pc]; 
        /* Execute */
        exec_cmd(curr_cmd);
        /* Check for monitor updates */
        update_monitor();
        /* Check for disk updates */
        update_disk();
        /* If the command is not branch or jump than advance PC */
        if (!is_jump_or_branch(curr_cmd->opcode)) {
            g_pc++;
        }
    }
}


int main(int argc, char const *argv[])
{
    /* FOR DEBUGGING in launch.json
    "args": [
        "C:\\Users\\User1\\OneDrive\\Desktop\\CompStructProj\\ComputerOrganizationProject\\src\\assembler\\imemin.txt",
        "C:\\Users\\User1\\OneDrive\\Desktop\\CompStructProj\\ComputerOrganizationProject\\src\\assembler\\dmemin.txt"],
    */

    /* imemin.txt */
    FILE* input_cmd_file = fopen(argv[1], "r");
    /* dmemin.txt */
    FILE* input_data_file = fopen(argv[2], "r");
    asm_cmd_t* instr_arr = (asm_cmd_t*)malloc(MAX_ASSEMBLY_LINES * sizeof(asm_cmd_t));

    // TODO INIT DISK

    /* Load instructions file and store them in instr_arr */
    load_instructions(input_cmd_file, &instr_arr);
    /* Load data memory and store in g_dmem */
    load_data_memory(input_data_file);
    /* Execure program */
    exec_instructions(instr_arr);


    /* Update dmemout with the updatede memory */
    // TODO : put g_dmem to dmemout.txt 
    free(instr_arr);
    return 0;
}
