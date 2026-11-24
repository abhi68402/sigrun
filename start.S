# start.S — Step 2: print a whole message, using a loop.
#
# Idea: the message is stored in memory as a row of letters (numbers),
# ending with a 0. We send letters to the screen one by one, until we
# reach the 0.

.section .text          # the part of memory that holds instructions
.global _start
_start:
    li   t0, 0x10000000 # t0 = address of the screen device
    la   t1, message    # t1 = address of the FIRST letter of the message
                        #   ("la" = load address: the build tool fills in the number)

loop:
    lb   t2, 0(t1)      # "load byte": t2 = the number in the box at address t1
    beqz t2, hang       # "branch if equal to zero": if t2 is 0, the message
                        #   is finished -> jump to hang
    sb   t2, 0(t0)      # send that letter to the screen
    addi t1, t1, 1      # t1 = t1 + 1 -> move to the next letter's box
    j    loop           # go back to "loop" and do it again

hang:
    j    hang           # nothing left to do: wait forever


.section .rodata        # the part of memory for read-only data (text, constants)
message:
    .string "\r\nHello from Sigrun!\r\n"
                        # .string stores each letter as a number, one per box,
                        # and adds a 0 at the end automatically
