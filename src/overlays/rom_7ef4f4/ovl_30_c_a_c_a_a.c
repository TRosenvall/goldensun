/* OvlFunc_965_200a6fc
 *
 * Cut out of goldensun/asm/overlays/rom_7ef4f4/ovl_30_c_a_c_a_a.s, which holds
 * this function and nothing else.
 *
 * An absolute-difference proximity guard: reads field 0xc from actor slot 0 and
 * from whatever OvlFunc_965_200a660 returns, and runs OvlFunc_965_20080c4 only
 * when the two are within 0x80000 of each other.  The ROM tests the two signs
 * in separate blocks, each materialising 0x80 << 12 for itself.
 *
 * This was parked at 2 of 29 on "branch polarity, inner test of a two-arm range
 * check".  The fix is NOT a spelling of the condition -- it is which statement
 * is the inner THEN arm.  Park attempts all made the inner then arm the
 * `return`; writing the CALL as the then arm (`if (d < K) goto call;`) flips the
 * fall-through and gcc emits the ROM's `bge <epilogue> / b <call>`.
 */
extern void *__MapActor_GetActor(int slot);
extern void *OvlFunc_965_200a660(void);
extern void OvlFunc_965_20080c4(void);

void OvlFunc_965_200a6fc(void)
{
    unsigned char *s;
    unsigned char *p;
    int a;
    int b;
    int d;

    s = (unsigned char *)__MapActor_GetActor(0);
    p = (unsigned char *)OvlFunc_965_200a660();
    if (p == 0)
        return;
    b = *(int *)(p + 0xc);
    a = *(int *)(s + 0xc);
    d = b - a;
    if (d >= 0) {
        if (d < (0x80 << 12))
            goto call;
    } else {
        if (a - b < (0x80 << 12))
            goto call;
    }
    return;
call:
    OvlFunc_965_20080c4();
}
