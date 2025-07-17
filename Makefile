AS = nasm
AFLAGS = -g -f bin

CC = gcc
CFLAGS = -Wall -Wextra -ggdb -march=i386 -mtune=i386 -m32 -std=c11 -nostdlib -nostartfiles -nodefaultlibs -ffreestanding -fno-PIC -masm=intel

BUILD_DIR = build

all: builddir link

run: all
	qemu-system-i386 -d cpu_reset -D log.txt -drive file=$(BUILD_DIR)/os.bin,format=raw,if=ide -device VGA -m 256M

link: bootloader kernel create_disk
	cat $(BUILD_DIR)/bootloader.bin $(BUILD_DIR)/kernel.bin $(BUILD_DIR)/drive.bin > $(BUILD_DIR)/os.bin

bootloader: $(BUILD_DIR)/bootloader.bin
kernel: $(BUILD_DIR)/kernel.bin linker.ld
create_disk: $(BUILD_DIR)/drive.bin

###

$(BUILD_DIR)/bootloader.bin: bootloader/boot.asm
	$(AS) $(AFLAGS) -o $@ $^

###

KSRS := kinit.c kmain.c vga.c keyboard.c ata.c fat.c mem.c
KOBJ := $(patsubst %.c, $(BUILD_DIR)/%.o, $(KSRS))

$(BUILD_DIR)/kernel.bin: $(KOBJ)
	ld -T linker.ld $^ -o $@

$(BUILD_DIR)/%.o: kernel/%.c
	$(CC) $(CFLAGS) -c -o $@ $^

$(BUILD_DIR)/%.o: kernel/%.asm
	$(AS) -g -f elf32 -o $@ $^

###

$(BUILD_DIR)/drive.bin: $(shell find disk/*)
	dd if=/dev/zero of=$(BUILD_DIR)/drive.bin bs=512 count=256
	/sbin/mkfs.fat -R 1 -r 112 -s 1 -S 512 -f 1 -F 12 -v -v $(BUILD_DIR)/drive.bin
	mcopy -i $(BUILD_DIR)/drive.bin -sQ disk/* ::

###

builddir:
	mkdir -p $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR)
