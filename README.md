# CDA3101
Computer Organization 2

The LC2 is an 8-register, 32-bit computer. All addresses are word-addresses. The LC2 has 65536 words of memory. By assembly-language convention, register 0 will always contain the value 0.

There are 4 instruction formats (bit 0 is the least-significant bit).  Bits
31-25 are unused for all instructions, and should always be 0.

R-type instructions (add, nand, mult):
    bits 24-22: opcode
    bits 21-19: reg A
    bits 18-16: reg B
    bits 15-3:  unused (should all be 0)
    bits 2-0:   destReg

I-type instructions (lw, sw, beq):
    bits 24-22: opcode
    bits 21-19: reg A
    bits 18-16: reg B
    bits 15-0:  offsetField (an 16-bit, 2's complement number with a range of
		    -32768 to 32767)

O-type instructions (halt, noop):
    bits 24-22: opcode
    bits 21-0:  unused (should all be 0)
    
## Description of Assembler
Read and parse  assembly-language file to produce corresponding machine language.

## Description of Pipelining
A cycle-accurate simulator for a pipelined implementation of the LC2, complete with data forwarding and branch prediction.
