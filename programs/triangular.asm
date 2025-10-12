	INP
	SUB one
	STA count
	ADD one
	BRZ end

start	LDA i
	ADD one
	STA i
	ADD prev
	STA prev
	OUT
	LDA count
	SUB one
	STA count
	BRP start
end	HLT

i	DAT 0
prev	DAT 0
count	DAT 0
one	DAT 1
