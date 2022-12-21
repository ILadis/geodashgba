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
#define packed __attribute__((__packed__))

#define length(array) (sizeof(array) / sizeof(array[0]))

#endif
