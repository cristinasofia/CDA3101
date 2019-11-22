Assembler.cc

--------------------------------------------
--------------------------------------------
Two-Pass Assembler:
--------------------------------------------
--------------------------------------------

First collects symbols and addresses in table. Checks
for duplicates, valid syntax, and unrecognized opcodes.
Second pass calculates instructions using bitwise operations
on 32-bit integer types. Checks for undefined labels and
overflow when calculating offsetField.

3 Instruction Formats:
--------------------------------------------
R-Type
add (000), nand (001), cmov (101)
--------------------------------------------
bits 24-22: opcode
bits 21-19: reg A
bits 18-16: reg B
bits 15-3:  unused (should all be 0)
bits 2-0:   destReg (allowed registers 0 - 7)

--------------------------------------------
I-Type
lw (010), sw, (011), beq (100)
--------------------------------------------
bits 24-22: opcode
bits 21-19: reg A
bits 18-16: reg B
bits 15-0:  offsetField (16-bit, range of -2^15 to 2^15 - 1)*

--------------------------------------------
O-Type
halt (110), noop (111)
--------------------------------------------
bits 24-22: opcode
bits 21-0:  unused (should all be 0)

* The range of signed numbers is represented by
-(2^(N-1)) to (2^(N-1)-1) where N = number of bits

--------------------------------------------
--------------------------------------------
TEST CASES:
--------------------------------------------
--------------------------------------------

test1.as:
Example given in original project write-up.
Test cases 2 - 6 have the same instructions, but have
one line that was changed for each type of error check.

test2.as:
Error checking: Duplicate labels.

test3a.as, test3b.as:
Error checking: Overflow for offsetField.

test4a.as, test4b.as, test4c.as, test4d.as:
Error checking: Valid entry for format (syntax).

test5a.as, test5b.as:
Error checking: Undefined labels.

test6.as:
Error checking: Unrecognized opcode.

test7.as:
All possible combinations of opcode (not already tested in
previous examples) with testing for both numeric
or symbolic addresses.
