/* Task_BlitAnim -- NOT MATCHING
 *
 * UPDATED IN BATCH 105: 29 differing to 3, by the BASIC-BLOCK LEVER.
 *
 * The constant hoist named below IS reachable. Giving the two 0x4000 uses in
 * case 1 their own named locals assigned in a dominating block -- and the
 * single use in case 0 a third -- makes gcc rematerialise each one instead of
 * building it once and keeping it in a callee-saved register. The prologue
 * comes back to `push {r5, r6, lr}` and the whole r5/r6/r7 renumbering goes
 * with it.
 *
 * The dosage matters and is not monotonic. Two locals (case 1 only) gives 5;
 * adding one for case 0 gives 3; giving all seven use sites their own local
 * gives 11, worse than two. That is the REG_N_REFS == 2 clause in the lever's
 * write-up biting from the other side -- past some point CSE merges the
 * locals back into one pseudo and the pseudo is referenced too often.
 *
 * WHAT IS LEFT IS THE POOL-LOADS-FIRST CLASS, three instructions on case 1's
 * first call:
 *
 *     rom    mov r1, r5 / lsl r2, #7 / ldr r0, =0x6004000
 *     ours   ldr r0, =0x6004000 / mov r1, r5 / lsl r2, #7
 *
 * gcc emits the literal-pool load before any `mov` in the same argument block;
 * the ROM emits it in source order. That is the class documented at
 * src/non_matching/overlays/pool_load_first.c, where the only construct known
 * to reach it is register pinning with inline asm. Staging the destination
 * through a named local in a dominating block -- the one thing the lever adds
 * to that class -- was tried and leaves it at 3.
 *
 * The original park text follows.
 *
 * Source asm: goldensun/asm/rom_c9000/rom_cd260_a.s
 * Best screen when this was written: 105 against the ROM's 105, 29 differing.
 * Best screen NOW: 3 differing, per the batch-105 note above.
 *
 * BLOCKER CLASS: a constant hoisted across a call.
 *
 * Case 1 makes two indirect calls and both take a length of 0x4000. 0x4000 is
 * not a Thumb immediate, so gcc materialises it as `mov #0x80 / lsl #7` -- and
 * having built it once, it keeps it in a CALLEE-SAVED register across the
 * first call and copies it into place for the second:
 *
 *     rom    mov r2, #0x80 / ... / lsl r2, #7 / bl   ...   mov r1, #0x80 / lsl r1, #7 / bl
 *     ours   mov r5, #0x80 / lsl r5, #7 / ... / bl   ...   mov r2, r5 / bl
 *
 * That third live value is why the prologue is `push {r5, r6, r7, lr}` where
 * the ROM has `push {r5, r6, lr}`, and the extra register renumbers r5/r6/r7
 * through the whole body -- which is most of the 29. Kill the hoist and the
 * function should close.
 *
 * FLAGS DO NOT REACH IT. Screened under -fno-gcse, -fno-rerun-cse-after-loop,
 * -fno-cse-follow-jumps and -fno-force-mem: all identical at 32.
 * -fno-expensive-optimizations is worse (110 lines, 110 differing). This is
 * local CSE, not global, which is consistent with the flags having no effect.
 * Worth noting that the sibling function out of this same .s,
 * src/rom_c9000/rom_cd260_b.c, IS built with -fno-rerun-cse-after-loop, so the
 * flag was the first thing tried.
 *
 * Source spellings tried: the fill value through its own `val` local (this
 * file, 29 -- three better than inlining it) and one function-pointer local
 * reused for both callees instead of two (34).
 *
 * This is the same family as the residue on the 1000+ instruction scouting
 * park src/non_matching/overlays/200b4c8.c, where about fifteen pooled
 * constants are hoisted into callee-saved registers. It is the smallest
 * instance of it found so far and therefore the best place to work on it.
 *
 * WHAT IS RIGHT: the `_call_via_r3` idiom from src/rom_c9000/rom_e0524.c for
 * both indirect calls; the two-word read of iwram_3001eec through
 * `pp = &iwram_3001eec; b = pp[0]; s = pp[1];` with pp[1] read INSIDE the
 * guard; the 0x7780 field addressed so gcc derives it from the 0x7824 already
 * in a register (`sub r2, #0xa4`); and the tail, where the two writes to
 * +0x7820 cross-jump into one `str` because both arms end on the same store.
 */
#include "gba/types.h"

typedef void (*CopyFn)(void *dst, void *src, u32 len);
typedef void (*FillFn)(void *dst, u32 len, u32 value);

extern void Func_8001af8(void *dst, void *src, u32 len);
extern void Func_80008d8(void *dst, u32 len, u32 value);
extern void BlitFade_Div2(void *src, void *dst, u32 len);
extern void BlitFade_Div4(void *src, void *dst, u32 len);
extern void BlitFade_Sub(void *src, u32 amt, void *dst, u32 len);
extern void BlitFade_Add(void *src, u32 amt, void *dst, u32 len);
extern unsigned char *iwram_3001eec;

void Task_BlitAnim(void)
{
    unsigned char **pp;
    unsigned char *b;
    void *s;
    CopyFn copy;
    FillFn fill;
    u32 val;
    u32 len1;
    u32 len2;
    u32 len0;

    len1 = 0x4000;
    len2 = 0x4000;
    len0 = 0x4000;
    pp = &iwram_3001eec;
    b = pp[0];
    if (*(int *)(b + 0x7824) == 1) {
        s = pp[1];
        switch (*(int *)(b + 0x7780)) {
        case 0:
            copy = Func_8001af8;
            copy((void *)0x6004000, s, len0);
            break;
        case 1:
            copy = Func_8001af8;
            copy((void *)0x6004000, s, len1);
            val = *(u32 *)(b + 0x7784);
            fill = Func_80008d8;
            fill(s, len2, val);
            break;
        case 2:
            if (*(int *)(b + 0x7784) == 0x32)
                BlitFade_Div2(s, (void *)0x6004000, 0x4000);
            else
                BlitFade_Div4(s, (void *)0x6004000, 0x4000);
            break;
        case 3:
            BlitFade_Sub(s, *(u32 *)(b + 0x7784), (void *)0x6004000, 0x4000);
            break;
        case 4:
            BlitFade_Add(s, *(u32 *)(b + 0x7784), (void *)0x6004000, 0x4000);
            break;
        }
        *(int *)(b + 0x7824) = 0;
        *(int *)(b + 0x7820) = 1;
    } else {
        *(int *)(b + 0x7820) = *(int *)(b + 0x7820) + 1;
    }
}
