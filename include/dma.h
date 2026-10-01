#ifndef _DMA_H_
#define _DMA_H_

#include "gba/io.h"
#include "gba/types.h"

// wrapper inline assembly for starting a DMA3 transfer.
// I'm not sure if this is the whole story, but it matches the
// common pattern of
// fill r0-r3 with dma-related values
// stmia r3!, {r0,r1,r2}
// subs r3, #0xc

static inline void DMA3_COPY(const void *src, void *dst, u32 size) {
    register vu32 *_base __asm__("r3") = &REG_DMA3SAD;
    register const void *_src  __asm__("r0") = src;
    register void *_dst  __asm__("r1") = dst;
    register u32 _cnt  __asm__("r2") = 0x84000000 | (size / 4);
    __asm__ volatile (
        "stmia\tr3!, {r0, r1, r2}\n\t"
        "sub\tr3, #0xc"
        :
        : "r" (_base), "r" (_src), "r" (_dst), "r" (_cnt)
        : "memory"
    );
}

static inline void DMA3_SET(const void *src, void *dst, u32 cnt) {
    register vu32 *_base __asm__("r3") = &REG_DMA3SAD;
    register const void *_src  __asm__("r0") = src;
    register void *_dst  __asm__("r1") = dst;
    register u32 _cnt  __asm__("r2") = cnt;
    __asm__ volatile (
        "stmia\tr3!, {r0, r1, r2}\n\t"
        "sub\tr3, #0xc"
        :
        : "r" (_base), "r" (_src), "r" (_dst), "r" (_cnt)
        : "memory", "r0"
    );
}

// there must be a way to unify those, maybe they were macros instead of
// inline functions and had some sort of common DMAN_SET

static inline void DMA3_CLEAR(void *dst, unsigned size) {
    u32 value;
    register u32 * _src  __asm__("r0") = (&value);
    *_src = 0;
    {
        register vu32 *_base __asm__("r3") = &REG_DMA3SAD;
        register unsigned _dst  __asm__("r1") = (unsigned)(dst);
        register unsigned _cnt  __asm__("r2") = (unsigned)(0x85000000 | (size / 4));
        __asm__ volatile (
            "stmia\t%0!, {%1, %2, %3}\n\t"
            "sub\t%0, #0xc"
            :
            : "l" (_base), "l" (_src), "l" (_dst), "l" (_cnt)
            : "memory"
        );
    }
}

// there must be a way to unify those, maybe they were macros instead of
// inline functions and had some sort of common DMAN_SET

static inline void DMA3_FILL(void *dst, u32 _value, unsigned size) {
    u32 value;
    register u32 * _src  __asm__("r0") = (&value);
    *_src = _value;
    {
        register vu32 *_base __asm__("r3") = &REG_DMA3SAD;
        register unsigned _dst  __asm__("r1") = (unsigned)(dst);
        register unsigned _cnt  __asm__("r2") = (unsigned)(0x85000000 | (size / 4));
        __asm__ volatile (
            "stmia\t%0!, {%1, %2, %3}\n\t"
            "sub\t%0, #0xc"
            :
            : "l" (_base), "l" (_src), "l" (_dst), "l" (_cnt)
            : "memory"
        );
    }
}

/* DMA3_FILL_OFS -- DMA3_FILL with the destination split into a BASE and an
 * OFFSET.  Passing the offset as its own argument is what makes integrate.c's
 * pre-copy cheap: it becomes `(set rN 64)`, the ROM's early `mov r1,#0x40`,
 * while the `add` of the base moves into the body after `mov r0,sp`.  Needed to
 * land Func_8005920 (SomethingSaveHeader).
 *
 * NAMED _OFS, NOT _AT, DELIBERATELY: src/non_matching/rom_b5000/80c02a4.c
 * already defines a file-local `DMA3_FILL_AT` with a DIFFERENT signature
 * (slot, value, dst, size).  Promoting this one as DMA3_FILL_AT would give the
 * tree two meanings for one name -- harmless to the 82 landed DMA3 users today,
 * but exactly the debt that makes a later rename pass ambiguous.  If that park
 * ever lands, the two should be reconciled under one name then, with both
 * signatures in view.
 */
