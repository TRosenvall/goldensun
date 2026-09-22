/* Cluster OvlFunc_944_2008240..OvlFunc_944_2008240 extracted from
 * goldensun/asm/overlays/rom_7ca63c/ovl_30_c_c_a_c.s.
 *
 * Total .text for this TU = 175 encodings. Never attempted before batch 279.
 * TWO PIN SITES -- one fakematch row. No flags. This file needs no split.
 *
 * `q0` ASSIGNED FIRST INSIDE PIN3 IS THE ARGUMENT-ORDER FIX, AND IT IS THE ROUND'S HIGHEST-YIELD
 * FINDING. The ROM writes `mov r1 / mov r2 / mov r0 / lsl r1 / lsl r2`; plain C AND `PIN3` with
 * `q0` assigned LAST both give `mov r1 / mov r2 / lsl r1 / lsl r2 / mov r0`.
 *
 * The unscheduled dumps say why: all five insns are ONE sched2 region, the two `mov`s that feed an
 * `lsl` carry priority 2 and the rest priority 1, so the priority-1 group falls to INSN_LUID --
 * and writing `q0 = N;` textually FIRST gives `mov r0` the lowest LUID in that group. That is the
 * same structural asymmetry recorded on OvlFunc_943_2009db0 this batch (a call SETS r0 and only
 * USES r1/r2, so r0-fills accumulate no anti-dependences), approached from the cheap side: where
 * the fills are in one region, ASSIGNMENT ORDER is enough and no barrier is needed.
 *
 * The other site, `__Func_80933f8(-1, -1, -1, 0)`, needs a PIN2 on r0/r1 because gcc builds -1
 * once and copies it twice where the ROM builds it three times. PIN3 and PIN4 also work; PIN2 is
 * the minimum and r0 alone explodes to 83 differing.
 *
 * A `GlobalState` STRUCT WITH REAL `short` FIELDS IS WHAT KEEPS TWO ADJACENT HALFWORD INDEX BUILDS
 * INDEPENDENT -- see the sibling ovl_30_c_c_a_c_c_c_a_b.c, which shares this lever.
 *
 * `(int)&_AREA_6f` must be a SYMBOL because the ROM loads it with a WORD `ldr`; a literal produces
 * an `ldrh` pool load via the halfword exception, and is 61 differing. `_AREA_6f` was already in
 * area.sym.
 *
 * NOTE FOR READERS OF objcmp: this reports one "phantom" encoding difference and one extra
 * relocation, because `_AREA_6f` is an absolute symbol the LINKER fills -- ref 0x0000006f against
 * our 0x00000000 placeholder. All 175 encodings and every other relocation are identical, and the
 * gate is `make compare`, which is green.
 */
typedef struct {
    unsigned char pad00[0x1c2];
    short f1c2;
    short f1c4;
    short f1c6;
    unsigned char pad1c8[0x2c0 - 0x1c8];
} GlobalState;

extern GlobalState gState;
extern unsigned char *iwram_3001ebc;
extern int _AREA_6f;
extern int L1940 __asm__(".L1940");
extern int L1928 __asm__(".L1928");

extern void __SetFlag(int id);
extern int __GetFlag(int id);
extern int __Random(void);
extern int __StartTask(void *f, int n);
extern void __MapActor_SetPos(int slot, int x, int y);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __WaitFrames(int n);
extern void __SetCameraTarget(int a, int b);
extern void __Func_8093fa0(void);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_800fe9c(void);
extern void OvlFunc_944_20090a0(void);
extern void OvlFunc_944_2008468(void);
extern void OvlFunc_944_200840c(void);
extern void OvlFunc_944_2008564(void);
extern void OvlFunc_944_20087b0(void);
extern void OvlFunc_944_2008af8(void);
extern void OvlFunc_944_2008e78(void);

#define PIN2 register int q0 __asm__("r0"); \
             register int q1 __asm__("r1")

#define PIN3 register int q0 __asm__("r0"); \
             register int q1 __asm__("r1"); \
             register int q2 __asm__("r2")

int OvlFunc_944_2008240(void)
{
    unsigned char *a;

    __SetFlag(0xa2 << 1);
    *(int *)(iwram_3001ebc + (0xe0 << 1)) = 0x209;
    if ((__GetFlag(0x927) || __GetFlag(0x928)) && !__GetFlag(0x93e)
        && !__GetFlag(0x8a << 4)) {
        L1940 = (unsigned short)__Random();
        L1928 = (unsigned short)__Random();
        __StartTask(OvlFunc_944_20090a0, 0xc8 << 4);
    }
    if (__GetFlag(0x925) && !__GetFlag(0x93e)) {
        { PIN3; q0 = 8; q1 = 0xa4 << 16; q2 = 0xa4 << 17;
          __MapActor_SetPos(q0, q1, q2); }
    }
    switch (gState.f1c2) {
    case 1:
        if (!__GetFlag(0x109)) {
            a = __MapActor_GetActor(0);
            __CutsceneStart();
            __Func_8093fa0();
            *(int *)(a + 0xc) = 0xe0 << 14;
            { PIN2; q0 = -1; q1 = -1; __Func_80933f8(q0, q1, -1, 0); }
            __WaitFrames(1);
            __SetCameraTarget(0, 0);
            __Func_800fe9c();
            __WaitFrames(1);
            __CutsceneEnd();
        }
        break;
    case 10:
        if (__GetFlag(0x928))
            OvlFunc_944_2008468();
        else
            OvlFunc_944_200840c();
        break;
    case 11:
        gState.f1c4 = (int)(&_AREA_6f);
        gState.f1c6 = 0x1e;
        OvlFunc_944_2008564();
        break;
    case 12:
        gState.f1c4 = (int)(&_AREA_6f);
        gState.f1c6 = 0x1e;
        OvlFunc_944_20087b0();
        break;
    case 13:
        gState.f1c4 = (int)(&_AREA_6f);
        gState.f1c6 = 0x1e;
        OvlFunc_944_2008af8();
        break;
    case 14:
        OvlFunc_944_2008e78();
        break;
    }
    return 0;
}
