AS = nasm
AFLAGS = -g -f bin

CC = gcc
CFLAGS = -Wall -Wextra -ggdb -march=i386 -mtune=i386 -m32 -std=c11 -nostdlib -nostartfiles -nodefaultlibs -ffreestanding -fno-PIC -masm=intel

BUILD_DIR = build

all: builddir link

run: build
	qemu-system-i386 -d cpu_reset -D log.txt -drive file=$(BUILD_DIR)/os.bin,format=raw,if=ide -device VGA

link: bootloader kernel
	cat $(BUILD_DIR)/bootloader.bin $(BUILD_DIR)/kernel.bin > $(BUILD_DIR)/os.bin

bootloader: $(BUILD_DIR)/bootloader.bin
kernel: $(BUILD_DIR)/kernel.bin linker.ld

$(BUILD_DIR)/bootloader.bin: bootloader/boot.asm
	$(AS) $(AFLAGS) -o $@ $^

$(BUILD_DIR)/kernel.bin: $(BUILD_DIR)/kmain.o $(BUILD_DIR)/kinit.o $(BUILD_DIR)/vga.o $(BUILD_DIR)/keyboard.o
	ld -T linker.ld $^ -o $@

$(BUILD_DIR)/%.o: kernel/%.c
	$(CC) $(CFLAGS) -c -o $@ $^

$(BUILD_DIR)/%.o: kernel/%.asm
	$(AS) -g -f elf32 -o $@ $^

builddir:
	mkdir -p $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR)
