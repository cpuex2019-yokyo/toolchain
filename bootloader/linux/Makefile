TOOLPREFIX = riscv32-unknown-elf-

CC = $(TOOLPREFIX)gcc
AS = $(TOOLPREFIX)gas
LD = $(TOOLPREFIX)ld
OBJCOPY = $(TOOLPREFIX)objcopy
OBJDUMP = $(TOOLPREFIX)objdump

CFLAGS = -Wall -Werror -O -fno-omit-frame-pointer
CFLAGS += -mabi=ilp32 -march=rv32ima
CFLAGS += -MD
CFLAGS += -mcmodel=medany
CFLAGS += -ffreestanding -fno-common -nostdlib -mno-relax
CFLAGS += -I.
CFLAGS += $(shell $(CC) -fno-stack-protector -E -x c /dev/null >/dev/null 2>&1 && echo -fno-stack-protector)

LDFLAGS = -z max-page-size=4096

BUILD_DIR = build
OBJS = $(BUILD_DIR)/entry.o $(BUILD_DIR)/vector.o $(BUILD_DIR)/start.o $(BUILD_DIR)/printf.o $(BUILD_DIR)/uart.o $(BUILD_DIR)/virtio.o
TARGET = $(BUILD_DIR)/bootloader_board.elf $(BUILD_DIR)/bootloader_qemu.elf

default: $(TARGET)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/entry.o: entry.S $(BUILD_DIR)
	$(CC) $(CFLAGS) -c entry.S -o $(BUILD_DIR)/entry.o

$(BUILD_DIR)/vector.o: vector.S $(BUILD_DIR)
	$(CC) $(CFLAGS) -c vector.S -o $(BUILD_DIR)/vector.o

$(BUILD_DIR)/start.o: start.c $(BUILD_DIR)
	$(CC) $(CFLAGS) -c start.c -o $(BUILD_DIR)/start.o

$(BUILD_DIR)/virtio.o: virtio.c $(BUILD_DIR)
	$(CC) $(CFLAGS) -c virtio.c -o $(BUILD_DIR)/virtio.o

$(BUILD_DIR)/printf.o: printf.c $(BUILD_DIR)
	$(CC) $(CFLAGS) -c printf.c -o $(BUILD_DIR)/printf.o

$(BUILD_DIR)/uart.o: uart.c $(BUILD_DIR)
	$(CC) $(CFLAGS) -c uart.c -o $(BUILD_DIR)/uart.o

$(BUILD_DIR)/bootloader_board.elf: $(OBJS) $(BUILD_DIR)
	$(LD) $(LDFLAGS) -T bootloader_board.ld -o $(BUILD_DIR)/bootloader_board.elf $(OBJS)

$(BUILD_DIR)/bootloader_qemu.elf: $(OBJS) $(BUILD_DIR)
	$(LD) $(LDFLAGS) -T bootloader_qemu.ld -o $(BUILD_DIR)/bootloader_qemu.elf $(OBJS)

clean:
	rm -rf $(OBJS) $(TARGET) $(BUILD_DIR)
