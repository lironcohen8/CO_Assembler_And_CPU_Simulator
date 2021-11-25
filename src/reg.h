#define CPU_REGS_NUM    (16)
#define IO_REGS_NUM     (23)

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