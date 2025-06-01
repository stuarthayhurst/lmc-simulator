#Modular division between 2 inputs
	INP
	STA val
	INP
	STA mod

loop	LDA val
	SUB mod
	STA val
	BRP loop
	ADD mod
	OUT
	HLT

val	DAT 0
mod	DAT 0
