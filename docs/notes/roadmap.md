# uOS > Docs > Notes > Roadmap
NOTE: This list is not complete. It's just meant as a simple way to decide what to do next. It will be expanded and changed frequently.

* Basics
  - [X] Compiler  
  - [X] Boot in 64-bit mode
    - [X] GDT
    - [X] Identity paging and Higher-half  
    With enough space before the MM.
  - [ ] I/O
    - [ ] Screen
    - [X] Serial
    - [ ] Log through screen and serial (exceptions, interrupts, and syscalls)
  - [ ] Basic memory manager  
    Linked list of free/used blocks of pages.  
    Recursive entries.
  - [ ] Basic ext2 driver (read/write whole file into memory at once)
  - [ ] Single-process scheduler (LIFO)
  - [ ] System calls (MM, disk, subscribe to interrupts, execute files...)
  - [ ] Cross-compiler with C stdlib (as needed with what's implemented)
  - [ ] Execute main process from the disk  
    Basic terminal that can:
    - [ ] Manipulate the filesystem (ls, mv, rm, cp)
    - [ ] Read files (cat)
    - [ ] Execute files
  - [ ] Boot without GRUB

* Automatic testing

* Multi-process-ize
  - [ ] Basic multi-process scheduler (each process gets a set time)
  - [ ] Inter-Process Communication (pipes)
  - [ ] Complete ext2 driver

* Usable
  - [ ] Permission levels
  - [ ] Install to disk
  - [ ] WM (X-like and custom on top)

* Expand
  - [ ] Multiple cores
  - [ ] Multi-threads
  - [ ] Swap memory
  - [ ] Hibernate
  - [ ] Checkpoint/Restore (CRIU)
