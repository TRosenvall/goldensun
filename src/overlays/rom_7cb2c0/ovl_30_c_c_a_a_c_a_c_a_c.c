/* Cluster OvlFunc_945_2008cc8..OvlFunc_945_2008cc8, the whole of
 * goldensun/asm/overlays/rom_7cb2c0/ovl_30_c_c_a_a_c_a_c_a_c.s -- a single-function TU after
 * OvlFunc_945_2008b84 was split out of the original, so no further split was
 * needed and overlay.ld is untouched.
 *
 * Total .text for this TU = 332 bytes (= 0x014c).
 *
 * BUILT WITH CSE_CFLAGS, for the same reason as its sibling _b: the flag 0x300
 * is read at the top and set near the bottom, and at -O2 gcc commons the two
 * builds into r5 before the first call -- 76 of 121 lines.
 *
 * __Func_8092c40 IS DELIBERATELY LEFT UNDECLARED, as in the sibling.
 *
 * EXACT ON THE FIRST SCREEN once those two were applied, which is the point
 * worth recording: the sibling's levers transferred unchanged.  That is not
 * automatic -- OvlFunc_955_2009424 is a near-twin of an elevated function whose
 * spelling did NOT transfer, because its register budget differed.  Here the two
 * functions have the same shape AND the same number of live values, so they
 * behave the same.  Check the second before assuming the first.
 */
extern char *iwram_3001ebc;

extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __MessageID(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_TravelTo(int slot, int x, int y);
extern void __MapActor_WaitMovement(int slot);
extern void __Func_80925cc(int a, int b);
extern void __Func_8093040(int a, int b, int c);
extern int __Func_8091c7c(int a, int b);
extern int OvlFunc_945_20092dc(void);
extern void OvlFunc_945_2009190(void);
extern void OvlFunc_945_200c86c(int n);

void OvlFunc_945_2008cc8(void)
{
    int s;
    unsigned char *p;
    char *w;

    __CutsceneStart();
    if (__GetFlag(0xc0 << 2)) {
        s = OvlFunc_945_20092dc();
        OvlFunc_945_2009190();
        __MessageID(0x1ea2);
        OvlFunc_945_200c86c(9);
        __MapActor_SetAnim(s, 2);
        p = __MapActor_GetActor(0);
        if (p != 0)
            __MapActor_TravelTo(s, *(short *)(p + 0xa), *(short *)(p + 0x12));
        __MapActor_WaitMovement(s);
        __MapActor_SetPos(s, 0, 0);
    } else {
        __MessageID(0x1e84);
        __Func_8093040(9, 0, 0x3c);
        __Func_80925cc(9, 1);
        __Func_8092c40(9, 0);
        if (__Func_8091c7c(0, 0) == 0) {
            OvlFunc_945_200c86c(9);
            __MapActor_SetAnim(9, 2);
            p = __MapActor_GetActor(0);
            if (p != 0)
                __MapActor_TravelTo(9, *(short *)(p + 0xa), *(short *)(p + 0x12));
            __MapActor_WaitMovement(9);
            __MapActor_SetPos(9, 0, 0);
            __SetFlag(0xc0 << 2);
            if (__GetFlag(0x92b))
                __SetFlag(0x991);
            else if (__GetFlag(0x92a))
                __SetFlag(0x918);
            else if (__GetFlag(0x929))
                __SetFlag(0x936);
            else
                __SetFlag(0x92d);
        } else {
            w = iwram_3001ebc;
            *(unsigned short *)(w + (0xec << 1)) += 1;
            OvlFunc_945_200c86c(9);
        }
    }
    __CutsceneEnd();
}
