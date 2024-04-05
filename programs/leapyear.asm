#Valid for years 1 - 999
	INP
	STA input
	BRA start

#To use, write to x and y, for x MOD y
#Set modEnd to branch + return address
mod	LDA x
	SUB y
	STA x
	BRZ done
	BRP mod
	ADD y
done	STA result
modEnd	DAT 0

#Prepare first call to mod
start	LDA branch
	ADD retAD
	STA modEnd

#Call mod with input and 400
	LDA input
	STA x
	LDA divA
	STA y
	BRA mod
retA	LDA result
	BRZ leap

#If it wasn't divisible by 400, check if it's divisible by 4 and not 100
#If it's not divisible by 4, it's not a leap year
	LDA branch
	ADD retBD
	STA modEnd
	LDA input
	STA x
	LDA divB
	STA y
	BRA mod
retB	LDA result
	BRZ cont
	BRA notLeap

#If it's divisible by 100, it's not a leap year
cont	LDA branch
	ADD retCD
	STA modEnd
	LDA input
	STA x
	LDA divC
	STA y
	BRA mod
retC	LDA result
	BRZ notLeap

#Must be a leap year
	BRA leap

#Print results
leap	LDA one
	OUT
	HLT
notLeap	LDA zero
	OUT
	HLT

input	DAT 0
x	DAT 0
y	DAT 0
result	DAT 0
one	DAT 1
zero	DAT 0

branch	DAT 600
retAD	DAT retA
retBD	DAT retB
retCD	DAT retC

divA	DAT 400
divB	DAT 4
divC	DAT 100
