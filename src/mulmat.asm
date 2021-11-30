.word 0x100 1
.word 0x101 1
.word 0x102 1
.word 0x103 1
.word 0x104 1
.word 0x105 1
.word 0x106 1
.word 0x107 1
.word 0x108 1
.word 0x109 1
.word 0x10A 1
.word 0x10B 1
.word 0x10C 1
.word 0x10D 1
.word 0x10E 1
.word 0x10F 1
.word 0x110 1
.word 0x111 1
.word 0x112 1
.word 0x113 1
.word 0x114 1
.word 0x115 1
.word 0x116 1
.word 0x117 1
.word 0x118 1
.word 0x119 1
.word 0x11A 1
.word 0x11B 1
.word 0x11C 1
.word 0x11D 1
.word 0x11E 1
.word 0x11F 1

Main:	
	add $s0, $0, $0, $imm1, 0x100, 0	#t0 set for the address of first value in mat A
	add $s1, $0, $0, $imm1, 0x110, 0	#t1 set for the address of first value in mat B 
	add $s2, $0, $0, $imm1, 0x120, 0	#t1 set for the address of first value in mat C 
	add $t0, $0, $0, $0, 0, 0		#Row counter
	add $t1, $0, $0, $0, 0, 0		#Column counter
	beq $0, $0, $0, LOOP, 0, 0	
	
LOOP:	
	#The next 3 lines get the address of the value in matrix C we want to update
	mac $t2, $t0, $imm1, $0, 4, 0
	add $s2, $s2, $t2, $0, 0, 0
	add $s2, $s2, $t1, $0, 0, 0
	
	add $s0, $s0, $t2, $0, 0, 0 	#Get $s0 to point to the start of the right row in mat A
	add $s1, $s1, $t1, $0, 0, 0	#Get $s1 to point to the start of the right col in mat B
	beq $0, $0, $0, CALC, 0, 0
	
CALC:	
	bgt $0, $s0, $imm1, COND, 0x10F, 0	#If we finished (If this is true the second check must also be true)
	bgt $0, $s1, $imm1, COND, 0x11F, 0
	
	lw $t2, $s0, $0, $0, 0, 0	#Get the current val from A
	lw $t3, $s1, $0, $0, 0, 0	#Get the current val from B
	mac $s2, $t2, $t3, $s2, 0, 0
	
	add $s0, $s0, $0, $imm1, 1, 0	#Get next A val in row
	add $s1, $s1, $0, $imm1, 4, 0	#Get next B val in col
	beq $0, $0, $0, CALC, 0, 0
	
COND:
	add $t1, $t1, $0, $imm1, 1, 0	#Advance to next column
	blt $0, $t1, $imm1, LOOP, 4, 0	#If we havent went through all the columns	
	
	add $t1, $s0, $0, $0, 0, 0	#Reset col counter 
	add $t0, $t0, $0, $imm1, 1, 0	#Advance to next row
	blt $0, $t0, $imm1, RESET, 4, 0	#Check if we have another row

RESET:
	add $s0, $imm1, $0, $0, 0x100, 0	#t0 set for the address of first value in mat A	
	add $s1, $imm1, $0, $0, 0x110, 0	#t1 set for the address of first value in mat B
	add $s2, $imm1, $0, $0, 0x120, 0	#t1 set for the address of first value in mat C
	beq $0, $0, $0, LOOP, 0, 0	
