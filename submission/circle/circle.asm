.word 0x100 80
MAIN:
	lw $s0, $zero, $imm2, $zero, 0, 0x100	# $s0 = R from address 0x100
	mac $s0, $s0, $s0, $zero, 0, 0			# $s0 = R^2
	add $t0, $zero, $zero, $zero, 0, 0		# $t0 = index = 0
	add $s2, $zero, $zero, $imm1, 256, 0	# $s2 = 256
	mac $s1, $s2, $s2, $zero, 0, 0			# $s1 = monitor size 65536
	add $s2, $zero, $zero, $imm1, 255, 0	# $s2 = 255
LOOP:
	bge $zero, $t0, $s1, $imm2, 0, END		# if index >= 65536 jump to END
	sra $t1, $t0, $imm1, $zero, 8, 0		# $t1 = index / 256 = i
	and $t2, $t0, $s2, $s2, 0, 0			# $t2 = index & 255 = j
	sub $t1, $t1, $imm1, $zero, 128, 0		# $t1 = i - 128
	sub $t2, $t2, $imm1, $zero, 128, 0		# $t2 = j - 128
	mac $t1, $t1, $t1, $zero, 0, 0			# $t1 = $t1 * $t1 = (i-128)^2
	mac $t2, $t2, $t2, $zero, 0, 0			# $t2 = $t2 * $t2 = (j-128)^2
	add $t1, $t1, $t2, $zero, 0, 0			# $t1 = (i-128)^2 + (j-128)^2
	bgt $zero, $t1, $s0, $imm2, 0, INC		# if out of circle jump to INC
	out $zero, $zero, $imm2, $t0, 0, 20 	# if in circle set index as address
	out $zero, $zero, $imm2, $imm1, 255, 21	# set pixel color to white
	out $zero, $zero, $imm2, $imm1, 1, 22	# draw pixel
INC:
	add $t0, $t0, $imm1, $zero, 1, 0		# index++
	beq $zero, $zero, $zero, $imm2, 0, LOOP	# jump to LOOP
END:
	halt $zero, $zero, $zero, $zero, 0, 0	# halt