#ifndef GUARD_TYPES_H
#define GUARD_TYPES_H

/* The fixed-width types the rest of the C sources use.  agbcc is a C89
 * compiler with no <stdint.h>, so these are spelled out. */

typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned int   u32;

typedef signed char    s8;
typedef signed short   s16;
typedef signed int     s32;

#ifndef NULL
#define NULL ((void *)0)
#endif

#endif /* GUARD_TYPES_H */
