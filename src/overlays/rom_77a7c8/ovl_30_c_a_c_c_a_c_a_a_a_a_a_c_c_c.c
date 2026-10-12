/* OvlFunc_881_2009a98 -- 0x02009a98.  PINNED EXACT, 3 register pins.
 * *** HELD FOR THE COORDINATOR: this is a landing that needs pins. ***
 *
 *   with 3 pins (as written)  0 -- 196 bytes, 73 encodings, 21 relocations identical
 *   PIN-FREE                  63 of 73, 46 / 48 instructions, 192 / 196 bytes
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/overlays/rom_77a7c8/ovl_30_c_a_c_c_a_c_a_a_a_a_a_c_c_c.c \
 *     asm/overlays/rom_77a7c8/ovl_30_c_a_c_c_a_c_a_a_a_a_a_c_c_c.s --func OvlFunc_881_2009a98
 *
 * Split shape: NONE NEEDED.  The reference .s holds exactly ONE function
 * (tools/piece.py; `grep -c thumb_func_start` = 1) and tools/datacheck.py
 * reports no data residue.
 *
 * THE RESIDUE WAS THE `-1` TRIPLE, and the park's prose was RIGHT about it --
 * the whole 2-instruction gap is in the first call and nothing else:
 *      rom   mov r0,#1 / mov r1,#1 / mov r2,#1 / mov r3,#0 / neg r1 / neg r2 / neg r0
 *      ours  mov r2,#1 / neg r2,r2 / mov r3,#0 / mov r0,r2 / mov r1,r2
 * gcc materialises one -1 and copies it; the ROM materialises three.
 *
 * HOW IT WAS FOUND: the LABELLED CORPUS, one command.  Scanning the 4,543
 * GENERATED .s files for two consecutive `neg` instructions gives 141 hits, and
 * one is asm/overlays/rom_77a7c8/ovl_30_c_a_c_c_a_c_a_a_a_a_a_c_c_b.s lines
 * 16-22: the ROM's exact seven-instruction window, emitted by gcc, in the same
 * directory and cut from the SAME PARENT .s as this function.
 *
 * The .c beside it holds FOUR LANDED FUNCTIONS and three of them --
 * OvlFunc_881_2009888 / _2009938 / _20099e8 -- are NEAR-IDENTICAL TWINS of this
 * one: same calls in the same order, differing only in the __Func_8091e9c
 * argument (0x67 / 0x68 / 0x69 against our 0x6a) and in that this one chooses
 * between two behaviour scripts.  This body is theirs with the branch added.
 *
 * LADDER, every rung measured:
 *      pin-free (Actor *, named zero, no pins)      46 / 48   63 of 73
 *      + p0/p1 on the -1 triple        (2 pins)     48 / 48    4 of 73
 *      + p1/p2 on __MapActor_SetSpeed  (3 pins)     48 / 48   EXACT
 *
 * THREE PINS IS THE MINIMUM.  Every one-pin reduction measured:
 *      drop p0 at the SetSpeed site        3 of 73
 *      drop p1 at the -1 site              47 / 48 instructions -- count REGRESSES
 *      drop p0 at the -1 site              47 / 48 instructions -- count REGRESSES
 *
 * THE SetSpeed PIN SET IS A FOURTH SHAPE.  The landed sibling's header says
 * "20097fc needs all three argument registers pinned; the other three need only
 * r0 ... pin the minimum each site needs".  This one needs r1 and r2 as well,
 * because what FOLLOWS the call differs -- a named-zero store and then a call
 * returning into r0.  At 2 pins the only thing wrong is the pool-load order:
 *      rom   ldr r1, =0x9999 / ldr r2, =0x4ccc
 *      ours  ldr r2, =0x4ccc / ldr r1, =0x9999
 * Batch 308's int-carrier lever does NOT reach it: an `int` carrier on 0x9999,
 * one on 0x4ccc, and one on both are all inert at 4 of 73 with the SAME first
 * differing index.
 *
 * NAMED ZERO, confirmed independently of the sibling: `*(short *)e = 0;` emits
 * `ldr r3, =0x0` where the ROM has `mov r3, #0x0` -- the narrowed
 * HImode-literal rule (docs/elevation.md:5214).  `int z = 0; *g = z;` restores
 * the `mov`.  It is NOT part of the length gap; the pin-free figure is 63 of 73
 * either way.
 *
 * MEASURED FLAT, pin-free: an 18-cell CROSS of three bodies (plain `-1,-1,-1`;
 * three locals assigned just before the call; three locals assigned at the top)
 * against six flags (none, -fno-gcse, -fno-rerun-cse-after-loop,
 * -fno-rerun-loop-opt, -fno-schedule-insns2, -O1).  Every cell 46 / 48 except
 * the early-definition body, which is WORSE at 45 / 48.  So the pin-free
 * ceiling really is 46 / 48 and the pins are the only route.
 * AUTOSWEEP on the park body: all 8 flag groups 63 of 73, 23 of 24 anchors
 * inert (1 did not measure).  Both cheap axes CLOSED.
 *
 * PRECEDENT FOR THE PINS, which is why this is held rather than parked:
 * fakematch.txt rows 443-446 are the four functions of the ADJACENT TU, cut
 * from the SAME parent .s, landed in commit 66a6dc35 ("batch 269's pin lever
 * closes the -1 cluster"), using these exact pins for this exact construct;
 * row 441 is OvlFunc_945_200c13c, the same lever.  This is the fifth member of
 * a family already booked four times.  Func_80979a4 was DEFERRED TO PASS 3
 * (docs/owner-decisions.md) because it would be a fourth pin on a veneer that
 * already carries three; this is the same three pins the veneer's own family
 * already uses.  If the ruling is pin-free-only, the park figure is 63 of 73 at
 * 46 / 48 and the ladder above is what pass 3 needs.
 *
 * Fields are include/actor.h's: 0x18 rotX, 0x1c rotY, 0x64 goalFacing (read
 * SIGNED here via a local `short *`, as the sibling cluster does).
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
extern int OvlFunc_881_200b41c(void);
extern unsigned char gScript_881__0200d158[];
extern unsigned char gScript_881__0200d1b8[];

void OvlFunc_881_2009a98(void)
{
    register int p0 __asm__("r0");
    register int p1 __asm__("r1");
    register int p2 __asm__("r2");
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
    p1 = 0x9999;
    p2 = 0x4ccc;
    __MapActor_SetSpeed(p0, p1, p2);
    g = (short *)&a->goalFacing;
    z = 0;
    *g = z;
    if (OvlFunc_881_200b41c() == 0xb)
        __MapActor_SetBehavior(8, gScript_881__0200d1b8);
    else
        __MapActor_SetBehavior(8, gScript_881__0200d158);
    do {
        __WaitFrames(1);
    } while (*g == 0);
    __MapTransitionOut();
    __WaitMapTransition();
    __SetFlag(0x927);
    __Func_8091e9c(0x6a);
    __CutsceneEnd();
}
