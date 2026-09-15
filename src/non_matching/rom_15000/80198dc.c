/*
 * ### BATCH 265 -- ADDITIONS. The prior note below is intact and still correct;
 * ### its "allocation shifted one register down, whole" reading is confirmed.
 *
 * Re-measured from a fresh draft: 7 of 20 encodings (the note's "12 differing"
 * counted a different candidate). The residue now has TWO parts, and the second
 * is not in the note:
 *
 *     rom   add r1,#1 / stmia r3!,{r0} / strh r0,[r2] / add r2,#2
 *     ours  add r1,#1 / strh r0,[r2]   / stmia r3!,{r0} / add r2,#2
 *
 * THE TWO STORES ARE SWAPPED, and that is a separate defect from the register
 * rotation. They go through DIFFERENT pointer types (`int *` and `short *`), so
 * they sit in different alias sets and sched2 may order them freely.
 *
 *   This is the recorded alias device running BACKWARDS. The usual lever gives
 *   two accesses two distinct tags so one cannot be hoisted over the other.
 *   Here they ALREADY have distinct tags, and that is the problem.
 *
 * NEW MEASUREMENTS (20 encodings unless noted), none of which the note covers:
 *     stores written in the opposite source order      52 bytes against 44
 *     `*q = z; q++;` split instead of `*q++ = z;`      48 bytes against 44
 *     named zero local                                  7 (inert)
 *     unsigned char* second pointer with a cast         7 (inert)
 *     both increments folded into the stores            7 (inert)
 *
 * NEXT, and it is unspent: make the two stores ALIAS. Batch 262 established that
 * only TAG IDENTITY separates alias sets -- one aggregate carrying both an int
 * and a short member should stop the halfword store being hoisted above the word
 * store. The note's levers are all about the register rotation and none of them
 * touch this.
 *
 * --- prior note follows, unchanged ---
 * Func_80198dc (ClearCallbackTable) -- asm/rom_15000/rom_1908c_c_a_c_c_b.s
 *
 * NOTE: this function was SPLIT OUT of a seven-function file this round, so it
 * now has its own .s and is individually elevatable. The split is byte-neutral
 * and make compare is green.
 *
 * BLOCKER: the allocation is shifted one register down, whole. 18 lines against
 * 18, 12 differing, and every difference is the same rotation:
 *
 *      value            rom   ours
 *      iwram base        r3    r2
 *      offset            r4    r3
 *      halfword pointer  r2    r1
 *
 * The ROM starts allocating at r3 and uses r4 (which is call-used here under
 * -fcall-used-r4, so it is free); we start at r2.
 *
 * TRIED AND REJECTED, all measured: the two pool loads in either order (12 and
 * 13 -- putting the offset first is what fixed the ORDER, and is kept below);
 * the counter and zero initialised before the pointer (12); their declarations
 * swapped (13); their assignments swapped (12); the second pointer built
 * DESTRUCTIVELY from the base (`b += off; p = (int *)b;`), which is what the
 * ROM's `add r3, r4` looks like (13).
 *
 * SETTLED: assigning `off = 0x12dc` BEFORE reading the global is what puts both
 * pool loads ahead of the dereference, matching the ROM. And 0x12bc is DERIVED
 * (`sub r4, #0x20`), not a second constant -- the derive-the-offset lever, with
 * the first offset added to a pointer loaded from memory.
 */
extern unsigned char *iwram_3001e8c;

void Func_80198dc(void)
{
    unsigned char *b;
    int *p;
    short *q;
    int off;
    int i;
    int z;

    off = 0x12dc;
    b = iwram_3001e8c;
    q = (short *)(b + off);
    off -= 0x20;
    i = 0;
    z = 0;
    p = (int *)(b + off);
    do {
        i++;
        *p++ = z;
        *q = z;
        q++;
    } while (i != 8);
}
