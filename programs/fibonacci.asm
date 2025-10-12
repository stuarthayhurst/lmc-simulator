	INP
	SUB one
	STA count
	ADD one
	BRZ end

start	LDA x
	ADD y
	STA next
	OUT
	LDA y
	STA x
	LDA next
	STA y
	LDA count
	SUB one
	STA count
	BRP start
end	HLT

#Use -1 and 0 as first 2 values, so 0 is the next
x	DAT -1
y	DAT 1
next	DAT 0
count	DAT 0
one	DAT 1
