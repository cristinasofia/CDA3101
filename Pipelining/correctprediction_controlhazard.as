  lw 0 2 data1
  lw 0 3 data2
  add 0 1 1
  beq 2 3 4
  add 0 1 1
  add 0 1 1
  add 0 1 1
  sw  0 5 data2
  halt
data1 .fill 23
data2 .fill 24
