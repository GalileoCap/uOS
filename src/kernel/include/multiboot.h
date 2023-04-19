#ifndef __MULTIBOOT_H__
#define __MULTIBOOT_H__

#include <defines.h>

//READ: https://www.gnu.org/software/grub/manual/multiboot/html_node/multiboot_002eh.html

/* The symbol table for a.out. */
struct multiboot_aout_symbol_table {
  u32 tabsize;
  u32 strsize;
  u32 addr;
  u32 reserved;
};

struct multiboot_elf_section_header_table {
  u32 num;
  u32 size;
  u32 addr;
  u32 shndx;
};

struct multiboot_color {
  u8 red;
  u8 green;
  u8 blue;
};

struct multiboot_mmap_entry {
  u32 size;
  u64 addr;
  u64 len;
#define MULTIBOOT_MEMORY_AVAILABLE              1
#define MULTIBOOT_MEMORY_RESERVED               2
#define MULTIBOOT_MEMORY_ACPI_RECLAIMABLE       3
#define MULTIBOOT_MEMORY_NVS                    4
#define MULTIBOOT_MEMORY_BADRAM                 5
  u32 type;
} __attribute__((packed));

struct multiboot_mod_list {
  /* the memory used goes from bytes ’mod_start’ to ’mod_end-1’ inclusive */
  u32 mod_start;
  u32 mod_end;

  /* Module command line */
  u32 cmdline;

  /* padding to take it to 16 bytes (must be zero) */
  u32 pad;
};

/* APM BIOS info. */
struct multiboot_apm_info {
  u16 version;
  u16 cseg;
  u32 offset;
  u16 cseg_16;
  u16 dseg;
  u16 flags;
  u16 cseg_len;
  u16 cseg_16_len;
  u16 dseg_len;
};

struct multiboot_info {
  /* Multiboot info version number */
  u32 flags;

  /* Available memory from BIOS */
  u32 mem_lower;
  u32 mem_upper;

  /* "root" partition */
  u32 boot_device;

  /* Kernel command line */
  u32 cmdline;

  /* Boot-Module list */
  u32 mods_count;
  u32 mods_addr;

  union {
    struct multiboot_aout_symbol_table aout_sym;
    struct multiboot_elf_section_header_table elf_sec;
  } u;

  /* Memory Mapping buffer */
  u32 mmap_length;
  u32 mmap_addr;

  /* Drive Info buffer */
  u32 drives_length;
  u32 drives_addr;

  /* ROM configuration table */
  u32 config_table;

  /* Boot Loader Name */
  u32 boot_loader_name;

  /* APM table */
  u32 apm_table;

  /* Video */
  u32 vbe_control_info;
  u32 vbe_mode_info;
  u16 vbe_mode;
  u16 vbe_interface_seg;
  u16 vbe_interface_off;
  u16 vbe_interface_len;

  u64 framebuffer_addr;
  u32 framebuffer_pitch;
  u32 framebuffer_width;
  u32 framebuffer_height;
  u8 framebuffer_bpp;
#define MULTIBOOT_FRAMEBUFFER_TYPE_INDEXED  0
#define MULTIBOOT_FRAMEBUFFER_TYPE_RGB      1
#define MULTIBOOT_FRAMEBUFFER_TYPE_EGA_TEXT 2
  u8 framebuffer_type;
  union {
    struct {
      u32 framebuffer_palette_addr;
      u16 framebuffer_palette_num_colors;
    };
    struct {
      u8 framebuffer_red_field_position;
      u8 framebuffer_red_mask_size;
      u8 framebuffer_green_field_position;
      u8 framebuffer_green_mask_size;
      u8 framebuffer_blue_field_position;
      u8 framebuffer_blue_mask_size;
    };
  };
};

#endif // __MULTIBOOT_H__
