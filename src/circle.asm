.word 0x100 10
main:
	lw $s0, $zero, $imm2, $zero, 0, 0x100		# $s0 = R from address 0x100
	mac $s0, $s0, $s0, $zero, 0, 0			# $s0 = R^2
	add $t0, $zero, $zero, $zero, 0, 0		# $t0 = index = 0
	add $s1, $zero, $zero, $imm1, 65536, 0	# $s1 = 65536
	add $s2, $zero, $zero, $imm1, -256, 0	# $s2 = -256
loop:
	bgt $zero, $t0, $s1, $imm2, 0, end		# if index >= 65536 jump to end
	sra $t1, $t0, $imm1, $zero, 16, 0		# $t1 = $t0 / 256 = i
	mac $t1, $t1, $t1, $zero, 0, 0			# $t1 = $t1 * $t1 = i^2
	mac $t2, $s2, $t1, $t0, 0, 0			# $t2 = -256*(index/256)+index = index%256 = j
	mac $t2, $t2, $t2, $zero, 0, 0			# $t2 = $t2 * $t2 = j^2
	add $t1, $t1, $t2, $zero, 0, 0			# $t2 = i^2 + j^2
	bgt $zero, $s0, $t2, $imm2, 0, inc		# if out of circle jump to inc
	out $zero, $zero, imm2, $t0, 0, 20 		# if in circle set index as address
	out $zero, $zero, $imm2, $imm1, 255, 21	# set pixel color to white
	out $zero, $zero, $imm2, $imm1, 1, 22	# draw pixel
inc:
	add $t0, $zero, $imm1, $zero, 1, 0		# i++;
	beq $zero, $zero, $zero, $imm2, 0, loop	# jump to loop
end:
	halt $zero, $zero, $zero, $zero, 0, 0	# halt