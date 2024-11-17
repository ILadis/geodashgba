#ifndef TYPES_H
#define TYPES_H

#define NULL ((void *) 0)
#define bool _Bool

#define true  1
#define false 0

typedef signed char    s8;
typedef signed short   s16;
typedef signed int     s32;

typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned int   u32;

#define align4 __attribute__((aligned(4)))
#define align8 __attribute__((aligned(8)))
#define packed __attribute__((__packed__))
#define unused __attribute__((__unused__))

#ifndef NOGBA
#define arm   __attribute__((target("arm")))
#define thumb __attribute__((target("thumb")))
// can be applied on code and data
#define ewram __attribute__((section(".ewram")))
// can be applied on code
#define iwram __attribute__((section(".iwram")))

#else
#define arm
#define thumb
#define ewram
#define iwram
#endif

#define length(array)  (sizeof(array) / sizeof(array[0]))
#define strlen(string) (length(string) - 1)
#define round(value)   (value + 0.5 - (value < 0))

#define hexof(value, index) ((const char[]){"0123456789ABCDEF"}[(value >> (index*4)) & 0xF])
#define hexto(value) ((const unsigned char[]){ \
  /* 01234567 */ 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, \
  /* 89:;<=>? */ 0x08, 0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
  /* @ABCDEFG */ 0x00, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x00, \
  /* HIJKLMNO */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
}[(value & 0x1F) ^ 0x10])

#endif
