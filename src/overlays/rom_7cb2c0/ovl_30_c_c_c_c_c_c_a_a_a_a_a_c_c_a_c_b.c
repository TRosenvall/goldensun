/* OvlFunc_945_200aff0 -- 239 instructions, 884 bytes, 309 encodings and 125
 * relocations identical, WITH ZERO PINS.  Split out of
 * asm/overlays/rom_7cb2c0/ovl_30_c_c_c_c_c_c_a_a_a_a_a_c_c_a_c.s (3 functions); its
 * two jump tables sit inside the function body and travel with it.  The split was
 * verified byte-neutral before this .c was written.
 *
 * THIS IS THE TARGET BATCH 280 ASSIGNED AND NEVER OPENED.  It was never hard --
 * 263 of 309 -> 13 -> 0 on two structural levers and no scaffolding at all.  Four
 * pins were tried at the __Func_8092950 sites and are individually AND JOINTLY
 * droppable; the correct pin count is zero.
 *
 * TWO LEVERS ONLY:
 *
 * 1. TWO `gState` BASE VARIABLES, ONE PER SWITCH.  The ROM holds the base in r1 --
 *    A CALL-CLOBBERED REGISTER -- and RELOADS it after the __Func_8092950 call.
 *    One base variable puts it in r5 and deletes the reload.  Reading a base held
 *    in a call-clobbered register as evidence of a per-region variable is the
 *    generalisable part.
 * 2. THE `L7f84 = 0` STORE ROUTED THROUGH AN `int` -- { int z = 0; L7f84 = z; } --
 *    which puts the value in r2 and the address in r3, matching `str r2, [r3]`.
 *
 * ================================================================
 * A NEW tryc BLIND SPOT, AND IT IS THE INVERSE OF BATCH 280'S
 * ================================================================
 *
 * tryc reports THIS BYTE-IDENTICAL OBJECT as "rom 330 lines, ours 331, 230 differ".
 * Every one of the 230 is tryc's OWN LABEL RENUMBERING (`bne L7` against `bne L8`):
 * the two sides emit the 54 jump-table words with different internal label orders,
 * so tryc's positional label map diverges and never re-synchronises.
 *
 * objcmp cuts the function out through .func_end -- jump tables included -- and
 * compares all 309 encodings (branch offsets are IN the encodings) and all 125
 * relocations, 54 of them the table words.  objcmp is right.
 *
 * **tryc's COUNT IS NOISE ON ANY FUNCTION CONTAINING A JUMP TABLE.**  Batch 280's
 * lesson was that a normalised count can UNDERSTATE the distance when the pool has
 * moved; this is the same tool OVERSTATING it to the point of reporting a perfect
 * match as 230 differences.  Neither direction is a distance.  Check for
 * `mov pc, rN` or a run of `.word .L` before trusting any tryc number.
 *
 * No per-file Makefile flag override exists for this stem.
 */
extern int L7f84 __asm__(".L7f84");
extern unsigned char *iwram_3001ebc;
typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern unsigned char gScript_945__0200e8e4[];

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __WaitFrames(int n);
extern void __SetFlag(int id);
extern int __GetFlag(int id);
extern int __StartTask(void (*fn)(void), int n);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_SetBehavior(int slot, unsigned char *s);
extern void __Func_8091e9c(int a);
extern void __Func_8092950(int a, int b);
extern void OvlFunc_945_200b364(void);
extern void OvlFunc_945_200b51c(void);
extern void OvlFunc_945_200b66c(void);
extern void OvlFunc_945_200bd10(void);
extern void OvlFunc_945_200bdec(void);
extern void OvlFunc_945_200be34(void);
extern void OvlFunc_945_200beec(void);
extern void OvlFunc_945_200bf94(void);
extern void OvlFunc_945_200c0e8(void);
extern void OvlFunc_945_200c13c(void);
extern void OvlFunc_945_200c198(void);
extern void OvlFunc_945_200c218(void);
extern void OvlFunc_945_200c8e8(int a, int b, int c);
extern void OvlFunc_945_200d068(void);
extern void OvlFunc_945_200d684(void);
extern void OvlFunc_945_200d6dc(void);
extern void OvlFunc_945_200d780(void);
extern void OvlFunc_945_200d7ec(void);
extern void OvlFunc_945_200dc48(void);
extern void OvlFunc_945_200dca4(void);
extern void OvlFunc_945_200dd10(void);
extern void OvlFunc_945_200e110(void);

