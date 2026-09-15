/*
 * ### BATCH 265 -- ADDITIONS. The prior note below is intact and still correct.
 *
 * Its "TRIED AND REJECTED: an explicit `q = p + off;` pointer local before each
 * store" is confirmed -- I re-derived it independently and it is still 4 bytes
 * short. What the note does not record is that the named-pointer forms DO fix
 * the FIRST store:
 *
 *     one named `short *a` recomputed between the stores   13 enc vs 13
 *     two named pointers `a` and `b`                       13 enc vs 13
 *     `unsigned char *a` with the cast at the store        13 enc vs 13
 *
 * Those reach the ROM's INSTRUCTION COUNT -- the first store becomes
 * `add`+`strh [reg]` -- but gcc still takes the register-offset form for the
 * SECOND store, and the register assignment then cascades so all 13 differ. So
 * the lever is half-working rather than inert, which is a better starting point
 * than the note implies.
 *
 * NEXT, and it is unspent: batch 261 established that the `[sp,#0]` fold is done
 * by pass 09 cse2 substituting a known base for a BARE-REG address, and that
 * `volatile` ON THE OBJECT defeats it. The register-offset fold here is the same
 * family -- an address gcc can re-form -- and neither `volatile` nor a pin has
 * been tried on this function.
 *
 * --- prior note follows, unchanged ---
 * Func_8019d0c (InitMenuLayer) -- asm/rom_15000/rom_1908c_c_c_b_b.s
 * SPLIT OUT this round; byte-neutral, verified.
 *
 * BLOCKER: register-offset stores where the ROM computes explicit addresses.
 * 8 lines against 10 -- TWO SHORT.
 *
 *      rom   add r1, r3, r0 / strh r2, [r1] / add r0, #2 / add r1, r3, r0 / strh r2, [r1]
 *      ours  strh r2, [r1, r3] / add r3, #2 / strh r2, [r1, r3]
 *
 * The ROM builds each address into a register; we use register-offset stores,
 * which is two instructions cheaper.
 *
 * TRIED AND REJECTED: an explicit `q = p + off;` pointer local before each
 * store, which is the lever that fixed exactly this in SetTextColor above --
 * still 8 lines here. An `unsigned short *` destination and a `short` value
 * local are both byte-identical.
 *
 * SETTLED: the second offset is DERIVED (`add r0, #2`), not a second constant,
 * and 0x3e7 is a word pool load in both.
 */
extern unsigned char *iwram_3001e8c;

void Func_8019d0c(void)
{
    unsigned char *p;
    int off;
    int v;
    unsigned char *q;

    p = iwram_3001e8c;
    off = 0x12ec;
    v = 0x3e7;
    q = p + off;
    *(short *)q = v;
    off += 2;
    q = p + off;
    *(short *)q = v;
}
