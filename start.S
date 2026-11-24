# start.S — Step 1: the smallest kernel possible.
#
# It does ONE thing: puts the letter H on the screen. Then it waits forever.
#
# Each line below is ONE instruction for the CPU.

.global _start          # "_start" is the name of the first instruction
_start:
    li   t0, 0x10000000 # put the number 0x10000000 into register t0
                        #   (that is the address of the screen device)
    li   t1, 72         # put the number 72 into register t1
                        #   (72 is the letter H)
    sb   t1, 0(t0)      # "store byte": copy t1 into the memory box at t0
                        #   -> the screen device gets 72 -> "H" appears!

hang:
    j    hang           # "jump to hang": go back one line, forever
