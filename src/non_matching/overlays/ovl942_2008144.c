/* OvlFunc_942_2008144 -- 0x02008144  (asm/overlays/rom_7c6bac/ovl_30_c_c_a_a_b.s)
 *
 * BLOCKER: duplicate-constant CSE into a callee-saved register.  NEW CLASS.
 *
 * Structurally exact and trivially readable -- a straight-line cutscene with no
 * control flow -- yet 18 of 30 instructions differ, because of ONE decision.
 *
 * The function calls __Func_80921c4 twice, and both calls take 0x94 << 1 as the
 * third argument.  0x128 is not an 8-bit immediate, so each occurrence costs a
 * `mov` + `lsl` pair.  gcc notices the repeat, computes it ONCE into r5, and
 * holds it across the intervening __MapActor_SetSpeed call.  Holding a value
 * across a call requires a callee-saved register, so r5 joins the prologue:
 *
 *     rom   push {lr}       ...  mov r2, #0x94 / lsl r2, #1   (both times)
 *     ours  push {r5, lr}   ...  mov r5, #0x94 / lsl r5, #1 / mov r2, r5
 *
 * That single hoist changes the prologue, the epilogue (`pop {r0}/bx r0` becomes
 * `pop {r5}/pop {r0}/bx r0`, +1 line), and the argument-fill order at both call
 * sites -- hence 18 differing from one cause.  The ROM recomputes the constant.
 *
 * WHAT WAS TRIED -- four spellings of the shared constant:
 *
 *     0x128,     0x128        18 differ
 *     0x94 << 1, 0x128        18 differ
 *     0x94 * 2,  0x94 * 2     18 differ
 *     0x128,     0x94 << 1    18 differ
 *
 * All four are BYTE-IDENTICAL to each other.  This is the finding: constant
 * folding runs before CSE, so every spelling of a compile-time constant has
 * collapsed to the same value by the time the hoist is decided.  Unlike the
 * pooled-constant tell, where `X << 1` vs `0x128` genuinely selects a different
 * instruction sequence, here the source form is not observable at all.
 *
 * Also ruled out:
 *     --no-rerun-cse    18 differ    (not -frerun-cse-after-loop)
 *     --O1              18 differ    (not an -O2-only pass)
 *
 * So it is the main CSE pass, and no flag the project uses would suppress it.
 *
 * HOW TO RECOGNISE IT: two or more calls in one function passing the SAME
 * non-encodable constant, with the ROM recomputing it each time and a
 * callee-saved register appearing in our prologue that the ROM does not push.
 * The extra push is the cheapest tell -- check the prologue before diffing.
 *
 * NOT YET RULED OUT: that the constants are not equal in the original source at
 * all, i.e. the two 0x94 values are distinct named symbols that happen to share
 * a value (a coordinate and a height, say).  If a future symbol pass gives them
 * different names this stays byte-identical -- folding still collapses them --
 * so that would not fix it either.  The only escape would be a form gcc cannot
 * fold, and every such form costs a load the ROM does not have.
 */
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __SetFlag(int id);
extern void __Func_80921c4(int a, int x, int y);
extern void __MapActor_SetSpeed(int a, int x, int y);
extern void __Func_8092adc(int a, int x, int y);

void OvlFunc_942_2008144(void)
{
    __CutsceneStart();
    __SetFlag(0x8aa);
    __Func_80921c4(0, 0xc4 << 1, 0x94 << 1);
    __MapActor_SetSpeed(8, 0x13333, 0x9999);
    __Func_80921c4(8, 0xcc << 1, 0x94 << 1);
    __Func_8092adc(8, 0x80 << 8, 0);
    __CutsceneWait(0x14);
    __CutsceneEnd();
}
