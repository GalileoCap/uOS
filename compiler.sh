#!/bin/sh

set -e
BINUSRC=binutils-2.39
GCCSRC=gcc-12.2.0

rm -rf compiler/{bin,build,i686-elf,lib,share,include,libexec,x86_64-elf}
mkdir -p compiler
cd compiler
PREFIX=$(pwd)

#TODO: Check if files already exist
[ ! -d $BINUSRC ] && wget -O- https://ftp.gnu.org/gnu/binutils/$BINUSRC.tar.gz | tar -xz
[ ! -d $GCCSRC ] && wget -O- https://ftp.gnu.org/gnu/gcc/$GCCSRC/$GCCSRC.tar.gz | tar -xz && echo -e "MULTILIB_OPTIONS += mno-red-zone\nMULTILIB_DIRNAMES += no-red-zone" > $GCCSRC/gcc/config/i386/t-x86_64-elf && sed -i "/^x86_64-\*-elf\*).*/a tmake_file=\"\${tmake_file} i386\/t-x86_64-elf\"" $GCCSRC/gcc/config.gcc

build_target() {
  mkdir build
  cd build

  ../$BINUSRC/configure --target=$TARGET --prefix="$PREFIX" --with-sysroot --disable-nls --disable-werror
  make
  make install

  cd ..
  rm -rf build
  mkdir build
  cd build

  ../$GCCSRC/configure --target=$TARGET --prefix="$PREFIX" --disable-nls --enable-languages=c,c++ --without-headers
  make all-gcc
  make all-target-libgcc
  make install-gcc
  make install-target-libgcc

  cd ..
  rm -rf build
}

#TARGET=i686-elf
#build_target

TARGET=x86_64-elf
build_target
