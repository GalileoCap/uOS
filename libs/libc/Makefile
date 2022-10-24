#************************************************************
#S: General

INCLUDE := ./include

CSRC := $(wildcard *.c) $(wildcard **/*.c)
ASMSRC := $(wildcard *.asm) $(wildcard **/*.asm)
OBJS := $(CSRC:.c=.o) $(ASMSRC:.asm=_asm.o)

CFLAGS += -I./include -I$(LIBS)/libc/include
ASMFLAGS += -i ./include

libc: CFLAGS += -pie -fPIE -fPIC -DUSER
libc: $(OBJS)
	$(AR) -rcs $(BUILDD)/libc.a $+

libk: CFLAGS += -DKERNEL -ffreestanding -static -I$(KERNELD)/include
libk: $(OBJS)
	$(AR) -rcs $(BUILDD)/libk.a $+

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%_asm.o: %.asm
	$(ASM) $(ASMFLAGS) $< -o $@
