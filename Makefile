# Makefile — build and run the kernel.
#   make        -> build build/kernel.elf
#   make run    -> boot it in QEMU (quit: Ctrl-A then X)
#   make debug  -> boot paused, waiting for GDB (run `make gdb` in another terminal)
#   make clean  -> delete build output

CC      = riscv64-elf-gcc
GDB     = riscv64-elf-gdb
QEMU    = qemu-system-riscv64

# -ffreestanding / -nostdlib : no operating system underneath us, no libc
# -mcmodel=medany            : our code lives at high addresses (0x80200000)
CFLAGS  = -march=rv64gc -mabi=lp64d -mcmodel=medany \
          -ffreestanding -nostdlib -fno-builtin -fno-tree-loop-distribute-patterns \
          -O2 -g -Wall -Wextra -Werror
LDFLAGS = -T kernel/kernel.ld -nostdlib

SRCS    = kernel/entry.S kernel/trap.S kernel/main.c kernel/uart.c \
          kernel/printf.c kernel/string.c kernel/trap.c \
          kernel/sbi.c kernel/timer.c
OBJS    = $(patsubst kernel/%,build/%.o,$(SRCS))

QEMUFLAGS = -machine virt -smp 1 -m 128M -nographic -bios default -kernel build/kernel.elf

build/kernel.elf: $(OBJS) kernel/kernel.ld
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJS)

build/%.o: kernel/% kernel/*.h
	@mkdir -p build
	$(CC) $(CFLAGS) -c -o $@ $<

run: build/kernel.elf
	$(QEMU) $(QEMUFLAGS)

debug: build/kernel.elf
	$(QEMU) $(QEMUFLAGS) -s -S

gdb:
	$(GDB) build/kernel.elf -ex "target remote :1234" -ex "break kmain" -ex "continue"

clean:
	rm -rf build

.PHONY: run debug gdb clean
