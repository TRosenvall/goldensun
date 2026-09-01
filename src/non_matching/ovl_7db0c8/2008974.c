/* OvlFunc_954_2008974 (0x02008974) -- NON-MATCHING.
 * Covers its twin OvlFunc_956_2008c5c too: tools/twin_families.py groups them
 * as one 61-instruction family, so the body below is the template for both.
 *
 * Blocker class: DUPLICATE-CONSTANT CSE in straight-line code.
 *
 * 63 lines against the ROM's 63, 29 differing, and the tell is the prologue
 * exactly as docs/elevation.md describes it:
 *
 *     rom    push {r5, r14}        one callee-saved register
 *     ours   push {r5, r6, r14}    two
 *
 * The function calls __MapActor_SetPos twice with the same `0xc0 << 16` third
 * argument. The ROM rebuilds `mov r2, #0xc0 / lsl r2, #0x10` at both sites; gcc
 * hoists `0xc0` into r5 before the first call and shifts a copy at each, which
 * costs the extra register and rotates everything after it. `0x80 << 7` repeats
 * the same way at the two __Func_809280c calls.
 *
 * NOT RE-MEASURED, deliberately. src/non_matching/ovl_7d30e0/200938c.c carries
 * the full table for this class -- per-use-site named locals byte-identical to
 * plain literals, and -fno-rerun-cse-after-loop, -fno-gcse and
 * -fno-cse-follow-jumps all inert -- and establishes why: the recorded
 * per-site-locals remedy is the dominating-block mechanism under another name
 * and needs a branch between the assignments and the uses. This function's only
 * branch is the animation choice, which sits AFTER both repeated pairs, so
 * there is no boundary. Re-deriving that table here would be re-deriving a
 * result the tree already holds.
 *
 * WHAT IS RIGHT: everything else. The two DeleteFieldActor calls, the
 * fixed-point positions, the `a < 0` animation choice written with the negative
 * case as the fall-through (the branch-polarity reading), the four-argument
 * Func_80933f8 and the common1 tail all come out correct on the first screen.
 *
 * NEXT: nothing source-level. If the duplicate-constant class is ever cracked,
 * this and OvlFunc_956_2008c5c close together off this file.
 */
extern void __DeleteFieldActor(int slot);
extern void __Func_807808c(int a);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __MapActor_SetPos(int slot, int x, int z);
extern void __Func_809280c(int a, int b, int c);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __WaitFrames(int n);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void OvlFunc_common1_fac(int a);

void OvlFunc_954_2008974(int a)
{
    __DeleteFieldActor(0x18);
    __DeleteFieldActor(0x19);
    __Func_807808c(1);
    __CutsceneStart();
    __MapActor_SetPos(8, 0xa5 << 19, 0xc0 << 16);
    __MapActor_SetPos(0, 0xa1 << 19, 0xc0 << 16);
    __Func_809280c(8, 0x80 << 7, 0);
    __Func_809280c(0, 0x80 << 7, 0);
    if (a < 0) {
        __MapActor_SetAnim(8, 0xa);
        __MapActor_SetAnim(0, 0x23);
    } else {
        __MapActor_SetAnim(8, 8);
        __MapActor_SetAnim(0, 0x1c);
    }
    __WaitFrames(1);
    __Func_80933f8(0xa3 << 19, 0, 0x80 << 16, 0);
    OvlFunc_common1_fac(a);
    __CutsceneEnd();
}
