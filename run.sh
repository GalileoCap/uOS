#!/bin/bash

udo

qemu-system-x86_64 \
  -accel tcg,thread=single -cpu core2duo \
  -serial stdio \
  -m 128 \
  -cdrom build/galilos.iso \
  -hda build/hdd.img \
  -smp 1 -usb -vga std -no-reboot
