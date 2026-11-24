# start.S — Step 2: print a whole message, using a loop.
#           (Step 2b: wait until the screen device is ready for each letter.)
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

wait_ready:             # The screen device needs time to send each letter.
                        # If we give it a new one too early, it gets LOST.
    lbu  t3, 5(t0)      # read the device's STATUS box (address t0 + 5)
    andi t3, t3, 0x20   # keep only bit 5, which means "ready for next letter"
    beqz t3, wait_ready # not ready yet -> check again

    sb   t2, 0(t0)      # ready! send that letter to the screen
    addi t1, t1, 1      # t1 = t1 + 1 -> move to the next letter's box
    j    loop           # go back to "loop" and do it again

hang:
    j    hang           # nothing left to do: wait forever


.section .rodata        # the part of memory for read-only data (text, constants)
message:
    .string "\r\nHello from Sigrun!\r\n"
                        # .string stores each letter as a number, one per box,
                        # and adds a 0 at the end automatically
