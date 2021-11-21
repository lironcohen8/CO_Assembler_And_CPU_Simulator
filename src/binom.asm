main:
	lw $a0, $imm2, $zero, $zero, 0, 100		# $a0 = n
	lw $a1, $imm2, $zero, $zero, 0, 101		# $a1 = k
	jal $ra, $zero, $zero, $imm2, 0, binom	# $v0 = binom(n,k)
	sw $zero, $zero, $imm2, $v0, 0, 102		# store binom(n,k) in 0x102
	halt $zero, $zero, $zero, $zero, 0, 0	# end
binom:
	add $sp, $sp, $imm2, $zero, 0, -4		# adjust stack for 3 items
	sw $s0, $sp, $imm2, $zero, 0, 3			# save $s0 in $sp+3
	sw $a0, $sp, $imm2, $zero, 0, 2			# save $a0 in $sp+2
	sw $a1, $sp, $imm2, $zero, 0, 1			# save $a1 in $sp+1
	sw $ra, $sp, $imm2, $zero, 0, 0			# save return address in $sp+0
infunc:
	beq $zero, $a1, $zero, $imm2, 0, base	# if k == 0 jump to base
	beq $zero, $a0, $a1, $imm2, 0, base		# if n == k jump to base
	add $a0, $a0, $zero, $imm2, 0, -1		# $a0 = n - 1
	add $a1, $a1, $zero, $imm2, 0, -1		# $a1 = k - 1
	jal $ra, $zero, $zero, $imm2, 0, binom	# $v0 = binom(n-1,k-1)
	add $s0, $v0, $zero, $zero, 0, 0		# $s0 = binom(n-1,k-1)
	add $a1, $a1, $zero, $imm2, 0, 1		# $a1 = k
	jal $ra, $zero, $zero, $imm2, 0, binom	# $v0 = binom(n-1,k)
	add $v0, $v0, $s0, $zero, 0, 0			# $v0 = binom(n-1,k-1) + binom(n-1,k)
	beq $zero, $zero, $zero, $imm2, 0, ret	# jump to ret
base:
	add $v0, $zero, $zero, $imm1, 0, 1		# $v0 = 1
ret:
	add $sp, $sp, $imm2, $zero, 0, -4		# adjust stack for 3 items
	lw $s0, $sp, $imm2, $zero, 0, 3			# load $s0 from $sp+3
	lw $a0, $sp, $imm2, $zero, 0, 2			# load $a0 from $sp+2
	lw $a1, $sp, $imm2, $zero, 0, 1			# load $a1 from $sp+1
	lw $ra, $sp, $imm2, $zero, 0, 0			# load return address from $sp+0
	beq $zero, $zero, $zero, $ra, 0, 0		# return