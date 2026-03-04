#Take 2 inputs to calculate the greatest common divisor
	INP
	STA a
	INP
	STA b

#Calculate a mod b, store it in c
modLoop	LDA a
	SUB b
	STA a
	BRP modLoop
	ADD b
	STA c

#Copy b to a, then c to b
	LDA b
	STA a
	LDA c
	STA b

#If c isn't 0, repeat
	BRZ done
	BRA modLoop

#Print the result
done	LDA a
	OUT
	HLT

a	DAT 0
b	DAT 0
c	DAT 0
