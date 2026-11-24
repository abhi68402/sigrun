# Makefile — the two commands we use.
#
#   make run   -> build kernel.elf, then start the fake computer
#   Quit QEMU with: Ctrl-A, then X

# Build: turn start.S (text) into kernel.elf (numbers).
# -T kernel.ld : use our map of where everything goes in memory
kernel.elf: start.S kernel.ld
	riscv64-elf-gcc -nostdlib -T kernel.ld -o kernel.elf start.S

# Run: start QEMU (the fake computer) with our kernel.
run: kernel.elf
	qemu-system-riscv64 -machine virt -nographic -bios default -kernel kernel.elf

.PHONY: run
