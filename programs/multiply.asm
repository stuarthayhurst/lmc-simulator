#Multiply 2 inputs
	INP
	STA num1
	INP
	SUB one
	STA num2

loop	LDA result
	ADD num1
	STA result
	LDA num2
	SUB one
	STA num2
	BRP loop
	LDA result
	OUT
	HLT

one	DAT 1
num1	DAT 0
num2	DAT 0
result	DAT 0