static inline void DMA3_FILL_OFS(void *dst, unsigned off, u32 _value, unsigned size) {
    u32 value;
    register u32 * _src  __asm__("r0") = (&value);
    *_src = _value;
    {
        register vu32 *_base __asm__("r3") = &REG_DMA3SAD;
        register unsigned _dst  __asm__("r1") = (unsigned)(dst) + off;
        register unsigned _cnt  __asm__("r2") = (unsigned)(0x85000000 | (size / 4));
        __asm__ volatile (
            "stmia\t%0!, {%1, %2, %3}\n\t"
            "sub\t%0, #0xc"
            :
            : "l" (_base), "l" (_src), "l" (_dst), "l" (_cnt)
            : "memory"
        );
    }
}

static inline void DMA3_COPY16(const void *src, void *dst, u32 size) {
    register vu32 *_base __asm__("r3") = &REG_DMA3SAD;
    register const void *_src  __asm__("r0") = src;
    register void *_dst  __asm__("r1") = dst;
    register u32 _cnt  __asm__("r2") = 0x80000000 | (size / 4);
    __asm__ volatile (
        "stmia\tr3!, {r0, r1, r2}\n\t"
        "sub\tr3, #0xc"
        :
        : "r" (_base), "r" (_src), "r" (_dst), "r" (_cnt)
        : "memory"
    );
}

// The DMA0 form: same shape as DMA3_SET, but its base register is the one
// UnknownDMAPrefix() has already loaded, so a function that does the prefix and
// then a DMA0 transfer loads &REG_DMA0SAD exactly once.
static inline void DMA0_SET(const void *src, void *dst, u32 cnt) {
    register vu32 *_base __asm__("r3") = &REG_DMA0SAD;
    register const void *_src  __asm__("r0") = src;
    register void *_dst  __asm__("r1") = dst;
    register u32 _cnt  __asm__("r2") = cnt;
    __asm__ volatile (
        "stmia\tr3!, {r0, r1, r2}\n\t"
        "sub\tr3, #0xc"
        :
        : "r" (_base), "r" (_src), "r" (_dst), "r" (_cnt)
        : "memory"
    );
}

// prelude on some functions

static inline u16 UnknownDMAPrefix(void) {
    vu16 *dma = (vu16*)&REG_DMA0SAD;
    u16 cnt = dma[5];
    dma[5] = cnt & 0xc5ff;
    cnt = dma[5];
    dma[5] = cnt & 0x7fff;
    return dma[5];
}

/* DMA3_COPY16 without the promise that the transfer preserves src and count.
 *
 * PROMOTED to this header in batch 299, on the standing instruction left where it was
 * first written (src/rom_a1000/rom_a1814_c_a_a_c_a_c_a_c_c_b.c): "Promote it if a
 * second function needs it."  Func_80a7478 is that second function -- without this
 * form, reload_cse_move2add derives the later transfers from the preserved operands.
 *
 * The "+l" outputs are what withdraw the promise, and they are LOAD-BEARING BOTH:
 * measured at object level, "+l" on the count alone or on the source alone each
 * leaves the other constant strength-reduced.  The legal "+l" form and an illegal
 * form clobbering "r0","r2" score IDENTICALLY, so there is no reason to prefer the
 * illegal one.
 *
 * DO NOT retrofit DMA3_COPY16 itself -- the rest of the tree depends on it keeping
 * its current promise, and 29 files use the existing helpers.
 */
static inline void DMA3_COPY16_RW(void *src, void *dst, u32 size)
{
    register vu32 *_base __asm__("r3") = &REG_DMA3SAD;
    register void *_src __asm__("r0") = src;
    register void *_dst __asm__("r1") = dst;
    register u32 _cnt __asm__("r2") = 0x80000000 | (size / 4);
    __asm__ volatile (
        "stmia\tr3!, {r0, r1, r2}\n\t"
        "sub\tr3, #0xc"
        : "+l" (_src), "+l" (_cnt)
        : "l" (_base), "l" (_dst)
        : "memory"
    );
}

#endif // _DMA_H_
