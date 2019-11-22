  lw 0 1 five
  lw 1 2 3
  add 1 2 1 # label previously defined at this address
  beq 0 1 2
  beq 0 0 start # no address for label, therefore undefined
  noop
done halt
five .fill 5
Neg1 .fill -1
stAddr .fill start
