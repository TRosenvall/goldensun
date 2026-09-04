// fakematch
/* OvlFunc_903_20084f4  --  0x020084f4
 *
 * Cut out of goldensun/asm/overlays/rom_798dc4/ovl_314_c_a_c_a.s.
 *
 * Post-cutscene fixup: sets a flag, re-poses several actors, and queues a
 * dialogue line for whichever of four save flags is set. Returns 0.
 *
 * WORKED THE NEW WAY ROUND -- natural form first, levers only where the
 * residue pointed. That order matters after OvlFunc_948_2008ccc, where a pin
 * added by habit was the only thing keeping the function from matching. Here
 * the plain spelling opened at 115 of 164 and each fix was aimed at a diff
 * that had already been read:
 *
 *     plain                                        115 of 164, one SHORT
 *     + orr destinations, stack pairs, one SetPos   108
 *     + the rematerialised constant 2               17, length exact
 *     + the two later SetPos fills and a named zero  3
 *     + the last stack pair                          MATCH
 *
 * THE CONSTANT 2 IS REMATERIALISED, and finding it was worth more than the
 * other four put together -- it moved the count from 108 to 17 and fixed the
 * length. `*p = 2` is written at two different actor offsets in two different
 * blocks; gcc hoists the 2 into r5, a callee-saved register, and feeds both
 * stores from it, while the ROM issues `mov r3, #2` at each. Assigning it
 * through a pin on r3 -- call-clobbered, so it cannot survive a `bl` -- forces
 * the rebuild. This is the batch-193 lever, and it is now the third function
 * where the whole residue was one hoisted constant.
 *
 * TWO `orr` SITES WANT THE CONSTANT AS DESTINATION and one wants the value:
 *
 *     rom, at +0x59   ldrb r2 / mov r3, #0x10 / orr r3, r2     constant in r3
 *     rom, at +0x23   ldrb r3 / mov r2, #0x2  / orr r3, r2     value in r3
 *
 * `*p = *p | 2` gives the second directly. The first needs a pin, because
 * `*p = 0x10 | *p` and `*p |= 0x10` are byte-identical to each other and both
 * give the value-as-destination form -- exactly as measured in
 * src/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_a_c_c_a_b.c. Source order does not
 * choose which operand ends up in the destination register; a pin does.
 *
 * A STACK ARGUMENT PAIR NEEDS BOTH VALUES LIVE AT ONCE. The ROM builds both
 * before storing either -- `mov r3 / mov r2 / str r3, [sp] / str r2, [sp, #4]`
 * -- where gcc reuses one register for both, `mov r3 / str / mov r3 / str`.
 * Same instruction count, different registers. Two named locals assigned
 * before the call restore it, at three separate sites.
 *
 * TEARDOWN, AND IT CAUGHT ONE. Each lever was removed from the finished file:
 *
 *     the pin on the two `*p = 2` stores       108 differing without it
 *     the pin on the two orr sites               4 differing without it
 *     the second SetPos argument fill            3 differing without it
 *     the named zero at +0x55                    2 differing without it
 *     the FIRST SetPos argument fill             STILL MATCHES  -- removed
 *
 * That last one was three pinned registers written from habit because the site
 * looked like the others. It is not in this file. The teardown is cheap and it
 * is the only thing that separates a lever from a superstition.
 */

extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __Actor_SetSpriteFlags(unsigned char *a, int n);
extern void __CutsceneWait(int n);
extern void __Func_8091ff0(int n);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

int OvlFunc_903_20084f4(void)
{
    unsigned char *p;
    int k;
    int e;
    int g;
    int z;
    register int q3 __asm__("r3");

    __SetFlag(0xa2 << 1);
    __CutsceneWait(0xa);
    __Func_8091ff0(0xaa);
    __MapActor_SetAnim(0xb, 2);
    p = __MapActor_GetActor(0xb) + 0x23;
    q3 = 2;
    *p = q3;
    p = __MapActor_GetActor(8) + 0x59;
    q3 = 0x10;
    *p = q3 | *p;
    p = __MapActor_GetActor(0xf) + 0x59;
    q3 = 8;
    *p = q3 | *p;
    if (__GetFlag(0x865) != 0) {
        e = 0x49;
        g = 0xb;
        __Func_8010704(0x4a, 0xb, 1, 1, e, g);
    }
    if (__GetFlag(0x86 << 4) != 0) {
        __MapActor_SetPos(8, 0x88 << 16, 0xc4 << 16);
        p = __MapActor_GetActor(8) + 0x23;
        *p = *p | 2;
        __MapActor_SetAnim(8, 2);
        k = 0xc;
        __Func_8010704(0x27, 0xc, 3, 1, 8, k);
        __Func_8010704(0x2b, 0xb, 3, 1, k, 0xb);
    }
    if (__GetFlag(0x861) != 0) {
        {
            register int p0 __asm__("r0");
            register int p1 __asm__("r1");
            register int p2 __asm__("r2");
            p1 = 0x84; p2 = 0x9c; p0 = 9; p1 <<= 17; p2 <<= 17;
            __MapActor_SetPos(p0, p1, p2);
        }
        e = 0x10;
        g = 0x12;
        __Func_8010704(0x30, 0x12, 1, 2, e, g);
    } else if (__GetFlag(0x862) != 0) {
        {
            register int p0 __asm__("r0");
            register int p1 __asm__("r1");
            register int p2 __asm__("r2");
            p1 = 0x8c; p2 = 0x9c; p0 = 9; p1 <<= 17; p2 <<= 17;
            __MapActor_SetPos(p0, p1, p2);
        }
        e = 0x10;
        g = 0x12;
        __Func_8010704(0x2f, 0x12, 1, 2, e, g);
    }
    if (__GetFlag(0x863) != 0) {
        __MapActor_SetPos(0xa, 0xbc << 17, 0x8c << 17);
        p = __MapActor_GetActor(0xa) + 0x23;
        q3 = 2;
        *p = q3;
        z = 0;
        p = __MapActor_GetActor(0xa) + 0x55;
        *p = z;
        __Actor_SetSpriteFlags(__MapActor_GetActor(0xa), 0);
        e = 0x17;
        g = 0x11;
        __Func_8010704(0x36, 0x11, 1, 1, e, g);
    }
    return 0;
}
