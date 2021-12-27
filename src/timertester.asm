    out $zero, $zero, $imm2, $imm1, 1, 0		# enable irq0
    out $zero, $zero, $imm2, $imm1, 100, 13		# set timermax to 100 cycles
    out $zero, $imm1, $zero, $imm2, 6, L3		# set irqhandler as L1
    add $s0, $imm1, $zero, $zero, 7, 0          # numbrr of timer interrupts to count
    add $s1, $imm1, $zero, $zero, 0, 0          # counter for timer interrupts
    out $zero, $zero, $imm2, $imm1, 1, 11		# enable timer
WAIT:
    beq $zero, $s1, $s0, $imm2, 1, EXIT		    # if we did n intrrupts exit
    beq $zero, $zero, $zero, $imm2, 0, WAIT		# jump to WAIT
EXIT:
    halt $zero, $zero, $zero, $zero, 0, 0		# End	
L3:
    add $s1, $s1, $zero, $imm1, 1, 0            # count interrupt
    out $zero, $zero, $imm2, $zero, 0, 3		# clear irq0 status
    reti $zero, $zero, $zero, $zero, 0, 0		# return from interrupt
