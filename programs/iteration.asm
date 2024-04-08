	INP
	SUB one
	STA count

loop	LDA data
	OUT
	LDA loop
	ADD one
	STA loop
	LDA count
	SUB one
	STA count
	BRP loop

count	DAT 0
one	DAT 1

data	DAT 0
	DAT 1
	DAT 2
	DAT 3
	DAT 2
	DAT 1
	DAT 0
