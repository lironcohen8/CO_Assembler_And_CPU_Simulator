

.word 0x100 3
.word 0x101 23
.word 0x102 7
.word 0x103 4
.word 0x104 12
.word 0x105 6
.word 0x106 3
.word 0x107 13
.word 0x108 5
.word 0x109 100
.word 0x10A 0
.word 0x10B -12
.word 0x10C 0x100
.word 0x10D 7
.word 0x10E 19
.word 0x10F 8
.word 0x110 1
.word 0x111 2
.word 0x112 3
.word 0x113 4
.word 0x114 5
.word 0x115 6
.word 0x116 7
.word 0x117 8
.word 0x118 9
.word 0x119 10
.word 0x11A 11
.word 0x11B 12
.word 0x11C 13
.word 0x11D 14
.word 0x11e 15
.word 0x11f 16

Main:	add $s0, $zero, $zero, $imm1, 0X100, 0	# s0 set for the address of first value in mat A
	add $s1, $zero, $zero, $imm1, 0x110, 0	# s1 set for the address of first value in mat B 
	add $s2, $zero, $zero, $imm1, 0x120, 0	# s2 set for the address of first value in mat C 
	add $t0, $zero, $zero, $zero, 0, 0		

	add $sp, $sp, $imm2, $zero, 0, -2		# adjust stack for 2 items		
	sw $t0, $sp, $imm2, $zero, 0, 1			# save curr Row in $sp+1
	sw $t1, $sp, $imm2, $zero, 0, 0			# save curr Col in $sp+0	
	
LOOP:	
	lw $t0, $sp, $imm2, $zero, 0, 1			# Get current row
	lw $t1, $sp, $imm2, $zero, 0, 0			# Get current col

	# The next 3 lines get the address of the value in matrix C we want to update
	mac $t2, $t0, $imm1, $zero, 4, 0
	add $s2, $s2, $t2, $zero, 0, 0
	add $s2, $s2, $t1, $zero, 0, 0
	
	add $s0, $s0, $t2, $zero, 0, 0 			# Get $s0 to point to the start of the right row in mat A
	add $s1, $s1, $t1, $zero, 0, 0			# Get $s1 to point to the start of the right col in mat B

	add $t2, $zero, $zero, $zero, 0, 0
	
CALC:	
	bgt $zero, $s0, $imm1, $imm2, 0x10F, COND	# If we finished (If this is true the second check must also be true)
	bgt $zero, $s1, $imm1, $imm2, 0x11F, COND
	
	lw $t0, $s0, $zero, $zero, 0, 0			# Get the current val from A
	lw $t1, $s1, $zero, $zero, 0, 0			# Get the current val from B
	mac $t2, $t0, $t1, $t2, 0, 0

	add $s0, $s0, $zero, $imm1, 1, 0		# Get next A val in row
	add $s1, $s1, $zero, $imm1, 4, 0		# Get next B val in col
	beq $zero, $zero, $zero, $imm2, 0, CALC
	
COND:
	sw $t2, $s2, $zero, $zero, 0, 0 		# Store result in current val from C
	lw $t0, $sp, $imm2, $zero, 0, 1			# Get current row
	lw $t1, $sp, $imm2, $zero, 0, 0			# Get current col

	add $t1, $t1, $zero, $imm1, 1, 0		# Advance to next column
	sw $t1, $sp, $imm2, $zero, 0, 0			# save curr Col in $sp+0

	blt $zero, $t1, $imm1, $imm2, 4, RESET	# If we havent went through all the columns	
	
	add $t1, $zero, $zero, $zero, 0, 0		# Reset col counter 
	sw $t1, $sp, $imm2, $zero, 0, 0			# save curr Col in $sp+0

	add $t0, $t0, $zero, $imm1, 1, 0		# Advance to next row
	sw $t0, $sp, $imm2, $zero, 0, 1			# save curr row in $sp+1	

	blt $zero, $t0, $imm1, $imm2, 4, RESET	# Check if we have another row
	halt $zero, $zero, $zero, $zero, 0, 0	# end

RESET:
	add $s0, $imm1, $zero, $zero, 0x100, 0	# t0 set for the address of first value in mat A	
	add $s1, $imm1, $zero, $zero, 0x110, 0	# t1 set for the address of first value in mat B
	add $s2, $imm1, $zero, $zero, 0x120, 0	# t1 set for the address of first value in mat C
	beq $zero, $zero, $zero, $imm2, 0, LOOP	
