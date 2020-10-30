#!/bin/bash

echo "Creating floppy image"
dd if=/dev/zero \
	of=$DISK_IMAGE_FILE \
	bs=512 \
	count=2880

echo "Writing stage1"
dd if=./Build/BootloaderStage1/Bootloader.o \
	of=$DISK_IMAGE_FILE \
	bs=512 \
	seek=0 \
	conv=notrunc

echo "Writing stage2"
dd if=./Build/BootloaderStage2/Stage2.boot \
	of=$DISK_IMAGE_FILE \
	bs=512 \
	seek=3 \
	conv=notrunc

dd if=./Build/Kernel/Kernel.kernel \
	of=$DISK_IMAGE_FILE \
	bs=512 \
	seek=18 \
	conv=notrunc

dd if=./Build/FSRootDir.bin \
	of=$DISK_IMAGE_FILE \
	bs=512 \
	seek=1 \
	conv=notrunc

dd if=./Build/FSSectorMap.bin \
		of=$DISK_IMAGE_FILE \
		bs=512 \
		seek=2 \
		conv=notrunc

echo "Floppy created $DISK_IMAGE_FILE"
