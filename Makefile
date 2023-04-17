#************************************************************
#S: General

KERNELD := $(PWD)/kernel
LIBSD := $(PWD)/libs
APPSD := $(PWD)/apps
COMPD := $(PWD)/compiler
BUILDD := $(PWD)/build
MOUNTD := $(BUILDD)/mnt

BIN := $(MOUNTD)/boot/galilos.bin
ISO := $(BUILDD)/galilos.iso
MAP := $(BUILDD)/galilos.map
DUMP := $(BUILDD)/galilos.dump

DISK := $(BUILDD)/hdd.img
DISKUNIT := 1048576 #In MiB
DISKSZ := 1

OFLAGS := -Og #TODO: -O2

CC := $(COMPD)/bin/x86_64-elf-gcc
CFLAGS := -std=c17 -m64 -mcmodel=large
CFLAGS += -fno-stack-protector -mno-red-zone -fno-asynchronous-unwind-tables
CFLAGS += -mgeneral-regs-only -ffreestanding
CFLAGS += -Wall
CFLAGS += $(OFLAGS)
CFLAGS += -masm=intel
CFLAGS += -I$(LIBSD)/libc/include

ASM := nasm
ASMFLAGS := -felf64
ASMFLAGS += -w+all

LD := $(COMPD)/bin/x86_64-elf-ld
LDFLAGS := -L$(COMPD)/lib/gcc/x86_64-elf/12.2.0/libgcc.a # -lgcc
LDFLAGS += $(OFLAGS)
LDFLAGS += -z max-page-size=0x1000

QEMU := qemu-system-x86_64
QEMUFLAGS := -accel tcg,thread=single -cpu core2duo
QEMUFLAGS += -serial stdio
QEMUFLAGS += -m 128
QEMUFLAGS += -cdrom $(ISO)
QEMUFLAGS += -hda $(DISK)
QEMUFLAGS += -smp 1 -usb -vga std -no-reboot

export

#************************************************************
#S: Targets

default: help
.PHONY: run iso kernel apps libc libk disk buildd grub help clean

run: compiler disk apps iso
	$(QEMU) $(QEMUFLAGS)

iso: kernel
	grub-mkrescue -o $(ISO) $(MOUNTD)
	@printf "[ISO] Done\n"

kernel: libk grub
	@$(MAKE) --no-print-directory -C $(KERNELD)
	@printf "[KERNEL] Done\n"

apps: libc buildd
	@$(MAKE) --no-print-directory -C $(APPSD)
	@printf "[APPS] Done\n"

libc: buildd
	@$(MAKE) libc --no-print-directory -C $(LIBSD)/libc
	@printf "[LIBC] Done\n"

libk: buildd
	@$(MAKE) libk --no-print-directory -C $(LIBSD)/libc
	@printf "[LIBK] Done\n"

disk: buildd
	rm -f $(DISK) $(DISK).lock
	dd if=/dev/zero of=$(DISK) bs=$(DISKUNIT) count=$(DISKSZ)
	mkfs.ext2 -Onone -LGALIDRIVE $(DISK)
	@printf "[DISK] Done\n"
	#TODO: Populate disk

compiler:
	@printf "[COMPILER] Building the compiler may take several minutes, are you sure? [y/N] " && read ans && [ $${ans:-N} = y ]
	@./compiler.sh
	@printf "[COMPILER] Done\n"

grub: buildd
	mkdir -p $(MOUNTD)/boot/grub
	printf "set timeout=0 \n\
	set default=0 \n\
	\n\
	menuentry \"GalilOS\" { \n\
		multiboot /boot/galilos.bin \n\
	}" > $(MOUNTD)/boot/grub/grub.cfg

buildd:
	mkdir -p $(BUILDD) $(MOUNTD)

help:
	@printf "TODO: Help\n"

clean:
	@printf "[CLEAN] Are you sure? [y/N] " && read ans && [ $${ans:-N} = y ]
	rm -fr $(BUILDD)
	find $(KERNELD) $(APPD) $(LIBSD) \( -name "*.o" -or -name "*.a" -or -name "*.elf" -or -name "*.bin" -or -name "*.log" \) -type f -delete
	@printf "\n[CLEAN] Cleaning\n"
