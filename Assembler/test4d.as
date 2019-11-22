  lw 0 1 five
  lw 1 2 3
start add 1 Neg1 1 # register B should be an integer 0 - 7
  beq 0 1 2
  beq 0 0 start
  noop
done halt
five .fill 5
Neg1 .fill -1
stAddr .fill start
