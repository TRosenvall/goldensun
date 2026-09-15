/* Func_80919d8 -- 0x080919d8, asm/rom_8a000/rom_91584_c_a_c_c_c_a_c.s (three
 * functions; this is the first).
 *
 * BLOCKER CLASS: A BARE POOL LOAD HOISTED ABOVE A CALL. Size exact -- 128
 * bytes, 56 instructions against 56 -- with 6 encodings differing, all of them
 * one `ldr` in the wrong place and the argument order it drags with it.
 *
 * THE BODY IS SETTLED and reproduces on the first candidate. The party loop is
 * the Func_808c2dc idiom (`gState[(0xfc << 1) + i]` under an ascending `for`),
 * the threshold `n * 30` is the ROM's `lsl #4 / sub / lsl #1`, and gcc's own
 * trick of reusing the accumulator's initial zero as the loop guard falls out
 * of the plain source -- `total = 0;` then `for (i = 0; i < n; i++)` gives the
 * ROM's `mov r10, r2 / cmp r10, r6 / bge` with no help.
 *
 * THE RESIDUE:
 *
 *     rom    bl _Func_8019908 / ldr r5, =0x97d / mov r1, #1 / mov r0, r5 / bl
 *     ours   ldr r5, .L10+4 / bl _Func_8019908 / mov r0, r5 / mov r1, #1 / bl
 *
 * The ROM loads the constant AFTER the first call returns; we hoist it above.
 * The constant has no dependence on anything, so sched2 is free to place it
 * anywhere, and it chooses earliest. The argument order at that call site then
 * follows: with the value already in a register we fill r0 first, where the ROM
 * fills r1 first.
 *
 * NOTE the ROM fills r0 LAST at the first _Func_801776c call and FIRST at the
 * second, in one function -- which the notebook records as normal and as
 * evidence about declarations. Here it is NOT a declaration fact: return-type
 * variations on both callees were measured (int/void, void/int, int/int) and
 * all are INERT or worse, 7, 7 and 6 against the baseline 6. The order follows
 * the pool load, not the prototype.
 *
 * MEASURED AND INERT, all 6:
 *   `id = 0x97d` assigned after the first call, before it, or at the top of the
 *     guarded block
 *   `id++` between the calls instead of `id + 1` at the second
 *   three return-type combinations on _Func_8019908 and _Func_801776c
 * MEASURED AND WORSE:
 *   two bare literals 0x97d and 0x97e             25 differing, one short
 *   -fno-schedule-insns2                          13 differing
 *   -fno-schedule-insns                            6, unchanged
 *
 * -fno-schedule-insns2 BEING WORSE IS THE USEFUL PART: it says the ROM was
 * built with sched2 ON and its placement IS the scheduled one, so this is a
 * priority decision inside the pass and not a case for a SCHED2_CFLAGS rule.
 *
 * THIS IS THE SECOND SPECIMEN OF THE CLASS THIS SESSION. Func_80a8578's
 * residue under its pin is the same shape:
 *
 *     rom    bl Func_8004938 / mov r5, r0 / ldr r0, =0xbe6 / ...
 *     ours   bl Func_8004938 / ldr r3, .L7+8 / mov r5, r0 / ...
 *
 * In both, the ROM finishes the preceding statement and then loads a pool
 * constant into a register that has just been freed; we hoist the load and it
 * takes a different register. The batch-265 sched2 lever (move the ARITHMETIC
 * in the source) does not reach either one, because in both cases the hoisted
 * insn is a bare load with nothing to move.
 *
 * NEXT: the open question is what gives a pool load a LOWER sched2 priority
 * than the call setup around it. Two specimens now share it, so it is worth
 * reading haifa-sched.c's rank_for_schedule rather than sweeping a third
 * function's spellings.
 */
#include "gba/types.h"

extern unsigned char gState[];
extern int _GetPartySize(void);
extern int _Func_8078af8(int id, int a);
extern void _Func_8019908(int a, int b);
extern void _Func_801776c(int a, int b);

int Func_80919d8(int a)
{
    int total;
    int n;
    int i;
    int id;

    total = 0;
    n = _GetPartySize();
    for (i = 0; i < n; i++)
        total += _Func_8078af8(gState[(0xfc << 1) + i], a);
    if (total >= n * 30) {
        _Func_8019908(a, 2);
        id = 0x97d;
        _Func_801776c(id, 1);
        _Func_8019908(a, 2);
        _Func_801776c(id + 1, 1);
        return -1;
    }
    return 0;
}
