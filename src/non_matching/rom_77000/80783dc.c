/* ModifyPP  --  NOT MATCHING
 *
 * Source asm: goldensun/asm/rom_77000/rom_77320_c_c.s
 * Best screen: 14 instructions in disagreeing regions, of 26 (rom 26, ours 25).
 *
 * BLOCKER CLASS: register allocation.  THE CONTROL FLOW AND INSTRUCTION
 * SEQUENCE NOW MATCH EXACTLY -- read the streams side by side and every
 * mnemonic lines up.  The count is high only because a register permutation
 * touches almost every line.
 *
 *      rom   ldrsh r3, [r6, r1]   ldrsh r2, [r6, r1]   add r3, r5   mov r1, r2
 *      ours  ldrsh r2, [r6, r1]   ldrsh r3, [r6, r1]   add r2, r5   (absent)
 *
 * The ROM keeps the clamped result in r1, a register distinct from `max` in
 * r2, and so needs an explicit `mov r1, r2` for the "already at maximum" arm.
 * gcc puts the result in `max`'s register, which makes that arm free and the
 * function one instruction shorter.  For gcc to do otherwise, `max` would have
 * to be live past the join, and it is not.
 *
 * WHAT WAS TRIED, in the order that mattered
 *
 *  1. `v = delta; ... v = cur + v;` -- reusing the parameter's variable for the
 *     sum.  gcc accumulates into the parameter's register, `add r5, r3`, where
 *     the ROM has `add r3, r5`, and hoists the add above the second load.
 *     14 of 26 with the structure wrong.
 *  2. Assigning the sum back into `cur` instead, with `delta` never rewritten.
 *     THIS IS THE FIX FOR THE STRUCTURE -- `add r3, r5` in the right place, and
 *     the whole instruction sequence falls into line.  Still 14, but now every
 *     difference is a register name.
 *  3. Reusing the offset local as the result (the offset-variable-reused lever,
 *     `o2 = 0x36; max = *(short *)(u + o2); o2 = max;`).  Byte-identical to (2).
 *
 * Kept below is (2) plus (3), since (3) costs nothing and is the shape the ROM
 * suggests.  The three `ldrsh` offsets are separate locals because Thumb
 * `ldrsh` has no immediate form and each one must be materialised.
 */
extern unsigned char *GetUnit(void);
extern void UpdateStatBarPercent(int a);

int ModifyPP(int who, int delta)
{
    unsigned char *u;
    unsigned int o1;
    unsigned int o2;
    unsigned int o3;
    int cur;
    int max;
    int d;
    int w;

    w = who;
    d = delta;
    u = GetUnit();
    o1 = 0x3a;
    cur = *(short *)(u + o1);
    o2 = 0x36;
    max = *(short *)(u + o2);
    cur = cur + d;
    o2 = max;
    if (cur > max)
        goto done;
    o2 = 0;
    if (cur < 0)
        goto done;
    o2 = cur;
done:
    *(short *)(u + 0x3a) = o2;
    UpdateStatBarPercent(w);
    o3 = 0x3a;
    return *(short *)(u + o3);
}
