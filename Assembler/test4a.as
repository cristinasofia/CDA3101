  lw 0 1 five
  lw 1 2 3
start add 1 2 8 # destReg out of range 0 to 7
  beq 0 1 2
  beq 0 0 start
  noop
done halt
five .fill 5
Neg1 .fill -1
stAddr .fill start
