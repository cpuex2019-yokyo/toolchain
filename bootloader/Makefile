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
OBJS = $(BUILD_DIR)/stager.o
TARGET = $(BUILD_DIR)/bootloader

default: $(TARGET)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/stager.o: stager.S $(BUILD_DIR)
	$(CC) $(CFLAGS) -c stager.S -o $(BUILD_DIR)/stager.o

$(TARGET): $(OBJS) $(BUILD_DIR)
	$(LD) $(LDFLAGS) -T bootloader.ld -o bootloader $(OBJS)

clean:
	rm -rf $(OBJS) $(TARGET) $(BUILD_DIR)
