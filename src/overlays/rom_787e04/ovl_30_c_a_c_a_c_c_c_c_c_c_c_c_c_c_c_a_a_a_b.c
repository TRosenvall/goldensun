/* OvlFunc_887_20083f8 -- 143 instructions, 384 bytes, 151 encodings and 35
 * relocations identical.  Split out of
 * asm/overlays/rom_787e04/ovl_30_c_a_c_a_c_c_c_c_c_c_c_c_c_c_c_a_a_a.s (2
 * functions); its file-mate OvlFunc_887_2008578 stays in asm, parked at 4 of 453.
 * The split was verified byte-neutral before this .c was written.
 *
 * THE HImode `int`-CARRIER RULE NEEDS A THIRD NAME, AND THE ORDER OF THE TWO IS
 * LOAD-BEARING.  docs/elevation.md records that the `int` intermediate often needs a
 * SECOND name, the destination pointer.  Measured here, where the ROM has
 * `mov r2,#1 / strh r2,[r3]`:
 *
 *     *(short *)(p + 0x1f84) = 1;                  ldrh (pooled HImode 1)   1 of 153
 *     int one = 1; ... = one;                      mov, but r2->r1 globally 16 of 153
 *     { short *pp = ...; int one = 1; *pp = one; } EXACT
 *
 * THE POINTER MUST BE NAMED *AND ASSIGNED BEFORE* THE CARRIER.  Carrier-first
 * reorders the offset scratch register through the whole function and drags 0x209
 * into the pool as well.  All four range-test spellings of the surrounding `if` were
 * inert; this one line was the whole function.
 *
 * `-6` IN HImode POOLS AS 0xfffa AND THEN POISONS THE COMPARE CONSTANT.
 * `unsigned short u; (unsigned short)(u - 6) <= 1` gives
 * `ldr r2,=0xfffa / add r3,r2 / lsl r3,#16 / add r2,#6 / cmp r3,r2` -- gcc pools
 * 0xfffa BECAUSE 0xfffa + 6 == 0x10000, the compare constant.  The ROM wants
 * `sub r3,#6` plus `mov r2,#0x80 / lsl r2,#9`.  DOING THE SUBTRACTION IN SImode
 * (`int t = *(unsigned short *)b; if ((unsigned short)(t - 6) <= 1)`) fixes both at
 * once: 4 of 153 -> 1.  Four HImode spellings of the range test were bit-identical
 * to each other; THE MODE OF THE SUBTRACTION IS THE ONLY AXIS.
 *
 * No per-file Makefile flag override applies to this stem.
 */
extern unsigned char *iwram_3001ebc;
typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;

extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __ClearFlag(int id);
extern void __WaitFrames(int n);
extern void __StartThunder(void);
extern void __MapTransitionIn(void);
extern void __WaitMapTransition(void);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __Actor_SetSpriteFlags(unsigned char *a, int f);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __Func_80118a8(int n);
extern void __Func_8092b08(int a, int b);
extern void __Func_800fe9c(void);
extern void __Func_8095240(void);
extern void __Func_8095268(void);
extern void OvlFunc_887_20093b4(void);
extern void OvlFunc_887_20093e4(void);
extern void OvlFunc_887_2008a0c(void);
extern void OvlFunc_887_2008578(void);

int OvlFunc_887_20083f8(void)
{
    short s;
    int t;
    unsigned int b, o;

    b = (unsigned int)&gState;
    o = 0xe1;
    o <<= 1;
    b += o;
    s = *(short *)b;
    if (s == 0x13) {
        __ClearFlag(0x12f);
        *(int *)(iwram_3001ebc + (0xe0 << 1)) = 0x209;
        return 0;
    }
    if (__GetFlag(0x834) != 0) {
        __MapActor_SetPos(0xb, 0, 0);
        __MapActor_SetPos(0xc, 0, 0);
        __MapActor_SetPos(0xd, 0, 0);
        __MapActor_SetPos(0xe, 0, 0);
        __MapActor_SetPos(0xf, 0, 0);
        __MapActor_SetPos(0x10, 0, 0);
    } else {
        OvlFunc_887_20093b4();
    }
    __Func_8092b08(0xd, 1);
    if (__GetFlag(0x87a) != 0) {
        __Actor_SetSpriteFlags(__MapActor_GetActor(0x11), 0);
        b = (unsigned int)&gState;
        o = 0xe1;
        o <<= 1;
        b += o;
        t = *(unsigned short *)b;
        if ((unsigned short)(t - 6) <= 1) {
            if (__GetFlag(0x109) != 0) {
                if (__GetFlag(0x203) != 0)
                    __Func_80118a8(0xc);
            } else {
                __Func_80118a8(0xb);
                __Actor_SetSpriteFlags(__MapActor_GetActor(8), 0);
                __MapActor_SetAnim(8, 0xa);
            }
        }
    } else {
        b = (unsigned int)&gState;
        o = 0xe1;
        o <<= 1;
        b += o;
        s = *(short *)b;
        if (s == 0x15) {
            OvlFunc_887_2008a0c();
        } else if (s == 0x14) {
            __SetFlag(0x834);
            OvlFunc_887_2008578();
        } else if (s == 0x16) {
            OvlFunc_887_20093e4();
        } else {
            *(int *)(iwram_3001ebc + (0xe0 << 1)) = 0x209;
            if (__GetFlag(0x834) != 0) {
                __StartThunder();
                { short *pp = (short *)((&iwram_3001ebc)[3] + 0x1f84);
                  int one = 1;
                  *pp = one; }
                __Func_8095240();
                __WaitFrames(0x1e);
                __MapTransitionIn();
                __WaitMapTransition();
                __Func_8095268();
            } else {
                __Func_800fe9c();
                __WaitFrames(1);
            }
        }
    }
    return 0;
}
