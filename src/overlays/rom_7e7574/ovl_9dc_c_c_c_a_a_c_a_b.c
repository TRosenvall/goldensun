/* OvlFunc_959_200d0e4  --  0x0200d0e4      EXACT
 *
 * From asm/overlays/rom_7e7574/ovl_9dc_c_c_c_a_a_c_a.s (3 functions; this is
 * the 3rd and last, so split_s writes ..._a.s with 200cda0 + 200cf60 and
 * ..._b.s with this one).  214 encodings, 576 bytes, 69 relocations.
 *
 * VERDICT
 *   OK OvlFunc_959_200d0e4 -- 576 bytes, 214 encodings and 69 relocations identical
 * from tools/objcmp.py, reproduced on three consecutive runs.  tools/datacheck.py
 * on the reference exits 0.  tools/shimcount.py exits 0: NO pins, no barriers,
 * no .equ, no device, no per-file flag group, plain -O2 default rule.
 *
 * ------------------------------------------------------------------ THE LEVER
 *
 * ONE EDIT against the park body, and the park called itself a FLOOR:
 *
 *     case 10/13/20/23/24:
 *         ...
 *         x = 0xda << 18;                 <-- ADDED, in the arm, BEFORE the if
 *         y = 0xf0 << 15;                 <-- ADDED
 *         if (__GetFlag(0xc5 << 2))
 *             __MapActor_SetPos(0x19, x, y);
 *
 * This is the GUARDED-INTERLEAVE lever exactly as docs/elevation.md states it:
 * "The interleave lever moves the argument you do NOT name" -- name the two
 * SPLIT BUILDS in the block DOMINATING the guarded call and leave the
 * single-instruction argument (the slot) a bare literal.  The ROM wants
 *
 *     mov r1,#0xda / mov r2,#0xf0 / mov r0,#0x19 / lsl r1,#18 / lsl r2,#15
 *
 * i.e. `mov r0` INSIDE the two split builds.
 *
 * WHY THE PARK MISSED IT, and this is the reusable part.  The park's nine
 * measured spellings include "x and y named in the if-body" and "named int x,y
 * locals built in two statements before the call" -- BOTH INSIDE THE ARM, both
 * 3 -- and "slot = 0x19 hoisted ABOVE the if", which names the WRONG argument.
 * The one position nobody tried is the one the lever actually requires: the two
 * split builds in the block that DOMINATES the guarded call.  elevation.md says
 * so twice ("Naming inside the arm still fails ... the dominance requirement is
 * established rather than suspected") and the park's own list is a map of
 * everywhere except there.
 *
 * So the park's chain of reasoning -- expand's precompute hoists the two
 * expensive constants above the cheap one, cse1 always folds a cost-0 constant
 * back down to the argument-load site, sched2's three-way tie falls through to
 * INSN_LUID -- is CORRECT AS MECHANISM and WRONG AS A BOUND.  Putting the two
 * expensive constants' defs in a different (dominating) basic block from the
 * call takes them out of the precompute stream at the call site entirely, so
 * the LUID order the park proved unreachable is reached without touching how
 * 0x19 is written.
 *
 * ------------------------------------------------- MEASURED, batch 329
 * Metric: tools/objcmp.py differing encodings, 214-encoding reference.
 *
 *   spelling                                                    differing
 *   ------------------------------------------------------------  -------
 *   SHIPPED: x,y assigned in the arm, before the if                     0
 *   same, assigned at the very TOP of the function                      0
 *   same, built in two statements each (x = 0xda; x <<= 18;)            0
 *   PARK BODY (x,y inline in the argument list)                         3
 * and from the park's own list, every spelling INSIDE the arm             3
 *
 * The three zero rows say the lever is insensitive to WHERE in a dominating
 * block the assignment sits and to whether the build is one statement or two;
 * it is sensitive only to the assignment being in a DIFFERENT BLOCK from the
 * call.  The arm-local spellings in the park's list are the control.
 *
 * Everything else in this function was already right in the park and is kept
 * unchanged: the switch subject's offset as a named local built in TWO
 * statements (off = 0xe1; off then shifted left by 1), which is what stops gcc
 * folding gState + 0x1c2 into one pool word and is worth 4 encodings and 8
 * bytes; the 31-entry jump table; the crossjumped tail shared by the
 * case 1,2,3 and case 21,22 arms, written out in full in both; the two separate
 * __MapActor_GetActor(8) calls in the tail.
 */
typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern unsigned char *iwram_3001ebc;
extern void OvlFunc_959_200d4b0(void);
extern void OvlFunc_959_2009150(void);
extern void OvlFunc_959_200938c(void);
extern void OvlFunc_959_2009a44(void);
extern void OvlFunc_959_200a06c(void);
extern void __Func_8092950(int a, int b);
extern void __Func_8092b08(int a, int b);
extern void __Func_80108c4(int a);
extern void __Func_800fe9c(void);
extern void __StartTask(void (*f)(void), int a);
extern void __WaitFrames(int n);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __Actor_SetSpriteFlags(unsigned char *a, int f);
extern void __MapActor_SetPos(int slot, int x, int y);
extern int __GetFlag(int id);

void OvlFunc_959_200d0e4(void)
{
    unsigned char *a;
    unsigned int off;
    int x, y;

    OvlFunc_959_200d4b0();
    __Func_8092950(9, 1);
    __Func_8092950(0xa, 1);
    __Func_8092950(0x11, 1);
    if (__GetFlag(0x94c))
        __MapActor_SetPos(0xf, 0, 0);
    if (__GetFlag(0x949))
        __MapActor_SetPos(0xb, 0, 0);
    if (__GetFlag(0x94b))
        __MapActor_SetPos(0x10, 0, 0);
    if (__GetFlag(0xf2e))
        __MapActor_SetPos(8, 0, 0);
    off = 0xe1;
    off <<= 1;
    switch (*(short *)((unsigned char *)&gState + off)) {
    case 1:
    case 2:
    case 3:
        *(int *)(iwram_3001ebc + (0xe0 << 1)) = 0x200;
        __Func_80108c4(0xe0 << 4);
        __StartTask(OvlFunc_959_2009150, 0xc8 << 4);
        __WaitFrames(1);
        __Func_800fe9c();
        __WaitFrames(1);
        break;
    case 10:
    case 13:
    case 20:
    case 23:
    case 24:
        *(int *)(iwram_3001ebc + (0xe0 << 1)) = 0x209;
        __Func_80108c4(0xc0 << 4);
        __Actor_SetSpriteFlags(__MapActor_GetActor(0x18), 0);
        x = 0xda << 18;
        y = 0xf0 << 15;
        if (__GetFlag(0xc5 << 2))
            __MapActor_SetPos(0x19, x, y);
        break;
    case 21:
    case 22:
        *(int *)(iwram_3001ebc + (0xe0 << 1)) = 0x200;
        __Func_80108c4(0xe0 << 4);
        __StartTask(OvlFunc_959_200938c, 0xc8 << 4);
        __WaitFrames(1);
        __Func_800fe9c();
        __WaitFrames(1);
        break;
    case 11:
    case 12:
        *(int *)(iwram_3001ebc + (0xe0 << 1)) = 0x200;
        if (__GetFlag(0x94a))
            OvlFunc_959_200a06c();
        break;
    case 31:
        *(int *)(iwram_3001ebc + (0xe0 << 1)) = 0x200;
        OvlFunc_959_200a06c();
        break;
    case 14:
    case 15:
    case 16:
        __StartTask(OvlFunc_959_2009a44, 0xc8 << 4);
        break;
    default:
        *(int *)(iwram_3001ebc + (0xe0 << 1)) = 0x200;
        __Func_80108c4(0xe0 << 4);
        break;
    }
    a = __MapActor_GetActor(8);
    __Actor_SetSpriteFlags(__MapActor_GetActor(8), 0);
    __Func_8092b08(8, 1);
    *(int *)(a + 0x18) = 0xc0 << 8;
    *(int *)(a + 0x1c) = 0xc0 << 8;
}
