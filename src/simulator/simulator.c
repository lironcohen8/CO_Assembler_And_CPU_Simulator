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
/* Global monitor buffer */
static unsigned char g_monitor[MONITOR_DIM * MONITOR_DIM];
/* Data memory */
static int g_dmem[DATA_MEMORY_SIZE];
/* Global program counter */
static int g_pc = 0;
/* Global flag indicating program is running */
static int g_is_running;
/* Disk object */
static disk_t g_disk;
/* hwregtrace file */
static FILE* g_hw_reg_trace_file;
/* leds file */
static FILE* g_leds_file;
/* 7 segment file */
static FILE* g_7segment_file;

static const char *g_io_regs_arr[] = {"irq0enable",
                                      "irq1enable",
                                      "irq2enable",
                                      "irq0status",
                                      "irq1status",
                                      "irq2status",
                                      "irqhandler",
                                      "irqreturn",
                                      "clks",
                                      "leds",
                                      "display7seg",
                                      "timerenable",
                                      "timercurrent",
                                      "timermax",
                                      "diskcmd",
                                      "disksector",
                                      "diskbuffer",
                                      "diskstatus",
                                      "reserved1",
                                      "reserved2",
                                      "monitoraddr",
                                      "monitordata",
                                      "monitorcmd"};

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

static void update_hw_reg_trace_file(char *type, int io_reg_index, int data) {
    fprintf(g_hw_reg_trace_file, "%d %s %s %08X" ,g_io_regs[clks], type, g_io_regs_arr[io_reg_index], data);
}

static void update_leds_file() {
    fprintf(g_leds_file, "%d %08X\n", g_io_regs[clks], g_io_regs[leds]);
}

static void update_7segment_file() {
    fprintf(g_7segment_file, "%d %08X\n", g_io_regs[clks], g_io_regs[display7seg]);
}

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
    update_hw_reg_trace_file("READ", g_cpu_regs[rs] + g_cpu_regs[rt], g_cpu_regs[rd]);
}

