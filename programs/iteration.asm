          INP
          SUB one
          STA count

loopStart LDA dataStart
          OUT
          LDA loopStart
          ADD one
          STA loopStart
          LDA count
          SUB one
          STA count
          BRP loopStart

count     DAT 0
one       DAT 1

dataStart DAT 0
          DAT 1
          DAT 2
          DAT 3
          DAT 2
          DAT 1
          DAT 0
