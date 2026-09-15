/* Func_80cd52c -- 0x080cd52c, asm/rom_c9000/rom_cd508_a_c.s (five functions;
 * this is the first, so a split would leave _a and _b).
 *
 * BLOCKER CLASS: LOOP DIRECTION. check_dbra_loop reverses our ascending loop
 * into a countdown; the ROM's stays ascending. SIZE IS EXACT -- 104 bytes, 48
 * instructions against 48 -- and 36 encodings differ, which is one counter
 * running the other way plus the register cascade that follows it.
 *
 * WHAT IT DOES: walks eight timer bytes at iwram_3001eec[0x7818 + i],
 * decrements each non-zero one, and when a decrement reaches zero calls
 * Func_80d6888 with the matching s16 at [*(base+0x7828)] + 0x24 + 2*i.
 *
 * TWO DEFECTS WERE FOUND AND CLOSED. Do not re-derive them.
 *
 * 1. `m = 0; m--;` IS WHAT PUTS -1 IN A REGISTER, AND IT IS A NEW LEVER.
 *    The ROM spends a whole callee-saved register on the constant -- `mov r1,#1
 *    / neg r1,r1 / mov r8,r1` in the preheader, then `mov r2,r8 / mov r3,r8` at
 *    the two argument sites -- and pays a four-instruction r8 save/restore for
 *    it. A plain `m = -1;` does NOT reproduce that: gcc rematerialises the
 *    constant at each use (`mov r3,#1 / mov r2,#1 / neg r2,r2 / neg r3,r3`
 *    inside the loop), never allocates r8, and comes out 92 bytes against 104.
 *    Writing the same value as an arithmetic result defeats the constant
 *    propagation that makes remat look free, and the value survives into a
 *    register. Worth 12 bytes.
 *
 *    This is the counterpart to batch 264's "a named constant can cost a whole
 *    callee-saved register": there, NAMING a value was enough. Here naming is
 *    not enough, because -1 is cheap enough to rebuild. THE LEVER IS NOT THE
 *    NAME, IT IS WHETHER gcc CAN STILL SEE A CONSTANT.
 *
 * 2. THE BYTE OFFSET MUST BE ITS OWN VARIABLE. The ROM carries THREE induction
 *    variables -- the counter i, the byte pointer p, and the s16 offset off --
 *    incrementing all three (`add r4,#1 / add r7,#2 / add r5,#1`). Writing the
 *    s16 access as an INDEX and letting strength_reduce derive the offset gives
 *    only two and is 96 bytes: measured on both `[0x12 + i]` and
 *    `+ 0x24 + i * 2`. Keeping `off` explicit alongside `p` is what makes the
 *    size exact.
 *
 *    That is worth stating because it runs the other way from the batch-265
 *    rule. There, transcribing strength_reduce's output was the mistake. Here
 *    the ROM genuinely has the third variable, and the tell is that all three
 *    increments are present with none derivable from another by a constant the
 *    addressing mode could fold.
 *
 * THE RESIDUE. Our `i` is only ever read by the loop test, so check_dbra_loop
 * reverses it: `mov r7,#7 / sub r7,#1 / cmp r7,#0 / bge`, with the counter in a
 * callee-saved register. The ROM keeps `mov r4,#0 / add r4,#1 / cmp r4,#8 /
 * bne` and puts the counter in r4, which is CALL-CLOBBERED under
 * -fcall-used-r4, so it pays `str r4,[sp,#4]` / `ldr r4,[sp,#4]` around the
 * call to keep it. gcc chose to spend a spill rather than reverse the loop, and
 * we cannot get it to.
 *
 * LOOP FORM IS INERT -- `for (i = 0; i < 8; i++)`, `for (i = 0; i != 8; i++)`,
 * `do { } while (i < 8)` and `do { } while (i != 8)` all measure 36 with the
 * same reversal. Making `i` appear in the BODY (so it cannot be dead after the
 * test) does stop the reversal, but only by removing the third induction
 * variable, which costs more than it saves: 96 bytes, 44 instructions.
 *
 * NEXT: the question is what makes check_dbra_loop decline. Candidates not yet
 * read: whether the spill of the counter is a CAUSE rather than a consequence
 * (i.e. gcc reverses only when the counter is cheap to hold), and whether the
 * call in the body changes the reversal cost estimate. Read loop.c's
 * check_dbra_loop conditions rather than sweeping more spellings -- eight loop
 * spellings have now measured identically and the axis is exhausted.
 */
#include "gba/types.h"

extern unsigned char *iwram_3001eec;
extern void Func_80d6888(int t, int color, int sanim, int idx, int dur);

void Func_80cd52c(void)
{
    u8 *base;
    u8 *p;
    int i;
    int off;
    int m;
    int t;

    base = iwram_3001eec;
    m = 0;
    m--;                        /* NOT `m = -1` -- see 1 above */
    p = base + 0x7818;
    off = 0x24;
    for (i = 0; i < 8; i++) {
        t = *p;
        if (t != 0) {
            t--;
            *p = t;
            if ((u8)t == 0)
                Func_80d6888(*(s16 *)(*(u8 **)(base + 0x7828) + off), 0, m, m, 0);
        }
        off += 2;
        p++;
    }
}
