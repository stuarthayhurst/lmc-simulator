start

#Load the next instruction, print it, increment it and store it
#Use the loop instruction as a counter since it's printed first
#Using a later address as a counter would mean its value differs from the source
loop	LDA start
	OUT
	LDA loop
	ADD one
	STA loop

#Determine if it has printed the length the whole program yet
	SUB load
	SUB final
	SUB one
	BRP end
	BRA loop

end	HLT

one	DAT 1

#Store the value of LDA and the final address
load	LDA start
final	DAT final
