  lw 0 1 five
  lw 1 2 3
start add 1 2 1
  beq 0 1 2
  beq 0 0 start
  noop
done halt
five .fill # label not defined
Neg1 .fill -1
stAddr .fill start
