import os
from pathlib import Path

#************************************************************
#* S: Utils *************************************************

def foo(op0, op1, op2, op3, kernel, libc):
  if kernel and libc: return op3
  elif kernel: return op1
  elif libc: return op2
  else: return op0

def getExtension(fpath):
  if fpath[-2:] == '.c': # C-file
    return fpath[:-2], '.c', '.o'
  elif fpath[-3:] == '.cc': # C++-file
    return fpath[:-3], '.cc', '.o'
  elif fpath[-4:] == '.asm':
    return fpath[:-4], '.asm', '_asm.o'

def getOpath(fpath, *, kernel = False, libc = False):
  noExtension, _, newExtension = getExtension(fpath)
  middle = foo('', '', '_libc_', '_libk_', kernel, libc)
  return noExtension + middle + newExtension

def compileCommand(fpath, *, kernel = False, libc = False):
  noExtension, extension, newExtension = getExtension(fpath)
  opath = getOpath(fpath, kernel = kernel, libc = libc)

  cflags = foo(CFLAGS, KCFLAGS, LIBCCFLAGS, LIBKCFLAGS, kernel, libc)
  asmflags = foo(ASMFLAGS, KASMFLAGS, LIBCASMFLAGS, LIBCASMFLAGS, kernel, libc)

  if extension in ['.c', '.cc']:
  	return f'{CC} {cflags} -c {fpath} -o {opath}'
  elif extension == '.asm':
    return f'{ASM} {asmflags} {fpath} -o {opath}'

def compileTask(fpath, *, kernel = False, libc = False):
  return {
    'name': fpath,
    'deps': [TaskCompiler, fpath], #TODO: Also depend on header
    'outs': [getOpath(fpath, kernel = kernel, libc = libc)],

    'actions': [compileCommand(fpath, kernel = kernel, libc = libc)],
  }

#************************************************************
#* S: Config ************************************************

COMPD = './compiler'
BUILDD = './build'
MOUNTD = f'{BUILDD}/mnt'
os.makedirs(BUILDD, exist_ok = True)

KERNELD = './src/kernel'
LIBSD = './src/libs'
APPSD = './src/apps'
LIBCD = f'{LIBSD}/libc'

BIN = f'{MOUNTD}/boot/galilos.bin'
ISO = f'{BUILDD}/galilos.iso'
MAP = f'{BUILDD}/galilos.map'
DUMP = f'{BUILDD}/galilos.dump'
GRUBCFG = f'{MOUNTD}/boot/grub/grub.cfg'

DISK = f'{BUILDD}/hdd.img'
DISKUNIT = 1024**2
DISKSZ = 1
DISKLABEL = 'TESTDRIVE'

OFLAGS = ' -Og' #TODO: -O2

# CC = f'{COMPD}/bin/x86_64-elf-gcc'
CC = f'{COMPD}/bin/x86_64-elf-g++'
CFLAGS = (
  '-std=c++20 -m64 -mcmodel=large'
  + ' -fno-stack-protector -mno-red-zone -fno-asynchronous-unwind-tables'
  + ' -mgeneral-regs-only -ffreestanding'
  + ' -w' # + ' -Wall' #TODO: Re-enable warnings
  + ' -nostartfiles -nostdlib'
  + ' -masm=intel'
  + f' -I{LIBSD}/libc/include'
  + OFLAGS
)

ASM = 'nasm'
ASMFLAGS = ' -felf64 -w+all'

LD = f'{COMPD}/bin/x86_64-elf-ld'
LDFLAGS = (
  f' -L{COMPD}/lib/gcc/x86_64-elf/12.2.0/libgcc.a' # -lgcc
  + ' -z max-page-size=0x1000'
  + OFLAGS
)

AR = 'ar'
ARFLAGS = ' -rcs'

KCFLAGS = (
  CFLAGS
  + ' -ffreestanding -static'
  + ' -fno-rtti -fno-exceptions'
  + f' -DKERNEL -I./{KERNELD}/include'
  + ' -ekmain'
)
KASMFLAGS = ASMFLAGS + f' -i ./{KERNELD}/include'
KLDFLAGS = (
  LDFLAGS
  + ' -static -nostdlib'
  + f' -T./{KERNELD}/linker.ld -Map={MAP}'
)
KSOURCES = [
  str(fpath)
  for fpath in list(Path(KERNELD).rglob('*.c')) + list(Path(KERNELD).rglob('*.cc')) + list(Path(KERNELD).rglob('*.asm'))
]
KOBJS = [
  getOpath(fpath, kernel = True)
  for fpath in KSOURCES
]

