/* Func_801edec  --  0x0801edec, asm/rom_15000/rom_1de5c_c_c_c_c_a_a_a_a.s
 *
 * BLOCKER CLASS: literal pool layout -- and this one is a WARNING ABOUT THE
 * SCREEN as much as about the function.
 *
 * tools/tryc.py reports 45 lines against 45 with ONE difference, and that
 * difference is only the screen printing `=_FUNC_80158E8_SIZE` where the
 * reference prints `=0x214` -- the same instruction, the same value. By every
 * measure the screen can take, this function matches.
 *
 * IT FAILS `make compare` BY 323,730 BYTES.
 *
 * The ROM keeps a literal pool INSIDE the function, between the two arms:
 *
 *      sub r3, #0xc
 *      b   .L1ee4e
 *      .align 2, 0
 *      .L1ee14: .word 0xe0e0
 *      .pool
 *      .L1ee24: ldr r5, =0x214 ...
 *
 * gcc lays its pool out differently, the translation unit comes out a different
 * SIZE, and everything after it in the ROM shifts. The first differing bytes
 * are not even in this function -- they are at 0x0801512c, in the rom_15000
 * exports table, whose veneers carry pooled addresses of functions that moved.
 *
 * AND tryc DID NOT WARN. It has an inline-pool check that fires on
 * `.pool_aligned` and mid-body `.word` -- it fired on Func_8017a64 and
 * Func_80c90e4 this same batch -- and it did not fire here. So "no warning" is
 * not evidence of no pool problem. The only test that decides pool layout is
 * `make compare`.
 *
 * EVERYTHING ELSE IS SETTLED, and each piece is a rule that is already written
 * down:
 *
 *   THE SIZE IS A SYMBOL. `ldr r5, =0x214` where gcc builds that constant with
 *   `mov`/`lsl` is the pool tell. `_FUNC_80158E8_SIZE` is emitted by
 *   `.func_end_emit_size` in asm/rom_15000/rom_15430.s and IS in the build --
 *   Func_80158e8 runs 0x080158e8..0x08015afc, 0x214 exactly.
 *
 *   THE FILL VALUE NEEDS AN int LOCAL. Written straight into a `u16`, gcc pools
 *   0xe0e0 as a HALFWORD; the ROM loads a word.
 *
 *   THE `do { } while (0)` WRAPPERS ARE LOAD-BEARING -- flat, the argument save
 *   and the size load come out in the other order, four lines. Same shape as
 *   src/rom_c0/rom_52f4.c and the two wrappers elevated this batch.
 *
 * So this is one blocker away, and the blocker is the same one that parks
 * Func_80c0e38 and ovl_7ec19c/200816c. What would fix it is unknown; nothing
 * tried in either of those moved gcc's pool placement.
 */

#include "dma.h"

extern unsigned int iwram_3001e8c;
extern void *Func_8004938(unsigned int size);
extern void Func_80158e8(void *a, unsigned int b);
extern void free(void *p);
extern char _FUNC_80158E8_SIZE[];
#define FUNC_80158E8_SIZE ((u32) _FUNC_80158E8_SIZE)

void Func_801edec(void *dst)
{
    unsigned int v;

    v = iwram_3001e8c;
    if (v == 0) {
        u16 value;
        int c;
        register u16 *_src __asm__("r0") = (&value);
        c = 0xe0e0;
        *_src = c;
        {
            register vu32 *_base __asm__("r3") = &REG_DMA3SAD;
            register unsigned _dst __asm__("r1") = (unsigned)(dst);
            register unsigned _cnt __asm__("r2") = 0x810000a0;
            __asm__ volatile (
                "stmia\t%0!, {%1, %2, %3}\n\t"
                "sub\t%0, #0xc"
                :
                : "l" (_base), "l" (_src), "l" (_dst), "l" (_cnt)
                : "memory"
            );
        }
    } else {
        do {
            void (*func)(void *, unsigned int) = Func_8004938(FUNC_80158E8_SIZE);
            DMA3_SET(Func_80158e8, func, 0x84000000 | (FUNC_80158E8_SIZE / 4));
            do {
                func(dst, v);
            } while (0);
            free(func);
        } while (0);
    }
}
