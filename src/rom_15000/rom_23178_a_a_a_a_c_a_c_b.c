/* Func_80270d8 -- 0x080270d8, split out of asm/rom_15000/rom_23178_a_a_a_a_c_a_c.s.
 *
 * Fills a 128-byte stack buffer with Func_801965c and hands it to Func_8017aa4
 * along with a word read through the static chain.
 *
 * THE r9 TRAFFIC IS A STATIC CHAIN: THE ORIGINAL WAS A NESTED FUNCTION. All
 * three tells of the recorded class are present -- r9 saved in the prologue and
 * restored in the epilogue, a `mov r2, r9` that no instruction here ever
 * defines, and a lone stack slot at sp+0x80 that nothing reads back. The caller
 * settles it: Func_8027114, still in _c.s, does
 *
 *     add r2, sp, #0x64 / mov r9, r2 / bl Func_80270d8
 *
 * which is the corpus-wide grep for this class -- gcc handing a callee a
 * pointer into the caller's own frame. So `chain - 8` is a local of the parent,
 * not an address in its own right.
 *
 * THIS IS A TRANSCRIPTION, NOT THE ORIGINAL SHAPE. A standalone TU cannot
 * declare a nested function, so the chain is an uninitialised `register` bound
 * to r9 copied into a volatile slot, per Func_8015fb8 next door. Unlike that
 * case the parent is UNREACHABLE for now -- Func_8027114 is the 2004-line
 * RunMainMenuScreen, the largest function in rom_15000 -- so the nested form
 * cannot be written until that is elevated. Whoever gets there should collapse
 * this pair rather than preserve the workaround.
 *
 * TWO NEW RESULTS, both measured.
 *
 * 1. A SECOND CALL-SAVED REGISTER MUST BE BOUGHT, AND r9 IS NOT FREE TO SPEND.
 *    The ROM keeps `chain - 8` in r5 across the call and `buf` in r6, pushing
 *    {r5, r6, lr}. Deriving the pointer from a PLAIN local copy of the register
 *    variable gets the value right but gcc allocates it to *r9* -- the register
 *    is already in the save mask because of the binding, so reload treats it as
 *    costing nothing and hands it out ahead of r6. That is 17 differing, and it
 *    is invisible if you only read the register numbers as "a rotation".
 *
 *    Deriving the pointer from the VOLATILE OBJECT instead keeps r9 reserved
 *    and the allocation falls out as the ROM has it: 17 -> 4.
 *
 * 2. gcc-2.96 FORWARDS A VOLATILE OBJECT'S VALUE IN A REGISTER. `q = chain;`
 *    after `chain = _chain;` emits NO LOAD -- the output has no read of sp+0x80
 *    anywhere, and `mov r5, r2` takes the value from the register before the
 *    `str` that writes the slot. So the second read costs nothing while still
 *    being a separate use for the scheduler, which is the whole trick here.
 *
 * THE LAST TWO INSTRUCTIONS WERE THE RETURN TYPE, again. The ROM ends
 * `pop {r1} / bx r1`; a void function pops into r0 because r0 is dead. Giving
 * the function a return type it never returns reserves r0 and moves the pop to
 * r1 -- the same rule Func_8015fb8 recorded, worth 2 of the last 4.
 *
 * THE FINAL 2 WERE sched2, NOT SPELLING. `mov r2, #0x34` and `sub r5, #8` came
 * out swapped, and stayed swapped through six spellings that all measured 2:
 * naming the count, naming the address constant, `(u32 *)chain - 2`, indexing
 * buf, and computing the offset into a local before the call.
 * `-fno-schedule-insns2` moves them, `-fno-schedule-insns` and
 * `-fno-peephole2` do not, so the second scheduling pass owns the order.
 * What fixes it is writing the SUBTRACTION AFTER THE CALL: read the chain into
 * a plain local before it, subtract after, and sched2 hoists the `sub` back to
 * exactly the ROM's slot. Moving the whole pointer computation after the call
 * (`q = chain - 8` before, `p = (u32 *)q` after) does NOT work and still
 * measures 2 -- it is where the arithmetic sits that matters, not the pointer.
 *
 * EXACT: 60 bytes, 27 encodings, 2 relocations, measured three times.
 */
#include "gba/types.h"

extern int Func_801965c(int a, u16 *out, u32 n);
extern void Func_8017aa4(void *buf, int b, int c, int d);

int Func_80270d8(void)
{
    volatile u32 chain;
    u16 buf[0x40];
    register u32 _chain __asm__("r9");
    u32 *p;
    u32 q;

    chain = _chain;
    q = chain;
    Func_801965c(0x80d, buf, 0x34);
    p = (u32 *)(q - 8);
    Func_8017aa4(buf, *(int *)(*p + 0x44), 0, 4);
}
