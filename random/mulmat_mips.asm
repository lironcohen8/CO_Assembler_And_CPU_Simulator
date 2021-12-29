Main:	
	addi $s0, $0, 0x100	#t0 set for the address of first value in mat A
	addi $s1, $0, 0x110	#t1 set for the address of first value in mat B 
	addi $s2, $0, 0x120	#t1 set for the address of first value in mat C 
	
	addi $t0, $0, 0	#Row counter
	addi $t1, $0, 0	#Column counter
	j LOOP		
	
LOOP:	
	#The next 3 lines puts gets the address of the current C val we calculate
	mul $t2, $t0, 4
	add $s2, $s2, $t2
	add $s2, $s2, $t1
	
	add $s0, $s0, $t2	#Get A mat to the right row
	add $s1, $s1, $t1	#Get B mat to the right col
	j CALC
	
CALC:
	bgt $s0, 0x10F, COND	#If we finished (If this is true the second one must also be true)
	bgt $s1, 0x11F, COND
	
	lw $t2, 0($s0)
	lw $t3, 0($s1)
	mul $t2, $t2, $t3
	add $s2, $s2, $t2
	
	add $s0, $s0, 1	#Get next A val in row
	add $s1, $s1, 4	#Get next B val in Col
	j CALC
	
COND:
	addi $t1, $t1, 1 	#Advance to next column
	blt $t1, 4, LOOP	#If we havent went through all the columns
	addi $t1, $0, 0		#Reset col counter
	addi $t0, $t0, 1	#Advance to next row
	blt $t0, 4, RESET	#If we have another row

RESET:
	addi $s0, $0, 0x100	#t0 set for the address of first value in mat A
	addi $s1, $0, 0x110	#t1 set for the address of first value in mat B 
	addi $s2, $0, 0x120	#t1 set for the address of first value in mat C 
	j LOOP	
