// fakematch
/* OvlFunc_968_2009150  --  0x02009150
 *
 * Cut out of goldensun/asm/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_a_c_c_a.s.
 *
 * Actor 0 runs a behaviour script, then is walked to one of two spots depending
 * on where it ended up, emotes, and animates.
 *
 * PARKED AT 14 OF 81, AND ELEVEN OF THOSE FOURTEEN WERE THE BUILD FLAGS. This
 * TU is caught by the mis-scoped `rom_7f2f14/ovl_30_c_a_c_a_c_a%` wildcard,
 * which applies O1_CFLAGS. The SAME unchanged parked candidate screens at 6 of
 * 81 once the default flags are used. It now carries an explicit -O2 rule, the
 * fourth function in this overlay to need one.
 *
 * THE PARK WAS NOT WRONG ABOUT ITS EVIDENCE -- it measured 14 honestly. It was
 * reading a residue produced by a compiler invocation the build never makes.
 * Two of the three real problems below were INVISIBLE at -O1, so no amount of
 * spelling work at the inherited flag could have found them.
 *
 * The remaining six were three ordinary transpositions, all closed by levers
 * already on file:
 *
 *   1. A STORE INTERLEAVED INTO ARGUMENT SETUP, twice. The ROM does
 *
 *          mov r1, #0x80 / lsl r1, #11 / mov r2, #0x80
 *          str r1, [r5, #0x28]                 <-- the store, mid-setup
 *          mov r0, #0 / lsl r2, #10
 *          bl __MapActor_SetSpeed
 *
 *      The stored value and the second argument are THE SAME VALUE, so the
 *      store has to sit between the halves of the argument fill. Pinning
 *      r0-r2 and writing the store between the assignments reproduces it. The
 *      __MapActor_Emote site is the same shape with the function-pointer store.
 *
 *   2. THE CONSTANT IS THE `orr` DESTINATION. The ROM has
 *
 *          ldrb r2, [r0] / mov r3, #1 / orr r3, r2 / strb r3, [r0]
 *
 *      loading into r2 and building the constant in r3. Both `*p |= 1` and
 *      `*p = 1 | *p` give the roles the other way round and are byte-identical
 *      to each other at 2 differing. Pinning ONE register -- r3, the constant
 *      and the orr destination -- and writing `*p = q3 | *p` matches.
 *
 * TORN DOWN, as the discipline requires. The first form that matched pinned
 * BOTH r2 and r3 around the read-modify-write. Removing the r2 pin changes
 * nothing, so it is not in this file. One pin on the destination is the whole
 * lever there.
 */

extern unsigned char *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __MapActor_SetBehavior(int slot, void *script);
extern void __MapActor_WaitScript(int slot);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_Emote(int slot, int a, int b);
extern void __MapActor_DoAnim(int slot, int n);
extern void __Func_8092950(int slot, int n);
extern void __Func_80921c4(int slot, int a, int b);
extern unsigned char gScript_968__0200d21c[];
extern void OvlFunc_968_20085e4(void);

void OvlFunc_968_2009150(void)
{
    unsigned char *a;
    unsigned char *p;
    int k;
    void *fp;
    register int p0 __asm__("r0");
    register int p1 __asm__("r1");
    register int p2 __asm__("r2");

    a = __MapActor_GetActor(0);
    __CutsceneStart();
    __MapActor_SetBehavior(0, gScript_968__0200d21c);
    __MapActor_WaitScript(0);
    __Func_8092950(0, 6);
    p1 = 0x80;
    p1 <<= 11;
    p2 = 0x80;
    *(int *)(a + 0x28) = p1;
    p0 = 0;
    p2 <<= 10;
    __MapActor_SetSpeed(p0, p1, p2);
    if (*(int *)(a + 0x10) >> 20 <= 0x36) {
        p = __MapActor_GetActor(0) + 0x5a;
        *p &= 0xfe;
        k = 0xd2;
    } else {
        p = __MapActor_GetActor(0) + 0x5a;
        *p &= 0xfe;
        k = 0xee;
    }
    __Func_80921c4(0, *(short *)(a + 0xa), k << 2);
    __CutsceneWait(1);
    p = __MapActor_GetActor(0) + 0x5a;
    {
        register int q3 __asm__("r3");
        q3 = 1;
        *p = q3 | *p;
    }
    __CutsceneWait(0x14);
    fp = (void *)OvlFunc_968_20085e4;
    p1 = 0x81;
    *(void **)(a + 0x6c) = fp;
    p2 = 0x3c;
    p0 = 0;
    p1 <<= 1;
    __MapActor_Emote(p0, p1, p2);
    __MapActor_DoAnim(0, 4);
    __Func_8092950(0, 0);
    __MapActor_DoAnim(0, 4);
    *(void **)(a + 0x6c) = 0;
    __CutsceneEnd();
}
