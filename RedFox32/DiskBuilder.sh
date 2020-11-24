#!/bin/bash

[[ "$DISK_IMAGE_FILE" == "/dev/sdb" ]] && [[ $EUID -ne 0 ]] && \
	echo "Writing to storage device, please run as sudo!" && \
	sudo DISK_IMAGE_FILE=$DISK_IMAGE_FILE "$0" "$@" && \
	exit $?

[[ "$DISK_IMAGE_FILE" != "/dev/sdb" ]] && \
	echo "Creating floppy image" && \
	dd if=/dev/zero \
	of=$DISK_IMAGE_FILE \
	bs=512 \
	count=2880

[[ -f ./Build/BootloaderStage1/Bootloader.o ]] && \
	echo "Writing stage1" && \
	dd if=./Build/BootloaderStage1/Bootloader.o \
	of=$DISK_IMAGE_FILE \
	bs=512 \
	seek=0 \
	conv=notrunc

[[ -f ./Build/BootloaderStage2/Stage2.boot ]] && \
	echo "Writing stage2" && \
	dd if=./Build/BootloaderStage2/Stage2.boot \
	of=$DISK_IMAGE_FILE \
	bs=512 \
	seek=1 \
	conv=notrunc

[[ -f ./Build/Kernel/Kernel.kernel ]] && \
	echo "Writing kernel" &&
	dd if=./Build/Kernel/Kernel.kernel \
	of=$DISK_IMAGE_FILE \
	bs=512 \
	seek=18 \
	conv=notrunc

echo "Floppy created $DISK_IMAGE_FILE"
