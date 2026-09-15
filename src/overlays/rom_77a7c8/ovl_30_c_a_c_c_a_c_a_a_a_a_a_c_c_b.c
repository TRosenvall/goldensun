// fakematch
/* Cluster OvlFunc_881_20097fc..OvlFunc_881_20099e8 extracted from goldensun/asm/overlays/rom_77a7c8/ovl_30_c_a_c_c_a_c_a_a_a_a_a_c_c.s.
 *
 * Total .text for this TU = 668 bytes (= 0x29c).
 * Preserves the original ROM layout when slotted between
 * asm/overlays/rom_77a7c8/ovl_30_c_a_c_c_a_c_a_a_a_a_a_c_c_a.o and asm/overlays/rom_77a7c8/ovl_30_c_a_c_c_a_c_a_a_a_a_a_c_c_c.o in
 * goldensun/overlays/rom_77a7c8/overlay.ld.
 *
 * FOUR FUNCTIONS, one park. src/non_matching/overlays/20097fc.c stood for all
 * four and named the blocker as the `-1` constant-rematerialisation case,
 * pointing at OvlFunc_945_200c13c. That function was elevated in batch 269 with
 * a bare register pin, and the same pin closes all four here.
 *
 * TEARDOWN, by REMOVING each piece from the finished file:
 *
 *   the p0/p1 pins on __Func_80933f8(-1, -1, -1, 0)   40 / 56 / 56 / 56 differing
 *   the pins on __MapActor_SetSpeed                    3 /  2 /  2 /  2
 *   the named zero for the goalFacing store            pool 23 entries against 20,
 *                                                      then 7 / 7 / 7
 *   nothing (as landed)                                0
 *
 * THE SetSpeed PIN COUNT IS ASYMMETRIC and the teardown is what found it.
 * 20097fc needs all three argument registers pinned; the other three need only
 * r0. 20097fc's SetSpeed is followed by __Func_80921c4 with two more pool loads
 * and the siblings' is followed by a store, so the interleave being fixed is not
 * the same one. Four functions that look identical at this call do not take the
 * same scaffolding -- pin the minimum each site needs, not the maximum any site
 * needed.
 *
 * THE NAMED ZERO IS A POOL FACT, NOT A SCHEDULING ONE. `*g = 0` through a
 * `short *` narrows the constant to HImode, and HImode constants go to the
 * literal pool -- so gcc emits `ldr r3, =0x0` where the ROM has `mov r3, #0`.
 * The pool is per TRANSLATION UNIT, so the extra word shows up as a pool-size
 * mismatch on 20097fc, which does not contain the store at all. An `int` local
 * assigned 0 and then stored keeps SImode and the `mov` comes back.
 *
 * The three siblings differ from each other in exactly one instruction -- the
 * argument to __Func_8091e9c, 0x67 / 0x68 / 0x69 -- which is what the park
 * predicted.
 *
 * Fields are include/actor.h's: 0x18 rotX, 0x1c rotY, 0x64 goalFacing. Note
 * 0x64 is READ SIGNED here (`ldrsh`, which in Thumb-1 has no immediate form,
 * hence the register-offset load) while actor.h declares it u16; the store and
 * the test go through a local `short *` rather than changing a shared header on
 * one cluster's evidence.
 */
#include "actor.h"

extern Actor *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __WaitFrames(int n);
extern void __MapActor_SetPos(int slot, int x, int z);
extern void __MapActor_SetSpeed(int slot, int vx, int vz);
extern void __MapActor_SetBehavior(int who, unsigned char *s);
extern void __SetCameraTarget(int slot, int n);
extern void __MapTransitionIn(void);
extern void __MapTransitionOut(void);
extern void __WaitMapTransition(void);
extern void __SetFlag(int id);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_8091e9c(int n);
extern unsigned char gScript_881__0200d158[];