LIBCCFLAGS = (
  CFLAGS
  + f' -I{LIBCD}/include'
  + ' -pie -fPIE -fPIC -DUSER'
)
LIBKCFLAGS = (
  CFLAGS
  + f' -I{LIBCD}/include'
  + f' -DKERNEL -I./{KERNELD}/include'
)
LIBCASMFLAGS = ASMFLAGS + f' -i {LIBCD}/include'
LIBCSOURCES = [
  str(fpath)
  for fpath in list(Path(LIBCD).rglob('*.c')) + list(Path(LIBCD).rglob('*.asm')) #TODO: Repeated code
]
LIBCOBJS = [
  getOpath(fpath, libc = True)
  for fpath in LIBCSOURCES
]
LIBKOBJS = [
  getOpath(fpath, kernel = True, libc = True)
  for fpath in LIBCSOURCES
]

#************************************************************
#* S: Kernel ************************************************

def TaskKernel():
  return {
    'subtasks': [
      compileTask(fpath, kernel = True)
      for fpath in KSOURCES
    ],
  }

def TaskLinkKernel():
  return {
    'deps': KOBJS + [f'{BUILDD}/libk.a', TaskCompiler, TaskGrub],
    'outs': [BIN, MAP],

    'actions': [
      f'{LD} {KLDFLAGS} -o {BIN} {" ".join(KOBJS)} {BUILDD}/libk.a',
    ],
  }

def TaskDumpKernel():
  return {
    'deps': [BIN],
    'outs': [DUMP],

	  'actions': [f'objdump -S --disassemble {BIN} > {DUMP}'],
  }

#************************************************************
#* S: Apps **************************************************

def TaskApps():
  return {
  }

#************************************************************
#* S: Libc **************************************************

def TaskLibc():
  return {
    'subtasks': [
      compileTask(fpath, libc = True)
      for fpath in LIBCSOURCES
    ] + [archiveLibc()]
  }

def TaskLibk():
  return {
    'subtasks': [
      compileTask(fpath, kernel = True, libc = True)
      for fpath in LIBCSOURCES
    ] + [archiveLibc(kernel = True)]
  }

def archiveLibc(*, kernel = False):
  opath = f'{BUILDD}/{"libk.a" if kernel else "libc.a"}'
  objs = LIBKOBJS if kernel else LIBCOBJS

  return {
    'name': 'archiveLibc',
    'deps': objs,
    'outs': [opath],

	  'actions': [
      f'{AR} {ARFLAGS} {opath} {" ".join(objs)}',
    ],
  }

#************************************************************
#* S: ISO ***************************************************

def TaskIso():
  return {
    'deps': [BIN, GRUBCFG],
    'outs': [ISO],

	  'actions': [
      f'grub-mkrescue -o {ISO} {MOUNTD}',
    ],
  }

def TaskGrub():
  cfg = '''set timeout=0
set default=0

menuentry "uOS" {
  multiboot /boot/galilos.bin
}'''

  grubd = f'{MOUNTD}/boot/grub'

  return {
    'outs': [GRUBCFG, grubd],

	  'actions': [
      f'mkdir -p {grubd}',
      f'printf "{cfg}" > {GRUBCFG}',
    ],
  }

#************************************************************
#* S: Disk **************************************************

def TaskPopulateDisk():
  mountd = f'{BUILDD}/mnt2'

  return {
    'deps': [DISK],
    'skipRun': lambda: (
      not os.path.isfile(DISK) or
      input('Populate disk? [y/N] ').lower() not in ['y', 'yes']
    ),

    'actions': [
      f'sudo mount --mkdir {DISK} {mountd}', # TODO: MOUNTD
      f'sudo find {mountd}/* ! -name \'lost+found\' -exec rm -rf ' + '{} +',

      f'sudo echo "Ahoy there!" | sudo tee -a {mountd}/README.md', # TODO: MOUNTD
      f'sudo mkdir {mountd}/subdir',
      f'sudo echo "My friend!!" | sudo tee -a {mountd}/subdir/test', # TODO: MOUNTD
      f'sudo umount {DISK}',
    ],
  }

def TaskDisk():
  #TODO: Populate
  return {
    'outs': [DISK],
    'skipRun': lambda: os.path.isfile(DISK),
    'clean': lambda: os.path.isfile(DISK) and input('Delete disk image? [y/N] ') in ['y', 'yes'] and os.remove(DISK),

    'actions': [
      f'dd if=/dev/zero of={DISK} bs={DISKUNIT} count={DISKSZ}',
      f'mkfs.ext2 -Onone -L{DISKLABEL} {DISK}',
    ],
  }

#************************************************************
#* S: Compiler **********************************************

def TaskCompiler():
  return {
    'deps': ['./compiler.sh'],
    'outs': [COMPD],
    'clean': False,
    'actions': ['./compiler.sh'],
  }
