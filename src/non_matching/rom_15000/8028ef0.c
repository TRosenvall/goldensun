/* Func_8028ef0 -- 0x08028ef0, asm/rom_15000/rom_23178_a_c_a.s (single-function
 * file, so it would convert WHOLE with no split).
 *
 * BLOCKER CLASS: SCRATCH REGISTER EXCHANGE, r2 against r3. Size exact -- 168
 * bytes, 73 instructions against 73 -- with 20 encodings differing and NOTHING
 * else wrong. Every instruction is present, in the ROM's order, with the ROM's
 * operands.
 *
 * The residue is a single consistent exchange between two short-lived temps:
 *
 *     rom    ldr r3, =0x99b ... add r10, r3      |  mov r2, #0xe / str r2, [sp]
 *     ours   ldr r2, .L3    ... add sl, sl, r2   |  mov r3, #14  / str r3, [sp]
 *
 * The pool temp for 0x99b and the constant temp for 0xe have swapped registers,
 * and every instruction mentioning either follows. gcc's REG_ALLOC_ORDER hands
 * out call-clobbered registers {3, 2, 1, 0, ...}, so r3 goes first -- which is
 * what we get. The ROM gives r3 to the LATER temp.
 *
 * THIS FUNCTION IS A SINGLE BASIC BLOCK. There are no branches at all, so there
 * is no control flow to lever against, no loop for an induction variable to
 * come out of, and nothing for a `goto` or a barrier to split. That makes it a
 * clean specimen of the tie and a bad one to attack by spelling.
 *
 * MEASURED AND INERT, all 20:
 *   0xe as three bare literals, or named in a local assigned after the first
 *     call (the two spellings that keep the size exact)
 *   `short v` parameter, `short v` local, `(s16)id` cast into an int local
 *   `void *w` against `int w`
 *   `name = f(...); name += 0x99b;` and `name = 0x99b + f(...);`
 *   -fno-schedule-insns
 * MEASURED AND WORSE:
 *   0xe named and assigned BEFORE the first call    172 bytes, 75 instructions
 *   -fno-schedule-insns2                            30 differing
 *
 * -fno-schedule-insns2 being worse says the ROM was built with sched2 ON, so
 * the interleave we do have is the scheduled one and this is not a case for a
 * SCHED2_CFLAGS rule.
 *
 * NEXT -- AND THIS IS NOW THE FOURTH FUNCTION IN A ROW. Func_942e0,
 * Func_80cd52c, Func_80a8578 and this one all end on a local-alloc decision
 * that no source spelling moves, and all four report `;; 0 regs to allocate` in
 * .18.greg. Three of them are priority ties between named values; this one is
 * the same thing between anonymous temps, which is the cleanest form of it.
 *
 * STOP SWEEPING SPELLINGS ON THIS CLASS. The next person should read
 * local-alloc.c -- `qty_compare_1` for the ordering and `find_free_reg` for how
 * a quantity picks from REG_ALLOC_ORDER once ordered -- and find what feeds the
 * decision besides the published priority formula. Four independent specimens
 * are now available to test any hypothesis against, which is more than any
 * single function's sweep could give.
 */
#include "gba/types.h"

extern unsigned char L37428[] __asm__(".L37428");
extern int _GetLocationName(int a, int b);
extern void Func_8016478(void *w);
extern void Func_801e9a0(int a, int b, void *w, int c, int d);
extern void Func_801e858(void *s, void *w, int x, int y);
extern void DrawSmallText(int id, void *w, int x, int y);

void Func_8028ef0(void *w, int id, s16 *p)
{
    int v;
    int name;

    v = (s16)id;
    name = _GetLocationName(v, *p) + 0x99b;
    Func_8016478(w);
    Func_801e9a0(v, 3, w, 0, 0xe);
    Func_801e9a0(*p, 3, w, 0x52, 0xe);
    Func_801e858(L37428, w, 0x4a, 0);
    DrawSmallText(v + 0xa07, w, 0, 0);
    Func_801e858(L37428, w, 0x4a, 0xe);
    DrawSmallText(name, w, 0x52, 0);
}
