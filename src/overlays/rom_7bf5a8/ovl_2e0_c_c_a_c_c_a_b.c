/* OvlFunc_935_20088a8  --  0x020088a8
 *
 * Cut out of goldensun/asm/overlays/rom_7bf5a8/ovl_2e0_c_c_a_c_c_a.s.
 *
 * A one-shot scene, guarded by flag 0x9a8 so it plays only the first time:
 * unlock entry 0x1528, set the flag, then two pairs of tile rewrites with
 * waits and a sound between them, and hand off to OvlFunc_935_2008754.
 *
 * NEEDS CSE_CFLAGS (-fno-rerun-cse-after-loop). Flag id 0x9a8 is read once
 * before the guard branch, to test it, and once after, to set it -- the first
 * use DOMINATES the second, which is the recorded guard/set shape this flag
 * group exists for. Without it gcc parks the id in callee-saved r5 and feeds
 * the second site with `mov r0, r5` where the ROM reloads `ldr r0, =0x9a8`.
 * -fno-gcse and -O1 do not help.
 *
 * The intervening calls do not excuse the rebuild, for the reason recorded on
 * OvlFunc_891_2009b44: there are two calls between the two uses, but the
 * constant lives in a CALLEE-SAVED register and survives them both.
 *
 * A NOTE ON THE SCREEN COUNT, because it is misleading here. The plain -O2
 * candidate reports "60 differing of 60" -- apparently a total mismatch -- when
 * only THREE instructions actually disagree. One extra instruction near the top
 * shifts every later line by one and difflib aligns almost nothing. Read the
 * itemised regions, not the headline number, before concluding a candidate is
 * structurally wrong; this one was correct on the first try.
 *
 * THE TWO STACK ARGUMENTS ARE NAMED LOCALS, and the tell is the one recorded on
 * OvlFunc_927_2009c34 read the other way. Here the ROM materialises both into
 * registers BEFORE either stack store --
 *
 *     mov r5, #0x1b / mov r6, #0x5c / ... / str r6, [sp] / str r5, [sp, #4]
 *
 * -- which is the named-local order. Had they been literals gcc would have
 * hoisted them, and the first use would have gone straight to the stack with
 * the register copy following. The second value changes to 0x19 partway
 * through while the first stays 0x1b across all four calls, which is what makes
 * a variable the natural reading anyway.
 */

extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __PlaySound(int id);
extern void __CutsceneWait(int n);
extern void __Func_801776c(int a, int b);
extern void __Func_80105d4(int a, int b, int c, int d, int e, int f);
extern void OvlFunc_935_2008754(void);

void OvlFunc_935_20088a8(void)
{
    int a;
    int b;

    if (__GetFlag(0x9a8))
        return;
    __Func_801776c(0x1528, 1);
    __SetFlag(0x9a8);
    __PlaySound(0x9b);
    a = 0x1b;
    b = 0x5c;
    __Func_80105d4(0x6b, 0x1b, 1, 1, b, a);
    __CutsceneWait(0x27);
    __Func_80105d4(0x6c, 0x1b, 1, 1, b, a);
    __CutsceneWait(0x32);
    __PlaySound(0x9c);
    b = 0x19;
    __Func_80105d4(1, 0x18, 1, 2, b, a);
    __CutsceneWait(0x28);
    __Func_80105d4(2, 0x18, 1, 2, b, a);
    __CutsceneWait(0x28);
    OvlFunc_935_2008754();
}
