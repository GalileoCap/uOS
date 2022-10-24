#************************************************************
#S: General

INCLUDE := ./include

CSRC := $(wildcard *.c) $(wildcard **/*.c)
ASMSRC := $(wildcard *.asm) $(wildcard **/*.asm)
OBJS := $(CSRC:.c=.o) $(ASMSRC:.asm=_asm.o)

CFLAGS += -ffreestanding -static -DKERNEL
CFLAGS += -ekmain
CFLAGS += -I./include -I$(LIBSD)/libc/include

ASMFLAGS += -i ./include

LDFLAGS += -static -nostdlib
LDFLAGS += -L$(BUILDD)/libk.a
LDFLAGS += -T./linker.ld

all: $(OBJS)
	$(LD) $(LDFLAGS) -o $(BIN) $(OBJS)
	grub-file --is-x86-multiboot $(BIN)
	objdump -S --disassemble $(BIN) > $(DUMP)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%_asm.o: %.asm
	$(ASM) $(ASMFLAGS) $< -o $@
