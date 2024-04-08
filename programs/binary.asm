	INP
	STA input

#Main loop for each bit
bitLoop	LDA one

#Reset result
	STA result
	LDA bCount
#Handle n^0
	BRZ powEnd

#Modified from power.asm (starts with (bit number - 1) loaded)
#-----------------------
	SUB one
	STA power

powLoop	LDA powTwo
	SUB one
	STA pCount

#Save the result as the working value, then reset result
	LDA result
	STA working
	LDA zero
	STA result

#Multiply working value by original input
loop	LDA result
	ADD working
	STA result
	LDA pCount
	SUB one
	STA pCount
	BRP loop

	LDA power
	SUB one
	STA power
	BRP powLoop
#-----------------------

#power.asm's logic saves result to result
powEnd	LDA result
	STA bitVal

#Update the bit number
	LDA bCount
	SUB one
	STA bCount

#Exit when bit number is -2 (-1 is actually bit 0)
	ADD one
	BRP skip
	BRA end

#Process current bit
skip	LDA input
	SUB bitVal
	BRP bitHigh
	LDA zero
	OUT
	BRA bitLoop
bitHigh	STA input
	LDA one
	OUT
	BRA bitLoop

end	HLT

input	DAT 0
one	DAT 1
zero	DAT 0
bitVal	DAT 0
bCount	DAT 7

powTwo	DAT 2
result	DAT 1
working	DAT 0
pCount	DAT 0
power	DAT 0
