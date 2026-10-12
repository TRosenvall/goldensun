/* Func_80160fc -- FlushUiToVram.  Per-frame task: while the suspend byte at
 * iwram_3001e8c+0xEA6 is clear, read the dirty mask at +0xEA3 and DMA one
 * 0x100-byte block to 0x6002000 per set bit, advancing source and destination
 * by 0x100 each time; bit 0 means "all of it" and substitutes 0x3F.  The mask
 * is shifted right once before the loop and cleared after it.
 *
 * Converted from asm/rom_15000/rom_15e8c_a_c_a_a_c_a_c.s, the whole file -- one
 * function, no split.  Landed by batch 339 brief C off a park that was PROSE
 * until batch 338 recovered its body from scratch/.
 *
 * THE PARK WAS AT "ref 50 INSTRUCTIONS AGAINST OURS 49" AND BLAMED THE WRONG
 * THING, twice.  Both corrections are worth keeping.
 *
 * ONE: the `goto` loop was WRONG, and the reason it was believed is instructive.
 * The park argued the loop could not be a `do`/`while` because the ROM keeps
 * `ldr r3, =REG_DMA3SAD` and `ldr r2, =0x84000040` INSIDE the loop body, which
 * docs/elevation.md records as the signature of a `goto` loop ("`goto` loops
 * disable loop optimisation ENTIRELY").  But those two loads sit inside the
 * `if ((m & one) != 0)` arm, so they are CONDITIONAL, and loop.c will not hoist
 * an insn that may never execute -- the conditional is what keeps them in, not
 * the absence of NOTE_INSN_LOOP_BEG.  Meanwhile `step` IS unconditional, and
 * the ROM has it hoisted (`mov r6,#0x80 / lsl r6,#1` in the preheader, then
 * `add r5, r6` / `add r12, r6` in the body), which REQUIRES loop optimisation
 * to be ON.  A `do { } while (m != 0);` gives both.
 *
 * > A loop-invariant rebuilt inside the ROM's loop body is only evidence for a
 * > `goto` loop when the rebuild is UNCONDITIONAL.  Check which arm it is in
 * > first; loop.c declines to hoist out of a conditional arm on its own.
 *
 * TWO: deleting the `step` local is what CLOSED THE LENGTH GAP, and it closed it
 * by making the function LONGER -- the opposite of what naming a constant
 * usually does.  With `step` named and the loop a `goto`, gcc rebuilt 0x100
 * inside the body (`mov r3,#0x80 / lsl r3,#1`, two insns per iteration) and `d`
 * fitted in low r7 (`ldr r7, =0x6002000`, one insn).  With the increment written
 * as the plain `0x80 << 1` at both uses and the loop a `do`/`while`, loop.c
 * hoists the invariant into the preheader, which costs two insns there and saves
 * two in the body -- net zero -- and the extra live value then pushes `d` into
 * HIGH r12, which costs `ldr r3, =0x6002000 / mov r12, r3` instead of one `ldr`.
 * That +1 is the whole gap.  The ROM's register set (r4-r7 plus r12 AND r14) is
 * the tell: it is one long-lived value richer than ours was.
 *
 * THREE SMALLER THINGS, each load-bearing:
 *   * `m` must be `unsigned`.  The ROM shifts it with `lsr`, twice.
 *   * the mask must be DESTRUCTIVE -- `m &= 0x3f; m >>= 1;` gives the ROM's
 *     `and r4, r3` / `lsr r4, #1`, where `m = (m & 0x3f) >> 1;` gives
 *     `and r3, r4` / `lsr r4, r3, #1`.  Same instruction count, different
 *     instructions.
 *   * `s = b;` must be born right after `d`, BEFORE the `m & 1` test.  The four
 *     other positions tried score 12, 8, 9 and 10 differing.
 *
 * AND THE PARK'S STATED BLOCKER WAS ALREADY SOLVED IN ITS OWN BODY.
 * docs/elevation.md's "The INVERSE constant problem: the ROM derives, gcc does
 * not" cites this function and says, of `off = 0xea6; ... off -= 3;`, that
 * "writing the offset as one variable mutated in place does not reach it".  IT
 * DOES: this body uses exactly that spelling and emits the ROM's `sub r2, #0x3`,
 * and it did so in the recovered park body too -- the first thirteen
 * instructions matched before anything was changed.  That doc entry needs the
 * correction; the shape it describes as "not currently reachable" is reachable
 * and has been for at least one batch.
 *
 * MEASURED (ref 50 instructions / 54 tryc lines):
 *   the park body, `goto` + named `step` + signed `m`      49 insns, 42 differing
 *   ... + `m` unsigned                                     49 insns, 42
 *   ... as a `do`/`while` instead                          49 insns, 42
 *   ... + destructive mask                                 49 insns, 42
 *   ... without the `one` local                            49 insns, 42
 *   ... as a `while (1)` with a `break`                     49 insns, 42
 *   `do`/`while` + literal increment, expression mask      50 insns, 10
 *   ... + destructive mask, `s = b;` at four other places   50 insns, 12/8/9/10
 *   ... + destructive mask, `s = b;` after `d`              50 insns, 0  <- this
 *
 * Verified: 124 bytes, 56 encodings and 1 relocation identical.
 */
#include "dma.h"

extern char *iwram_3001e8c;

void Func_80160fc(void)
{
    char *b;
    char *s;
    char *d;
    unsigned char *q;
    int off;
    unsigned int m;
    int one;

    b = iwram_3001e8c;
    off = 0xea6;
    q = (unsigned char *)(b + off);
    if (*q != 0)
        return;
    off -= 3;
    q = (unsigned char *)(b + off);
    m = *q;
    if (m == 0)
        return;
    d = (char *)0x6002000;
    s = b;
    if ((m & 1) != 0)
        m = 0x3f;
    m &= 0x3f;
    m >>= 1;
    one = 1;
    do {
        if ((m & one) != 0)
            DMA3_COPY(s, d, 0x100);
        m >>= 1;
        s += 0x80 << 1;
        d += 0x80 << 1;
    } while (m != 0);
    *(unsigned char *)(b + 0xea3) = m;
}
