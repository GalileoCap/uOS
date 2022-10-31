# GalilOS

## General roadmap

- [ ] Basic
  - [X] Boot
    - [X] Long mode
  - [X] Serial output
  - [ ] Interrupts
    - [ ] Catch exceptions
  - [X] Memory
    - [X] Higher-half
    - [X] Kernel heap
    - [X] Page manager
    - [ ] Manage page faults
  - [ ] I/O
    - [ ] HDD
  - [ ] Filesystem
    - [ ] Virtual
    - [ ] Ext2
    - [ ] read/write/stat syscalls
  - [ ] Parse, load, and start ELF
  - [ ] Scheduler  
    - [ ] exit/fork/execve
  - [ ] Root process (shell)
  - [ ] Userspace
    - [ ] [libc](https://devdocs.io/c/)
      - [ ] stdio
    - [ ] libui
- [ ] Expanded
  - [ ] MBR/GPT
  - [ ] Multiprocess
    - [ ] Simultaneous
    - [ ] open/close/lseek files
  - [ ] Multithread
  - [ ] ...

## TODO

- [ ] EXT2
  * https://wiki.osdev.org/Ext2
- [ ] PIC/APIC
  * https://wiki.osdev.org/PIC#Masking
  * https://wiki.osdev.org/Programmable_Interval_Timer#I.2FO_Ports
- [ ] Drives
  * https://wiki.osdev.org/PCI
  * https://wiki.osdev.org/PCI_IDE_Controller#Read.2FWrite_From_ATA_Drive
  * https://wiki.osdev.org/ATA_PIO_Mode#Interesting_information_returned_by_IDENTIFY
  * https://wiki.osdev.org/ATA_PIO_Mode
  * https://wiki.osdev.org/ATA_read/write_sectors#Read_in_LBA_mode
- [ ] Serial Ports
  * https://wiki.osdev.org/Serial_Ports
- [ ] CPUID
  * https://wiki.osdev.org/CPUID
- [ ] Reading ELF
  * https://www.uclibc.org/docs/elf-64-gen.pdf

## Licenses

### Inspiration

* [Melvix](https://github.com/marvinborner/Melvix)
