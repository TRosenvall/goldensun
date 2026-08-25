/* OvlFunc_964_2009458 -- NOT MATCHING. 3 of 36, same length.
 *
 * Source asm: goldensun/asm/overlays/rom_7ed0a0/ovl_30_a_c_c_a_a.s
 *
 * TWIN: OvlFunc_964_20094ac in the same .s is the same shape, so this park
 * covers two.
 *
 * Blocker: register allocation on the read-modify-write, and nothing else.
 *
 *     rom    ldrb r2, [r0] / mov r3, #0xf7 / and r3, r2
 *     ours   ldrb r3, [r0] / mov r2, #0xf7 / and r2, r3
 *
 * Same instructions, same order, same operand roles -- the mask is the
 * destination in both. Only which register holds which differs, and it
 * propagates to the `orr` and the shared `strb` at the join.
 *
 * TWO FIXES GOT IT FROM 13 OF 36 TO 3, and both are worth keeping:
 *
 *   THE +0x62 STORE IS A DESTRUCTIVE WALK. `p[0x62] = 0;` keeps p live and
 *   costs `mov r2, r0 / add r2, #0x62` -- 38 lines against 36. `p += 0x62;
 *   *p = 0;` gives the ROM's `add r0, #0x62 / strb`. The offset is past the
 *   5-bit `strb` immediate, so the add is forced either way; what varies is
 *   whether the base survives.
 *
 *   THE LOADED BYTE IS ITS OWN LOCAL. `t = *q; v = 0xf7 & t;` rather than
 *   `v = 0xf7 & *q;` -- 5 of 36 to 3.
 *
 * TRIED AND IDENTICAL: `t & 0xf7` instead of `0xf7 & t` (and `t | 8` for the
 * other arm). gcc picks the destination register itself; operand order in the
 * source does not reach it.
 *
 * NEXT: nothing at the expression level. This is the allocator choosing r2/r3
 * the other way round from Camelot's compiler on a three-value block.
 */
extern void __SetFlag(int id);
extern int __GetFlag(int id);
extern void *__MapActor_GetActor(int slot);

void OvlFunc_964_2009458(void)
{
    unsigned char *p;
    unsigned char *q;
    int v;
    int t;

    __SetFlag(0x80 << 2);
    if (__GetFlag(0x201)) {
        p = (unsigned char *)__MapActor_GetActor(0xe);
        p += 0x62;
        *p = 0;
        q = (unsigned char *)__MapActor_GetActor(0xe);
        q += 0x59;
        t = *q;
        v = 0xf7 & t;
    } else {
        p = (unsigned char *)__MapActor_GetActor(0xe);
        p += 0x62;
        *p = 1;
        q = (unsigned char *)__MapActor_GetActor(0xe);
        q += 0x59;
        t = *q;
        v = 8 | t;
    }
    *q = v;
}
