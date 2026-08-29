/* Func_801ce90 (SelectByPhase) -- NON-MATCHING.
 * Blocker class: REGISTER-ROLE SWAP.  16 of 38, and the STRUCTURE IS EXACT --
 * every instruction is the right instruction in the right place; r1 and r2
 * are exchanged throughout.
 *
 *     rom    ldr r3, =gState / mov r2, #0x83 / lsl r2, #2 ... add r1, r3, r2
 *     ours   ldr r3, =gState / mov r1, #0x83 / lsl r1, #2 ... add r2, r3, r1
 *
 * THREE LEVERS GOT IT FROM 24 DIFFERING AND SIX LINES SHORT TO THIS, and the
 * order they had to be applied in is the point:
 *
 *   1. NAME THE OFFSET, NOT THE BASE (docs/elevation.md, batch 137). Written
 *      as `p = gState + 0x205`, gcc folds the whole thing into ONE pool entry
 *      `=gState+517`; the ROM keeps `ldr r3, =gState` and the offset separate.
 *      Naming just the offset was not enough here.
 *   2. NAME THE BASE TOO, INSIDE EACH ARM. The ROM loads `=0x2000240` in all
 *      THREE switch arms rather than once after the join, so `g = gState;`
 *      belongs in each arm. That is what stops the fold: with the base in a
 *      register gcc has nothing to fold the offset into.
 *   3. AN INT INTERMEDIATE FOR THE DECREMENT. The ROM has `mov r3, r2` then
 *      `add r3, #0xff` -- a copy, then a byte-wrapping add. Written as
 *      `*p = *p - 1` gcc emits `sub r3, #1` and no copy; written as
 *      `v = *p; t = v; t += 0xff;` it emits both.
 *
 * Note that lever 1 alone made it WORSE in the sense that mattered -- 23
 * differing and three lines short -- and only became right once lever 2 was
 * added. "Name the offset" and "name the base" are usually alternatives; here
 * the ROM wanted both, because the base is reloaded per arm.
 *
 * Carrying the offset out of the switch and adding after the join (`p = g +
 * off` once) gives 19, worse than doing the add inside each arm, even though
 * the ROM's `add r1, r3, r2` sits after the join. gcc reassociates it either
 * way; only the register assignment differs, and no spelling reaches that.
 */
extern unsigned char gState[];

void Func_801ce90(char *rec)
{
    unsigned short *ph;
    unsigned char *g;
    unsigned char *p;
    int v;
    int t;

    rec += 0x574;
    ph = (unsigned short *)rec;
    switch (*ph) {
    case 0:
        g = gState;
        p = g + (0x83 << 2);
        break;
    case 1:
        g = gState;
        p = g + 0x205;
        break;
    case 2:
        g = gState;
        p = g + 0x206;
        break;
    default:
        return;
    }
    v = *p;
    t = v;
    if (t == 0)
        return;
    t += 0xff;
    *p = t;
}
