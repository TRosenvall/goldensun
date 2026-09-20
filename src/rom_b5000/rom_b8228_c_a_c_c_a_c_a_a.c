/* Cluster Func_80b8f58..Func_80b8f58 extracted from goldensun/asm/rom_b5000/rom_b8228_c_a_c_c_a_c_a.s.
 *
 * Total .text for this TU = 124 bytes (= 0x7c).
 * Preserves the original ROM layout when slotted between
 * asm/rom_b5000/rom_b8228_c_a_c_c_a_b.o and asm/rom_b5000/rom_b8228_c_a_c_c_a_c_a_b.o in
 * goldensun/stage1.ld.
 *
 * Never attempted before batch 272. No pins, no flags.
 *
 * A 16-entry palette brighten at 0x50001c0: read the backup copy at p[0x10], add
 * (1.0 - cos(iwram_3001e40 * 3 << 10)) / 0x2aaa to each 5-bit channel, clamp at 31.
 *
 * THREE THINGS ARE LOAD-BEARING and each was verified by REMOVAL (ref 57 encodings):
 *
 *   `u16 c`                      58 lines, 58 differ -- HImode drags the mask 31
 *                                into the literal POOL (`ldrh r2, .L13 / .word 31`)
 *   `int c` (signed)             58 lines, 40 differ -- `asr` where the ROM has `lsr`
 *   the NAMED mask `m = 0x1f`    without it, 59 lines and 40 differ
 *   the CHAINED channel form     one expression per channel is 58 lines / 40 differ
 *
 * WHY THE NAMED MASK MATTERS, read out of .17.lreg and .18.greg. Two loop-carried
 * values compete for r7/r8, ordered by global-alloc priority
 * floor_log2(n_refs)*freq/live_length. As an anonymous CSE'd constant the mask is a
 * COMPILER pseudo whose live range ends at its last `and` -- shorter than the
 * counter's, which reaches the bottom-of-loop test -- so the mask wins r7 and the
 * counter is exiled to r8, where Thumb cannot `mov` an immediate and every
 * decrement costs `mov r3,#1 / neg r3 / add r8,r3 / mov r3,r8` against the ROM's
 * `sub r7, #1`. Giving the mask a NAMED USER VARIABLE reorders the allocno list
 * from `36 37 35 32 60 33` to `36 37 35 32 33 39` -- counter before mask -- and the
 * ROM's decrement falls out.
 *
 * The second half is the chaining: `x = c >> 10; x &= m; x += t;` on ONE variable
 * ties the shift temp and the component to the same hard register, giving the ROM's
 * destructive `add r3, r0` rather than a three-operand `add r1, r3, r0`. Green's and
 * blue's splits are required; RED'S IS OPTIONAL -- flattening red alone is still
 * exact, flattening green is 38 differing and blue 8.
 *
 * A `register unsigned int m __asm__("r8")` pin is byte-identical to the unpinned
 * named variable, so the pin buys nothing and none is shipped. Two separate
 * pointers instead of `p[0x10]` spill to r10 (64 lines, 63 differing).
 */
#include "gba/types.h"
#include "math.h"
extern int iwram_3001e40;
void Func_80b8f58(void)
{
    u16 *p; int i, t; unsigned int r, g, b, c, m;
    p = (u16 *)0x50001c0;
    m = 0x1f;
    for (i = 0xf; i >= 0; i--) {
        c = p[0x10];
        t = (0x10000 - cos(iwram_3001e40 * 3 << 10)) / 0x2aaa;
        r = c >> 10; r &= m; r += t;
        g = c >> 5;  g &= m; g += t;
        b = c & m;   b += t;
        if (r > 0x1f) r = 0x1f;
        if (g > 0x1f) g = 0x1f;
        if (b > 0x1f) b = 0x1f;
        *p++ = (r << 10) | (g << 5) | b;
    }
}