int OvlFunc_945_200aff0(void)
{
    unsigned int g, h;
    unsigned char *s;

    __WaitFrames(0x1);
    __SetFlag(0xa2 << 1);
    *(int *)(iwram_3001ebc + (0xe0 << 1)) = 0x209;
    g = (unsigned int)&gState;
    switch (*(short *)(g + (0xe1 << 1))) {
    case 1:
    case 2:
    case 11:
        if (__GetFlag(0x93e) == 0 && __GetFlag(0x928) != 0)
            __Func_8092950(0x9, 0x2);
        else if (__GetFlag(0x911) != 0)
            __Func_8092950(0xc, 0x2);
        break;
    case 4:
    case 12:
    case 15:
    case 16:
    case 17:
    case 18:
    case 19:
    case 20:
    case 21:
    case 23:
    case 24:
        __Func_8092950(0x13, 0x2);
        break;
    case 5:
        if (__GetFlag(0x93e) == 0 && __GetFlag(0x911) != 0)
            __Func_8092950(0xd, 0x2);
        break;
    }
    h = (unsigned int)&gState;
    switch (*(short *)(h + (0xe1 << 1))) {
    case 1:
    case 2:
        OvlFunc_945_200b51c();
        break;
    case 4:
        OvlFunc_945_200b66c();
        break;
    case 5:
        OvlFunc_945_200b364();
        break;
    case 10:
        if (__GetFlag(0x928) != 0)
            OvlFunc_945_200bdec();
        else
            OvlFunc_945_200bd10();
        break;
    case 11:
        if (__GetFlag(0x928) != 0)
            OvlFunc_945_200beec();
        else
            OvlFunc_945_200be34();
        break;
    case 12:
        OvlFunc_945_200bf94();
        break;
    case 13:
        OvlFunc_945_200c0e8();
        break;
    case 14:
        OvlFunc_945_200c13c();
        break;
    case 15:
        if (__GetFlag(0x109) != 0) {
            __CutsceneStart();
            OvlFunc_945_200c8e8(0x19, 0x1, 0);
            OvlFunc_945_200c8e8(0x16, 0, 0);
            s = gScript_945__0200e8e4;
            __MapActor_SetBehavior(0x24, s);
            __MapActor_SetBehavior(0x25, s);
            __MapActor_SetBehavior(0x26, s);
            __Func_8092950(0x24, 0x3);
            __Func_8092950(0x25, 0x3);
            __Func_8092950(0x26, 0x3);
            __CutsceneEnd();
        } else {
            OvlFunc_945_200c198();
        }
        break;
    case 16:
        OvlFunc_945_200c218();
        break;
    case 17:
        if (__GetFlag(0x109) != 0) {
            __CutsceneStart();
            OvlFunc_945_200c8e8(0x19, 0x2, 0);
            OvlFunc_945_200c8e8(0x16, 0, 0);
            s = gScript_945__0200e8e4;
            __MapActor_SetBehavior(0x24, s);
            __MapActor_SetBehavior(0x25, s);
            __CutsceneEnd();
        } else {
            OvlFunc_945_200d068();
        }
        break;
    case 18:
        OvlFunc_945_200d684();
        break;
    case 19:
        if (__GetFlag(0x109) != 0) {
            __CutsceneStart();
            OvlFunc_945_200c8e8(0x19, 0x3, 0);
            OvlFunc_945_200c8e8(0x16, 0, 0);
            s = gScript_945__0200e8e4;
            __MapActor_SetBehavior(0x24, s);
            __MapActor_SetBehavior(0x25, s);
            __Func_8092950(0x24, 0x3);
            __Func_8092950(0x25, 0x3);
            __CutsceneEnd();
        } else {
            OvlFunc_945_200d6dc();
        }
        break;
    case 20:
        OvlFunc_945_200d780();
        break;
    case 21:
        if (__GetFlag(0x109) != 0) {
            if (__GetFlag(0x302) != 0) {
                { int z = 0; L7f84 = z; }
                __StartTask(OvlFunc_945_200dc48, 0xc8 << 4);
                __MapActor_SetAnim(0x9, 0x5);
            }
        } else {
            OvlFunc_945_200d7ec();
        }
        break;
    case 22:
        OvlFunc_945_200dca4();
        break;
    case 23:
        if (__GetFlag(0x109) == 0)
            OvlFunc_945_200dd10();
        break;
    case 24:
        OvlFunc_945_200e110();
        break;
    case 30:
        OvlFunc_945_200c8e8(0x14, 0x926, 0x92b);
        OvlFunc_945_200c8e8(0x15, 0, 0);
        __SetFlag(0x902);
        __Func_8091e9c(0x1);
        break;
    }
    return 0;
}
