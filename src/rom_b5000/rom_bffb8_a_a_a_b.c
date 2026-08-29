/* Cluster Func_80c0098..Func_80c0098 extracted from goldensun/asm/rom_b5000/rom_bffb8_a_a_a.s.
 *
 * Preserves the original ROM layout when slotted between
 * asm/rom_b5000/rom_bffb8_a_a_a_a.o and asm/rom_b5000/rom_bffb8_a_a_a_c.o in
 * goldensun/stage1.ld.
 *
 * Writes two ramps of packed byte-index words -- 0x03020100, 0x07060504, ... --
 * 64 words then 56 words, and then clears 0x220 bytes past the end through
 * Func_80008d8 on the `_call_via_r3` veneer.
 *
 * THE LEVER HERE IS THE OPPOSITE OF THE NAMED-INTERMEDIATE ONE. The step
 * 0x4040404 must be written as a LITERAL in the loop body, not as a named local
 * assigned before the loop:
 *
 *     v += 0x4040404;     ->  ldr r1, =0x4040404 hoisted ABOVE the loop  (ROM)
 *     v += step;          ->  ldr r1, =0x4040404 left INSIDE the loop
 *
 * Same instruction count either way -- 27 against 27 -- and only the placement
 * differs, which is why it is worth writing down. Naming the value made gcc
 * treat it as a live variable to be materialised where it is used; leaving it a
 * literal let loop-invariant motion lift it out, which is what the ROM does.
 *
 * That is the reverse of the usual named-intermediate lever, where naming a
 * computed value is what stops gcc folding it into its consumer. Both are
 * about the same question -- whether gcc gets to move a value -- and the answer
 * runs in opposite directions inside and outside a loop.
 *
 * The step is re-materialised for the SECOND loop as well, in both the ROM and
 * ours, because `v` is modified in the first loop and has to be reset. That is
 * not the constant-CSE blocker; nothing is being cached here that should not
 * be.
 *
 * The loop shape is `do { i++; ... } while (i <= N)` with an UNSIGNED counter,
 * matching the ROM's `bls`. A signed counter gives `ble`.
 *
 * r0 is never reloaded before the final call -- the pointer has already been
 * advanced past both ramps by the stores, and gcc keeps it there.
 */
#include "gba/types.h"

extern void Func_80008d8(void *dst, u32 size, u32 value);

void Func_80c0098(u32 *p)
{
    u32 v, i;
    void (*fp)(void *, u32, u32);

    v = 0x3020100;
    i = 0;
    do { i++; *p++ = v; v += 0x4040404; } while (i <= 0x3f);
    v = 0x3020100;
    i = 0;
    do { i++; *p++ = v; v += 0x4040404; } while (i <= 0x37);
    fp = Func_80008d8;
    fp(p, 0x88 << 2, -1);
}
