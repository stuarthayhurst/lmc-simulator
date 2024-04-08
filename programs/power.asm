	INP
	STA input
	INP
	BRZ end
	SUB one
	STA power

powLoop	LDA input
	SUB one
	STA count

#Save the result as the working value, then reset result
	LDA result
	STA working
	LDA zero
	STA result

#Multiply working value by original input
loop	LDA result
	ADD working
	STA result
	LDA count
	SUB one
	STA count
	BRP loop

	LDA power
	SUB one
	STA power
	BRP powLoop

end	LDA result
	OUT
	HLT

input	DAT 0
count	DAT 0
result	DAT 1
power	DAT 0
working	DAT 0
zero	DAT 0
one	DAT 1
