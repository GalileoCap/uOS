# uOS
An Operating System I'm making as a hobby just for fun. It'll probably only see updates every so often.  
I'm taking inspiration from what I learn of how Linux works and from OSDevWiki, but I'll try to compile my sources for every step.

## Plans
I'd like to make a customizable micro-kernel. This means a simple kernel that starts quickly without setting up much and then executes files that handle the setting up (eg. Scheduler, the 1st process, etc.)

### Current
I'm _dusting off_ the ol' GalilOS project. I'll be removing a lot of parts of the working OS to start them up again in a more organized way.  
- [X] Serial I/O.
- [ ] Remove most things to "restart" the project
- [ ] Put things back in order one by one (only what's needed for the new roadmap)

## Repo structure
- src
  - kernel
  - libs
    - libc (and libk): uOS' (partial) implementation of the C-library
  - apps: Non-essential programs (Empty for now)
- docs: Documentation
  - notes: Miscellaneous notes and bibliography
