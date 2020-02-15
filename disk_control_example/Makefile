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
OBJS = $(BUILD_DIR)/entry.o $(BUILD_DIR)/start.o $(BUILD_DIR)/printf.o $(BUILD_DIR)/uart.o $(BUILD_DIR)/virtio.o
TARGET = $(BUILD_DIR)/diskc.elf $(BUILD_DIR)/diskc.bin $(BUILD_DIR)/diskc.coe $(BUILD_DIR)/rawdisk

default: $(TARGET)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/entry.o: entry.S $(BUILD_DIR)
	$(CC) $(CFLAGS) -c entry.S -o $(BUILD_DIR)/entry.o

$(BUILD_DIR)/start.o: start.c $(BUILD_DIR)
	$(CC) $(CFLAGS) -c start.c -o $(BUILD_DIR)/start.o

$(BUILD_DIR)/virtio.o: virtio.c $(BUILD_DIR)
	$(CC) $(CFLAGS) -c virtio.c -o $(BUILD_DIR)/virtio.o

$(BUILD_DIR)/printf.o: printf.c $(BUILD_DIR)
	$(CC) $(CFLAGS) -c printf.c -o $(BUILD_DIR)/printf.o

$(BUILD_DIR)/uart.o: uart.c $(BUILD_DIR)
	$(CC) $(CFLAGS) -c uart.c -o $(BUILD_DIR)/uart.o

$(BUILD_DIR)/diskc.elf: $(OBJS) $(BUILD_DIR)
	$(LD) $(LDFLAGS) -T diskcontrol.ld -o $(BUILD_DIR)/diskc.elf $(OBJS)

$(BUILD_DIR)/diskc.bin: $(OBJS) $(BUILD_DIR)
	yokyo_elf2bin $(BUILD_DIR)/diskc.elf $(BUILD_DIR)/diskc.bin 

$(BUILD_DIR)/diskc.coe: $(OBJS) $(BUILD_DIR)
	yokyo_bin2coe $(BUILD_DIR)/diskc.bin > $(BUILD_DIR)/diskc.coe

$(BUILD_DIR)/rawdisk: $(BUILD_DIR)
	python -c "for i in range(0, 0xFF+1): print(chr(i) * 1024)" > $(BUILD_DIR)/rawdisk

clean:
	rm -rf $(OBJS) $(TARGET) $(BUILD_DIR)

run: $(BUILD_DIR)/diskc.elf $(BUILD_DIR)/rawdisk
	 qemu-system-riscv32 -machine virt -bios none -kernel $(BUILD_DIR)/diskc.elf -nographic -drive file=$(BUILD_DIR)/rawdisk,if=none,format=raw,id=x0 -device virtio-blk-device,drive=x0,bus=virtio-mmio-bus.0 -trace events=trace-events,file=trace.log

run-gdb: $(BUILD_DIR)/diskc.elf $(BUILD_DIR)/rawdisk
	 qemu-system-riscv32 -machine virt -bios none -kernel $(BUILD_DIR)/diskc.elf -nographic -drive file=$(BUILD_DIR)/rawdisk,if=none,format=raw,id=x0 -device virtio-blk-device,drive=x0,bus=virtio-mmio-bus.0 -S -gdb tcp::9999
