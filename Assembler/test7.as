  sw 0 1 five
  sw 1 2 3
start nand 1 2 1
  cmov 0 1 2
  beq 0 0 start
  noop
done halt
five .fill 5
Neg10 .fill -10
stAddr .fill start
