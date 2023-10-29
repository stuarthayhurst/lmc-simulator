           INP
           STA input

#Main loop for each bit
bitStart   LDA one

#Reset result
           STA result
           LDA bitCount
#Handle n^0
           BRZ powerEnd

#Modified from power.asm (starts with (bit number - 1) loaded)
#-----------------------
           SUB one
           STA power

powerLoop  LDA powerTwo
           SUB one
           STA powerCount

#Save the result as the working value, then reset result
           LDA result
           STA working
           LDA zero
           STA result

#Multiply working value by original input
loop       LDA result
           ADD working
           STA result
           LDA powerCount
           SUB one
           STA powerCount
           BRP loop

           LDA power
           SUB one
           STA power
           BRP powerLoop
#-----------------------

#power.asm's logic saves result to result
powerEnd   LDA result
           STA bitVal

#Update the bit number
           LDA bitCount
           SUB one
           STA bitCount

#Exit when bit number is -2 (-1 is actually bit 0)
           ADD one
           BRP skip
           BRA end

#Process current bit
           skip LDA input
           SUB bitVal
           BRP bitHigh
           LDA zero
           OUT
           BRA bitStart
bitHigh    STA input
           LDA one
           OUT
           BRA bitStart

end        HLT

input      DAT 0
one        DAT 1
zero       DAT 0
bitVal     DAT 0
bitCount   DAT 7

powerTwo   DAT 2
result     DAT 1
working    DAT 0
powerCount DAT 0
power      DAT 0
