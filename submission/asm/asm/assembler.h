#include <stdint.h>

#define MAX_LINE_LENGTH  (500)
#define MAX_LABEL_LENGTH (50)
#define CPU_REGS_NUM     (16)

#define OPCODES_NUM             (22)
#define MAX_OPCODE_LENGTH       (5) /* length in chars 'halt' is 4 chars */
#define MAX_REG_LENGTH          (5) /* length in chars 'imm1' is 4 chars */
#define MAX_ASSEMBLY_LINES      (4096)
#define DATA_MEMORY_SIZE        (4096)

typedef struct {
    char label[MAX_LABEL_LENGTH];
    int cmd_index;
} label_t;


typedef enum {
    $ZERO,
    $IMM1,
    $IMM2,
    $V0,
    $A0,
    $A1,
    $A2,
    $T0,
    $T1,
    $T2,
    $S0,
    $S1,
    $S2,
    $GP,
    $SP,
    $RA
} cpu_reg_e;

typedef enum {
    irq0enable,
    irq1enable,
    irq2enable,
    irq0status,
    irq1status,
    irq2status,
    irqhandler,
    irqreturn,
    clks,
    leds,
    display7seg,
    timerenable,
    timercurrent,
    timermax,
    diskcmd,
    disksector,
    diskbuffer,
    diskstatus,
    reserved1,
    reserved2,
    monitoraddr,
    monitordata,
    monitorcmd
} io_reg_e;