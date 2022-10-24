#************************************************************
#S: General

INCLUDE := ./include

CSRC := $(wildcard *.c) $(wildcard **/*.c)
ASMSRC := $(wildcard *.asm) $(wildcard **/*.asm)
OBJS := $(CSRC:.c=.o) $(ASMSRC:.asm=_asm.o)

CFLAGS += -I./include -I$(LIBS)/libc/include
ASMFLAGS += -i ./include

libc: CFLAGS += -pie -fPIE -fPIC -DUSER
libc: clean $(OBJS)
	$(AR) -rcs $(BUILDD)/libc.a $(OBJS)

libk: CFLAGS += -DKERNEL -ffreestanding -static -I$(KERNELD)/include
libk: clean $(OBJS)
	$(AR) -rcs $(BUILDD)/libk.a $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%_asm.o: %.asm
	$(ASM) $(ASMFLAGS) $< -o $@

clean:
	@find . -name "*.o" -type f -delete
