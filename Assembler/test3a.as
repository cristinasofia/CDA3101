  lw 0 1 five
  lw 1 2 -32769 # offsetfield out of range -32768 to 32767 
start add 1 2 1
  beq 0 1 2
  beq 0 0 start
  noop
done halt
five .fill 5
Neg1 .fill -1
stAddr .fill start
