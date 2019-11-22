  lw 0 1 five
  lw Neg1 2 3 # register A should be an integer 0 - 7
start add 1 2 1
  beq 0 1 2
  beq 0 0 start
  noop
done halt
five .fill 5
Neg1 .fill -1
stAddr .fill start
