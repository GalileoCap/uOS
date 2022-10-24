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

OFLAGS := -Og #TODO: -O2

CC := $(COMPD)/bin/x86_64-elf-gcc
CFLAGS := -std=c17 -m64 -mcmodel=large
CFLAGS += -fno-stack-protector -mno-red-zone -fno-asynchronous-unwind-tables
CFLAGS += -mgeneral-regs-only 
CFLAGS += -Wall
CFLAGS += $(OFLAGS)
CFLAGS += -masm=intel

ASM := nasm
ASMFLAGS := -felf64
ASMFLAGS += -w+all

LD := $(COMPD)/bin/x86_64-elf-ld
LDFLAGS := -L$(COMPD)/lib/gcc/x86_64-elf/12.2.0/libgcc.a # -lgcc
LDFLAGS += $(OFLAGS)
LDFLAGS += -z max-page-size=0x1000

#******************************
#S: Disk image

DISK := $(BUILDD)/hdd.img
DISKSZ := 1 #A: In MiB

export

#************************************************************
#S: Targets

default: help
.PHONY: bochs libc libk kernel apps iso help clean

bochs: all
	#TODO: Check disk exists
	cd $(BUILDD) && bochs -q

all: compiler libc libk kernel apps iso

compiler:
	@printf "[COMPILER] Building the compiler may take several minutes, are you sure? [y/N] " && read ans && [ $${ans:-N} = y ]
	@./compiler.sh
	@printf "[COMPILER] Done\n"

libc:
	@$(MAKE) libc --no-print-directory -C $(LIBSD)/libc
	@printf "[LIBC] Done\n"

libk:
	@$(MAKE) libk --no-print-directory -C $(LIBSD)/libc
	@printf "[LIBK] Done\n"

iso:
	grub-mkrescue -o $(ISO) $(MOUNTD)
	@printf "[ISO] Done\n"

disk:
	dd if=/dev/zero of=$(DISK) bs=1048576 count=$(DISKSZ)
	mkfs.ext2 -Onone -LGALIDRIVE $(DISK)
	@printf "[DISK] Done\n"
	#TODO: Populate disk

kernel:
	@$(MAKE) --no-print-directory -C $(KERNELD)
	@printf "[KERNEL] Done\n"

apps:
	@$(MAKE) --no-print-directory -C $(APPSD)
	@printf "[APPS] Done\n"

help:
	@printf "TODO: Help\n"

clean:
	@printf "[CLEAN] Are you sure? [y/N] " && read ans && [ $${ans:-N} = y ]
	rm -f $(DISK) $(DISK).lock
	rm -f $(ISO) $(MAP) $(DUMP)
	find $(BUILDD) $(KERNELD) $(APPD) $(LIBSD) \( -name "*.o" -or -name "*.a" -or -name "*.elf" -or -name "*.bin" \) -type f -delete 
	@printf "\n[CLEAN] Cleaning\n"
