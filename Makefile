BINARY = minimal

PREFIX 		:=riscv64-linux-gnu-

CC 		:=$(PREFIX)gcc

make:
	$(CC) -fno-pic -march=rv64imafd -g -ffreestanding -Wl,--gc-sections -nostartfiles -nostdlib -nodefaultlibs -Wl,-T,riscv64-virt.ld crt0.S minimal.c -o $(BINARY).elf

clean:
	rm $(BINARY).elf
