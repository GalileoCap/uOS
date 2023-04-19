#ifndef __DEFINES_H__
#define __DEFINES_H__

/*
 * S: Types
 */

#include <stddef.h>
#include <stdbool.h>

typedef signed char s8;
typedef unsigned char u8;

typedef signed short s16;
typedef unsigned short u16;

typedef signed int s32;
typedef unsigned int u32;

typedef signed long long s64;
typedef unsigned long long u64;

/*
typedef float f32;
typedef double f64;
typedef long double f80;
*/

typedef u64 paddr_t;
typedef u64 vaddr_t;

/*
 * S: Macros
 */

#define ALWAYS_INLINE inline __attribute((always_inline))

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define CEIL(a, b) (1 + ((a - 1) / b))

#define UPPER(a) ((a) >= 'A' && ((a) <= 'Z'))
#define LOWER(a) ((a) >= 'a' && ((a) <= 'z'))
#define ALPHA(a) (UPPER(a) || LOWER(a))
#define NUMERIC(a) ((a) >= '0' && ((a) <= '9'))
#define ALPHANUMERIC(a) (ALPHA(a) || NUMERIC(a))

#define ALIGN_UP(addr, align) (((addr) + (align)-1) & ~((align)-1))
#define ALIGN_DOWN(addr, align) ((addr) & ~((align)-1))

#define FLAGS_SET(x, y) (((x) & (y)) == (y))

#define U8_MAX ((u8)255)
#define S8_MAX ((s8)127)
#define S8_MIN ((s8)-128)
#define U16_MAX ((u16)65535)
#define S16_MAX ((s16)32767)
#define S16_MIN ((s16)-32768)
#define U32_MAX ((u32)4294967295)
#define S32_MAX ((s32)2147483647)
#define S32_MIN ((s32)-2147483648)
#define U64_MAX ((u64)18446744073709551615)
#define S64_MAX ((s64)9223372036854775807)
#define S64_MIN ((s64)-9223372036854775808)
//#define F64_MAX ((f64)0x7fefffffffffffff)
//#define F64_MIN ((f64)0x10000000000000)

#define PAGE_SIZE 0x1000

#ifdef KERNEL
/* S: Multiboot  *******************************************/
#define MULTIBOOT_MAGIC 0x2BADB002

/* S: GDT **************************************************/
//U: Selectors
#define GDTCS_R0 0x0008
#define GDTDS_R0 0x0010
#define GDTCS_R3 0x0018
#define GDTDS_R3 0x0020
#define GDTTSS_S 0x0028

/* S: Memory ***********************************************/
#define USTACK_ENTRY  254
#define KSSTART_ENTRY 500
#define KSTACK_ENTRY  509
#define TMP_ENTRY     510
#define RECURSE_ENTRY 511
#define TOTAL_ENTRIES 512

#endif // KERNEL

#endif // __DEFINES_H__
