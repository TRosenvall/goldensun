/* Func_801edec  @ 0x0801edec  [rom_15000]
 *
 * Source asm: goldensun/asm/rom_15000/rom_1de5c_c_c_c_c_a_a_a_a.s
 *
 * BLOCKER: TWO instructions, an ordering swap inside the DMA staging sequence.
 * 45 lines against 45, 3 differing, and one of those three is not a real
 * difference at all:
 *
 *     rom    mov r0, sp / ldr r3, =0xe0e0
 *     ours   ldr r3, =0xe0e0 / mov r0, sp
 *
 *     rom    ldr r5, =0x214
 *     ours   ldr r5, =_FUNC_80158E8_SIZE
 *
 * The second is a POOL SYMBOL versus its value. `_FUNC_80158E8_SIZE` is emitted
 * by `.func_end_emit_size Func_80158e8, _FUNC_80158E8_SIZE` in
 * asm/rom_15000/rom_15430.s as an absolute `End - Start` = 0x214, so it
 * resolves to the identical word at link time. tryc compares disassembly text
 * and cannot see that. Do not spend attempts on it.
 *
 * THE DMA SHAPE IS NOT IN include/dma.h, AND THAT IS THE USEFUL FINDING.
 * The ROM stages a HALFWORD (`strh` at sp+2) and passes 0x810000a0 -- a 16-bit
 * fill. dma.h has DMA3_SET, DMA3_CLEAR and DMA3_FILL, and all three stage a
 * WORD with 0x85000000. The file's own comment already says "there must be a
 * way to unify those". A DMA3_FILL16 written in that file's exact style --
 * `u16 value`, a register-pinned `u16 *` in r0, control word 0x81000000 --
 * gets this function to the 3 lines above on the first screen.
 *
 * It is NOT added to dma.h, deliberately: an unused static inline in a shared
 * header, added on the strength of a function that does not match, is noise
 * that the next person has to evaluate. The helper is reproduced below so it
 * costs nothing to pick up when someone closes the last two lines.
 *
 * MEASURED, on the helper's internals:
 *   `*_src = _value;` after the pinned pointer (dma.h's own pattern)      3
 *   `value = _value;` storing through the object instead                 35  (46 lines)
 *   hoisting the `dst` register binding above the store                   3  (no change)
 *
 * WHAT IS LEFT is that gcc materialises the pooled 0xe0e0 before `mov r0, sp`
 * and the ROM does the reverse. Both orders are inside the inline expansion,
 * so the call site cannot reach it; it wants a spelling of the helper that
 * makes the pointer live first, and the two tried above do not.
 *
 * The rest is settled and should not be re-derived: the else arm is
 * `f = Func_8004938(size); DMA3_SET(Func_80158e8, f, 0x84000000 | (size / 4));
 * f(dst, v); free(f);` with the size read from the linker symbol, and
 * `bl _call_via_r6` is an ordinary function-pointer local.
 */

/* The helper this function wants, in include/dma.h's own style:
 *
 * static inline void DMA3_FILL16(void *dst, u16 _value, unsigned count) {
 *     u16 value;
 *     register u16 * _src  __asm__("r0") = (&value);
 *     *_src = _value;
 *     {
 *         register vu32 *_base __asm__("r3") = &REG_DMA3SAD;
 *         register unsigned _dst  __asm__("r1") = (unsigned)(dst);
 *         register unsigned _cnt  __asm__("r2") = (unsigned)(0x81000000 | count);
 *         __asm__ volatile (
 *             "stmia\t%0!, {%1, %2, %3}\n\t"
 *             "sub\t%0, #0xc"
 *             :
 *             : "l" (_base), "l" (_src), "l" (_dst), "l" (_cnt)
 *             : "memory"
 *         );
 *     }
 * }
 */
#include "gba/types.h"
#include "dma.h"

extern unsigned int iwram_3001e8c;
extern void *Func_8004938(unsigned int size);
extern void Func_80158e8(void *a, unsigned int b);
extern void free(void *p);
extern char _FUNC_80158E8_SIZE[];

void Func_801edec(void *dst)
{
    unsigned int v;
    void (*f)(void *, unsigned int);
    unsigned int size;

    v = iwram_3001e8c;
    if (v == 0) {
        DMA3_FILL16(dst, 0xe0e0, 0xa0);
    } else {
        size = (unsigned int)_FUNC_80158E8_SIZE;
        f = Func_8004938(size);
        DMA3_SET(Func_80158e8, f, 0x84000000 | (size / 4));
        f(dst, v);
        free(f);
    }
}
