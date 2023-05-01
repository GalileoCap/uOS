import os
from pathlib import Path

#************************************************************
#* S: Utils *************************************************

def filesWithExtension(d, extension):
  return [ str(fpath) for fpath in list(Path(d).rglob(f'*{extension}')) ]

def getExtension(fpath):
  if fpath[-2:] == '.c': # C-file
    return fpath[:-2], '.c', '.o'
  elif fpath[-3:] == '.cc': # C++-file
    return fpath[:-3], '.cc', '.o'
  elif fpath[-4:] == '.ipp': # Template C++ file
    return fpath[:-4], '.ipp', '_ipp.o'
  elif fpath[-4:] == '.asm': # Asm file
    return fpath[:-4], '.asm', '_asm.o'

def getOpath(fpath, target):
  noExtension, _, newExtension = getExtension(fpath)
  middle = f'_{target}_' if target in ['libc', 'libk'] else ''
  return noExtension + middle + newExtension

def getCFlags(target):
  if target == 'kernel': return KCFLAGS
  elif target == 'libc': return LIBCCFLAGS
  elif target == 'libk': return LIBKCFLAGS
  elif target == 'stdlib': return CFLAGS
  else: return CFLAGS

def getAsmFlags(target):
  if target == 'kernel': return KASMFLAGS
  elif target in ['libc', 'libk', 'stdlib']: return LIBCASMFLAGS
  else: return ASMFLAGS

def compileCommand(fpath, target):
  noExtension, extension, newExtension = getExtension(fpath)
  opath = getOpath(fpath, target)

  cflags = getCFlags(target)
  asmflags = getAsmFlags(target)

  if extension in ['.c', '.cc']:
  	return f'{CC} {cflags} -c {fpath} -o {opath}'
  elif extension == '.asm':
    return f'{ASM} {asmflags} {fpath} -o {opath}'

def compileTask(fpath, target):
  return {
    'name': fpath,
    'deps': [TaskCompiler, fpath] + IPPSOURCES, #TODO: Also depend on header #TODO: Don't depend on all ipp's
    'outs': [getOpath(fpath, target)],

    'actions': [compileCommand(fpath, target)],
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
STDLIBD = f'{LIBSD}/stdlib'

BIN = f'{MOUNTD}/boot/galilos.bin'
ISO = f'{BUILDD}/galilos.iso'
MAP = f'{BUILDD}/galilos.map'
DUMP = f'{BUILDD}/galilos.dump'
GRUBCFG = f'{MOUNTD}/boot/grub/grub.cfg'

LIBCA = f'{BUILDD}/libc.a'
LIBKA = f'{BUILDD}/libk.a'

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
  + f' -I{LIBCD}/include'
  + f' -I{STDLIBD}/include'
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
KSOURCES = filesWithExtension(KERNELD, '.c') + filesWithExtension(KERNELD, '.cc') + filesWithExtension(KERNELD, '.asm')
KOBJS = [
  getOpath(fpath, 'kernel')
  for fpath in KSOURCES
]

LIBCCFLAGS = (
  CFLAGS
  + ' -pie -fPIE -fPIC -DUSER'
)
LIBKCFLAGS = (
  CFLAGS
  + f' -DKERNEL -I./{KERNELD}/include'
)
LIBCASMFLAGS = ASMFLAGS + f' -i {LIBCD}/include'
LIBCSOURCES = filesWithExtension(LIBCD, '.c') + filesWithExtension(LIBCD, '.asm')
LIBCOBJS = [ getOpath(fpath, 'libc') for fpath in LIBCSOURCES ]
LIBKOBJS = [ getOpath(fpath, 'libk') for fpath in LIBCSOURCES ]

STDLIBSOURCES = filesWithExtension(STDLIBD, '.c') + filesWithExtension(STDLIBD, '.asm')
IPPSOURCES = filesWithExtension(STDLIBD, '.ipp')

#************************************************************
#* S: Kernel ************************************************

def TaskKernel():
  return {
    'subtasks': [
      compileTask(fpath, 'kernel')
      for fpath in KSOURCES
    ]
  }

def TaskLinkKernel():
  return {
    'deps': KOBJS + [LIBKA, TaskCompiler, TaskGrub],
    'outs': [BIN, MAP],

    'actions': [
      f'{LD} {KLDFLAGS} -o {BIN} {" ".join(KOBJS)} {LIBKA}',
    ],
  }

def TaskDumpKernel():
  return {
    'deps': [BIN],
    'outs': [DUMP],

	  'actions': [
      f'objdump -S --disassemble {BIN} > {DUMP}'
    ],
  }

#************************************************************
#* S: Apps **************************************************

def TaskApps():
  return {
    'name': 'apps',
  }

#************************************************************
#* S: Libs **************************************************

def TaskLibc():
  return lib('libc')

def TaskLibk():
  return lib('libk')

def TaskStdlib():
  return lib('stdlib')

def libSources(lib):
  if lib in ['libc', 'libk']: return LIBCSOURCES
  elif lib == 'stdlib': return STDLIBSOURCES
  else: return []

def libObjs(lib):
  if lib == 'libc': return LIBCOBJS
  elif lib == 'libk': return LIBKOBJS
  elif lib == 'stdlib': return STDLIBSOURCES
  else: return []

def lib(lib):
  sources = libSources(lib)
  objs = libObjs(lib)

  return {
    'name': lib,

    'subtasks': [
      compileTask(fpath, lib)
      for fpath in sources
    ] + [archiveLib(lib, objs)]
  }

def archiveLib(lib, objs):
  opath = f'{BUILDD}/{lib}.a'

  return {
    'name': 'archiveLib',
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
    'clean': lambda: os.path.isfile(DISK) and input('Delete disk image? [y/N] ').lower() in ['y', 'yes'] and os.remove(DISK),

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
    'skipRun': lambda: os.path.isdir(COMPD) or input('Rebuild compiler? This can take a long time [y/N] ').lower() not in ['y', 'yes'],

    'actions': ['./compiler.sh'],
  }
