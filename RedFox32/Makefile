.PHONY: build debug run Filesystem

# Source directories
VPATH:=./Source\
	:./Source/BootloaderStage1\
	:./Source/BootloaderStage2\
	:./Source/Kernel\
	:./Source/Drivers\
	:./Source/Common

# Compilers and compiler arguments
AS:=nasm
CC:=gcc
LD:=ld
CCInclude:=-I./Source
CCFLAGS:=-m32 -fno-pic -nostdlib -nolibc -ffreestanding -c -Wall 

# The final product
DISK_IMAGE_FILE:=./Build/Floppy.img

# Directories required to build successfully
BUILD_DIRECTORIES:=./Build \
	./Build/BootloaderStage1 \
	./Build/BootloaderStage2 \
	./Build/Kernel \
	./Build/Drivers \
	./Build/Common

build: build_bootloader build_kernel Filesystem

floppy: build floppy_image

usb: floppy
	@echo "Caution! Writing to /dev/sdb -- CTRL+C if you do not want this to happen!"
	@read
	sudo dd if=$(DISK_IMAGE_FILE) of=/dev/sdb status=progress bs=512

debug: floppy
	qemu-system-i386 -fda $(DISK_IMAGE_FILE) -s -S --monitor stdio 

run: floppy 
	qemu-system-i386 -fda $(DISK_IMAGE_FILE)

clean:
	@echo "Cleaning"
	@rm -rf *.o *.bin *.dmp
	@rm -rf ./Build
	@mkdir $(BUILD_DIRECTORIES) 


# Generic rule for C files
./Build/BootloaderStage2/%.o: %.c
	@echo "Building $@"
	$(CC) $(CCFLAGS) $(CCInclude) -o $@ $<

# Generic rule for ASM files
./Build/BootloaderStage2/%.o ./Build/Kernel/%.o: %.asm
	@echo "Building $@"
	$(AS) -felf32 $< -o $@

# Bootloader
build_bootloader: build_bootloader_stage1 build_bootloader_stage2

# Bootloader stage 1
# Give the first stage it's own compilation rule
# Stage1 is required to be a binary file unlike (most) other asm files which can
# be compiled to elf files
build_bootloader_stage1: ./Source/BootloaderStage1/Bootloader.asm
	@echo "Building BootloaderStage1"
	$(AS) -fbin ./$< -o ./Build/BootloaderStage1/Bootloader.o

# Bootloader stage 2
BOOTLOADER_STAGE2_FILES:=$(addsuffix .o,\
	$(basename $(patsubst ./Source%, ./Build%,\
	$(wildcard ./Source/BootloaderStage2/*.asm\
	$(wildcard ./Source/BootloaderStage2/*.c)\
))))
build_bootloader_stage2: $(BOOTLOADER_STAGE2_FILES)
	@echo "Building BootloaderStage2"
	$(LD) \
		-TLinkerScripts/Stage2Linker.ld \
		-o ./Build/BootloaderStage2/Stage2.boot \
		$(BOOTLOADER_STAGE2_FILES)


# Kernel
./Build/Common/%.o : %.c
./Build/Kernel/%.o : %.c
	@echo "Building $@"
	$(CC) $(CCFLAGS) $(CCInclude) -o $@ $<

KERNEL_FILES:=$(addsuffix .o,\
	$(basename $(patsubst ./Source%, ./Build%,\
	$(wildcard ./Source/Kernel/*.c) $(wildcard ./Source/Kernel/*.asm\
))))

build_kernel: $(KERNEL_FILES)
	@echo "Linking kernel"
	$(LD) -TLinkerScripts/Kernel.ld -o ./Build/Kernel/Kernel.kernel $(KERNEL_FILES)

Filesystem:
	nasm -fbin Source/Filesystem/FSRoot.asm -o Build/FSRootDir.bin
	nasm -fbin Source/Filesystem/SectorMap.asm -o Build/FSSectorMap.bin

# Generate a floppy image
floppy_image: build
	DISK_IMAGE_FILE=$(DISK_IMAGE_FILE) ./DiskBuilder.sh

floppy_image_no_build: 
	DISK_IMAGE_FILE=$(DISK_IMAGE_FILE) ./DiskBuilder.sh