void OvlFunc_881_20097fc(void)
{
    register int p0 __asm__("r0");
    register int p1 __asm__("r1");
    register int p2 __asm__("r2");
    Actor *a;

    a = __MapActor_GetActor(8);
    __CutsceneStart();
    p0 = -1;
    p1 = -1;
    __Func_80933f8(p0, p1, -1, 0);
    __WaitFrames(1);
    __MapActor_SetPos(0, 0, 0);
    a->rotY = 0xa0 << 9;
    a->rotX = 0xa0 << 9;
    __SetCameraTarget(8, 1);
    __MapTransitionIn();
    p0 = 8;
    p1 = 0x6666;
    p2 = 0x3333;
    __MapActor_SetSpeed(p0, p1, p2);
    __Func_80921c4(8, 0x14a8, 0x918);
    __MapTransitionOut();
    __WaitMapTransition();
    __SetFlag(0x927);
    __Func_8091e9c(0x66);
    __CutsceneEnd();
}

void OvlFunc_881_2009888(void)
{
    register int p0 __asm__("r0");
    register int p1 __asm__("r1");
    Actor *a;
    short *g;
    int z;

    a = __MapActor_GetActor(8);
    __CutsceneStart();
    p0 = -1;
    p1 = -1;
    __Func_80933f8(p0, p1, -1, 0);
    __WaitFrames(1);
    __MapActor_SetPos(0, 0, 0);
    __MapActor_SetPos(8, 0x1f080000, 0xc8 << 16);
    a->rotX = 0xa0 << 9;
    a->rotY = 0xa0 << 9;
    __WaitFrames(1);
    __SetCameraTarget(8, 1);
    __MapTransitionIn();
    p0 = 8;
    __MapActor_SetSpeed(p0, 0x9999, 0x4ccc);
    g = (short *)&a->goalFacing;
    z = 0;
    *g = z;
    __MapActor_SetBehavior(8, gScript_881__0200d158);
    do {
        __WaitFrames(1);
    } while (*g == 0);
    __MapTransitionOut();
    __WaitMapTransition();
    __SetFlag(0x927);
    __Func_8091e9c(0x67);
    __CutsceneEnd();
}

void OvlFunc_881_2009938(void)
{
    register int p0 __asm__("r0");
    register int p1 __asm__("r1");
    Actor *a;
    short *g;
    int z;

    a = __MapActor_GetActor(8);
    __CutsceneStart();
    p0 = -1;
    p1 = -1;
    __Func_80933f8(p0, p1, -1, 0);
    __WaitFrames(1);
    __MapActor_SetPos(0, 0, 0);
    __MapActor_SetPos(8, 0x1f080000, 0xc8 << 16);
    a->rotX = 0xa0 << 9;
    a->rotY = 0xa0 << 9;
    __WaitFrames(1);
    __SetCameraTarget(8, 1);
    __MapTransitionIn();
    p0 = 8;
    __MapActor_SetSpeed(p0, 0x9999, 0x4ccc);
    g = (short *)&a->goalFacing;
    z = 0;
    *g = z;
    __MapActor_SetBehavior(8, gScript_881__0200d158);
    do {
        __WaitFrames(1);
    } while (*g == 0);
    __MapTransitionOut();
    __WaitMapTransition();
    __SetFlag(0x927);
    __Func_8091e9c(0x68);
    __CutsceneEnd();
}

void OvlFunc_881_20099e8(void)
{
    register int p0 __asm__("r0");
    register int p1 __asm__("r1");
    Actor *a;
    short *g;
    int z;

    a = __MapActor_GetActor(8);
    __CutsceneStart();
    p0 = -1;
    p1 = -1;
    __Func_80933f8(p0, p1, -1, 0);
    __WaitFrames(1);
    __MapActor_SetPos(0, 0, 0);
    __MapActor_SetPos(8, 0x1f080000, 0xc8 << 16);
    a->rotX = 0xa0 << 9;
    a->rotY = 0xa0 << 9;
    __WaitFrames(1);
    __SetCameraTarget(8, 1);
    __MapTransitionIn();
    p0 = 8;
    __MapActor_SetSpeed(p0, 0x9999, 0x4ccc);
    g = (short *)&a->goalFacing;
    z = 0;
    *g = z;
    __MapActor_SetBehavior(8, gScript_881__0200d158);
    do {
        __WaitFrames(1);
    } while (*g == 0);
    __MapTransitionOut();
    __WaitMapTransition();
    __SetFlag(0x927);
    __Func_8091e9c(0x69);
    __CutsceneEnd();
}

