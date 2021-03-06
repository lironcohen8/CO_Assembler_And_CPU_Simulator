MAIN:
	add $t0, $imm1, $zero, $zero, 7, 0 			# Sector num
	add $s0, $zero, $zero, $zero, 0, 0 			# Buffer
	out $zero, $imm1, $zero, $s0, 16, 0			# Set diskbuffer to $s0
	
FOR:	
	blt $zero, $t0, $imm1, $imm2, 0, RETURN 	# End of for loop
	add $t2, $t0, $imm1, $zero, 1, 0 			# Next sector num

  	jal $ra, $zero, $zero, $imm2, 0, WAIT		# Wait till disk is ready
	# Read data from sector
	out $zero, $imm1, $zero, $t0, 15, 0			# disksector = $t0
	out $zero, $imm1, $zero, $imm2, 14, 1		# diskcmd = 1 for read
	 
	jal $ra, $zero, $zero, $imm2, 0, WAIT		# Wait till disk is ready
	# Write data to sector
	out $zero, $imm1, $zero, $t2, 15, 0			# disksector = $t2 = $t0+1
	out $zero, $imm1, $zero, $imm2, 14, 2		# disckcmd = 2 for write

	add $t0, $t0, $imm1, $zero, -1, 0 			# Update sector num
	beq $zero, $zero, $zero, $imm2, 0, FOR		# Jump to next sector

WAIT:
	in $t1, $imm1, $zero, $zero, 17, 0			# Get diskstatus
	beq $zero, $t1, $imm1, $imm2, 1, WAIT		# While disk is busy
	beq $zero, $zero, $zero, $ra, 0, 0			# Return to loop
	
	
RETURN:
	halt $zero, $zero, $zero, $zero, 0, 0		# End	


