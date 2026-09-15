/* NewActor  --  0x0800c0cc   *** PARKED at 2 of 19 ***
 *
 * Source asm: goldensun/asm/rom_9000/rom_c004_c_a_a_a_a_a_a_a.s  (ONE function,
 * so it converts WHOLE when it closes -- no split, stage1.ld:132 verbatim.)
 * Previously UNATTEMPTED; this is a first attempt, not a re-park.
 *
 *     XX ENCODINGS differ in 2 place(s) (ref 19, ours 19)
 *        [3] ref 6813 (ldr r3,[r2])   ours 2000 (mov r0,#0)
 *        [4] ref 2000                 ours 6813
 *
 * Two adjacent instructions, swapped: the first slot load must precede the
 * result's zero-init.  The source already writes them in that order.
 *
 * BLOCKER CLASS: a cheap constant with nothing to be ordered against -- the
 * same shape as src/non_matching/ovl_793768/2008e0c.c.  `mov r0, #0` costs
 * nothing (rtx_cost 0 below 256), so precompute_register_parameters never
 * hoists it and no pin has anything to rank it against.  Confirmed: pinning the
 * result to r0 is INERT, and so is pinning it alongside either other variable.
 *
 * WHAT GOT IT FROM 16 TO 2, both worth keeping:
 *
 *  1. `goto test;` INTO THE LOOP'S BOTTOM TEST reproduces the ROM's `b .Lc0e4`
 *     entry.  Written as a plain `while`, gcc peels the first test to the top
 *     and the whole body is displaced -- 16 of 19.  With the goto, 7.
 *     This is the recorded "goto into a block" lever applied to loop rotation
 *     rather than to arm order.
 *  2. PINNING EITHER LOOP VARIABLE fixes a register exchange, 7 -> 2.  The ROM
 *     holds the slot pointer in r2 and the loaded word in r3; we had them the
 *     other way.  `p` pinned to r2 and `v` pinned to r3 are individually
 *     sufficient and identical in output, which is the recorded "pass-ownership
 *     swap, either member will do" case.
 *
 * MEASURED, all at 19 encodings unless noted:
 *     plain while, no pin, single-return           16
 *     plain while + result variable + goto done    14
 *     result initialised before the load            14
 *     goto test (loop rotation), no pin              7
 *     + p pinned r2                                  2
 *     + v pinned r3                                  2
 *     + p and r pinned                               2
 *     + v and r pinned                               2
 *     + p, v and r all pinned                        2
 *     init order permutations (4)                    2 or 3
 *
 * NEXT: the unspent step is reading .23.sched2 with -fsched-verbose=5 to see
 * which of priority / class / dependent count / LUID separates the load from
 * the constant.  On 2008e0c the same shape turned out NOT to be sched2 at all
 * (-fno-schedule-insns2 was inert there); that flag has not been tried here.
 */
extern unsigned char *iwram_3001e64;

unsigned char *NewActor(void)
{
    unsigned char *r;
    int i, v;
    register unsigned char *p __asm__("r2");

    p = iwram_3001e64;
    v = *(int *)p;
    r = 0;
    i = 0;
    goto test;
    do {
        i++;
        p += 0x70;
        if (i > 0x3f)
            goto done;
        v = *(int *)p;
test:
        ;
    } while (v != 0);
    r = p;
done:
    return r;
}