static void out_cmd(cpu_reg_e rd, cpu_reg_e rs, cpu_reg_e rt, cpu_reg_e rm) {
    g_io_regs[g_cpu_regs[rs] + g_cpu_regs[rt]] = g_cpu_regs[rm];
    update_hw_reg_trace_file("WRITE", g_cpu_regs[rs] + g_cpu_regs[rt], g_cpu_regs[rm]);
    if (g_cpu_regs[rs] + g_cpu_regs[rt] == leds) {
        update_leds_file();
    }
    if (g_cpu_regs[rs] + g_cpu_regs[rt] == display7seg) {
        update_7segment_file();
    }
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
    cmd->raw_cmd = raw;
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

static void update_trace_file(FILE* output_trace_file, asm_cmd_t* curr_cmd) {
    /* Writes the trace file for current variables status */
    fprintf("%03X %12X 00000000 %08X %08X %08X %08X %08X %08X %08X %08X %08X %08X %08X %08X %08X %08X %08X\n",
            g_pc, 
            curr_cmd->raw_cmd, 
            sign_extension_imm(curr_cmd->imm1), 
            sign_extension_imm(curr_cmd->imm2), 
            g_cpu_regs[$V0],
            g_cpu_regs[$A0], 
            g_cpu_regs[$A1],
            g_cpu_regs[$A2], 
            g_cpu_regs[$T0],
            g_cpu_regs[$T1],
            g_cpu_regs[$T2],
            g_cpu_regs[$S0],
            g_cpu_regs[$S1],
            g_cpu_regs[$S2],
            g_cpu_regs[$GP],
            g_cpu_regs[$SP],
            g_cpu_regs[$RA]);
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

static void load_disk_file(char *file_name) {
    FILE* diskin_file = fopen(file_name, "r");
    char line_buffer[DATA_LINE_LEN + 2];
    int line_count = 0;
    /* stops when either (n-1) characters are read, or /n is read
    We want to read the /n char so it won't get in to the next line */
    while (fgets(line_buffer, DATA_LINE_LEN + 2, diskin_file) != NULL) {
        sscanf(line_buffer, "%X", &g_disk[line_count / DISK_SECTOR_SIZE][line_count % DISK_SECTOR_SIZE]);
        line_count++;
    }
}

static void exec_instructions(asm_cmd_t* instructions_arr, FILE* output_trace_file) {
    g_is_running = True;
    asm_cmd_t* curr_cmd;
    while (g_is_running) {
        /* Update timer and clock cycles number*/
        update_timer();
        g_io_regs[clks] = ((unsigned int)g_io_regs[clks])++;
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
        /* Update trace file before executing cuurent command */
        update_trace_file(output_trace_file, curr_cmd);
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

static void write_memory_file(char *file_name) {
    /* Writes the memory data file */
    FILE* output_memory_file = fopen(file_name, "w");
    for (int i=0; i<=DATA_MEMORY_SIZE; i++){
        fprintf(output_memory_file,"%08X\n",g_dmem[i]);
    }
    fclose(output_memory_file);
}

static void write_regs_file(char *file_name) {
    /* Writes the regs file */
    FILE* output_cycles_file = fopen(file_name, "w");
    for (int i=3; i<=CPU_REGS_NUM; i++){
        fprintf(output_cycles_file,"%08X\n",g_cpu_regs[i]);
    }
    fclose(output_cycles_file);
}

static void write_cycles_file(char *file_name) {
    FILE* output_cycles_file = fopen(file_name, "w");
    fprintf(output_cycles_file, g_io_regs[clks]);
    fclose(output_cycles_file);
}

static void write_disk_file(char *file_name) {
    /* Writes the disk data file */
    FILE* output_disk_file = fopen(file_name, "w");
    for (int i=0; i<=DISK_SECTOR_NUM; i++){
        for (int j=0; j<=DISK_SECTOR_SIZE; j++){
            fprintf(output_disk_file,"%08X\n",(g_disk.data)[i][j]);
        }
    }
    fclose(output_disk_file);
}

static void write_monitor_files(char *file_txt_name, char *file_yuv_name) {
    /* Writes the monitor data file */
    FILE* output_monitor_data_file = fopen(file_txt_name, "w");
    for (int i=0; i<=MONITOR_DIM*MONITOR_DIM; i++) {
        fprintf(output_monitor_data_file,"%02X\n",(g_monitor)[i]);
    }
    fclose(output_monitor_data_file);

    /* Writes the monitor binary file */
    FILE* output_monitor_binary_file = fopen(file_yuv_name, "wb");
    fwrite(g_monitor, sizeof(char), MONITOR_DIM*MONITOR_DIM, output_monitor_binary_file);
    fclose(output_monitor_binary_file);
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

    /* trace.txt */
    FILE* output_trace_file = fopen(argv[7], "w");

    /* hw reg trace file */
    g_hw_reg_trace_file = fopen(argv[8], "w");

    /* leds file */
    g_leds_file = fopen(argv[10], "w"); 

    /* 7segment file */
    g_7segment_file = fopen(argv[11], "w");   

    // TODO INIT DISK

    /* Load instructions file and store them in instr_arr */
    load_instructions(input_cmd_file, &instr_arr);

    /* Load data memory and store in g_dmem */
    load_data_memory(input_data_file);

    /* Execure program */
    exec_instructions(instr_arr, &output_trace_file);

    /* Write dmemout file with the update memory */
    write_memory_file(argv[5]);

    /* Update regout file with the update regs values */
    write_regs_file(argv[6]);

    /* Write to cycles file */
    write_cycles_file(argv[9]);

    /* Write to diskout file */
    write_disk_file(argv[12]);
    
    /* Write monitor files */
    write_monitor_files(argv[13], argv[14]);

    fclose(g_hw_reg_trace_file);
    fclose(g_leds_file);
    fclose(g_7segment_file);

    free(instr_arr);
    return 0;
}
