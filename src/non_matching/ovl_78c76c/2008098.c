/* OvlFunc_891_2008098 -- 0x02008098  (asm/overlays/rom_78c76c/ovl_30_c_c_a_a_c.s)
 *
 * BLOCKER: a constant REMATERIALISED by the ROM at a dominated use, where gcc
 * commons it. This is the SECOND instance of the contradiction docs/elevation.md
 * records as unresolved under "When gcc HOISTS a repeated constant, exactly:
 * dominance" -- see the OvlFunc_952_200be40 note at the end of that section.
 *
 * Instructions 0 through 53 are EXACT. The whole animation loop, both stack
 * argument pairs, the counter and the two post-loop calls all reproduce. The
 * residue is one call:
 *
 *     rom   mov r3, #0x2 / str r3, [sp,#0] / str r3, [sp,#4]   then r0-r3
 *     ours  r0-r3 first, then  str r7, [sp,#0] / str r7, [sp,#4]
 *
 * r7 is the loop's stack-argument value, which is also 2 and is still live.
 * gcc commons the later 2 with it; the ROM builds a fresh one into a scratch
 * register and fills the stack slots BEFORE the register arguments. By the
 * dominance rule the earlier 2 dominates this use, so gcc's behaviour is what
 * the rule predicts and the ROM's is what it does not. The likeliest reading
 * remains the one already recorded for 200be40 -- two different symbols in the
 * original that coincide in value -- and this tree has no symbol space to write
 * that with. Two instances now; if a third appears, that reading is worth
 * taking seriously rather than treating as a curiosity.
 *
 * Naming the later constant does NOT separate them: `e = 2;` assigned
 * immediately before the call is byte-identical to the literal, because
 * partial redundancy elimination folds the initialisers before it runs. Same
 * failure mode as the per-use-site naming lever against a dominating use.
 *
 * THREE LEVERS DID PAY, and the progression is worth keeping:
 *
 *   22 differ  naive
 *   16         name BOTH stack arguments as locals, per call site, on the two
 *              sites where the ROM uses TWO registers (`mov r3,#2 / mov r2,#1 /
 *              str r3 / str r2`) and we used one register twice
 *   15         `goto` loop instead of `for`. The ROM counts UP -- `add r5,#1 /
 *              cmp r5,#0x16 / bne` -- and gcc normalises a dead-counter loop to
 *              count DOWN, which no `for` spelling prevented. The goto lever
 *              disables loop optimisation entirely and the counter direction
 *              came back with it.
 *   first diff 13 -> 54, by splitting DISJOINT LIVE RANGES into separate
 *              variables. The stack pair is 2/1 during the loop and 4/3 after
 *              it; written as one pair of variables assigned twice, gcc keeps
 *              them in one pair of registers, but the ROM uses r7/r6 for the
 *              first and r5/r6 for the second -- r5 being the dead counter
 *              reused. Two variables per range gives the ROM's allocation and
 *              took the entire loop region to exact.
 *
 * That last step raises the differing COUNT to 24 while moving the first
 * divergence from instruction 13 to 54, which is why the count alone is a bad
 * ranking: the version kept below is one line short with a single localised
 * residue, and the 15-differing version is exact length with the divergence
 * running through the whole body. Structure first, count second.
 *
 * Also measured and inert: literals in the loop body instead of named values
 * (35 differ -- the goto lever disables the LICM that would have hoisted them),
 * separate stack locals for the first and last call sites, and declaration
 * order of the counter against the pair.
 */
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __CutsceneWait(int n);
extern void __PlaySound(int id);
extern void __SetFlag(int id);
extern void OvlFunc_891_20096dc(void);

void OvlFunc_891_2008098(void)
{
    int i;
    int a;
    int b;
    int c;
    int d;
    int s1;
    int s2;

    s1 = 2;
    s2 = 1;
    __CopyMapTiles(0, 0x1c, 0x11, 8, s1, s2);
    __PlaySound(0xc8);
    i = 0;
    a = 2;
    b = 1;
loop:
    __CopyMapTiles(0xa, 0x3d, 0x11, 0x28, a, b);
    __CutsceneWait(4);
    __CopyMapTiles(8, 0x3d, 0x11, 0x28, a, b);
    __CutsceneWait(4);
    i++;
    if (i != 0x16)
        goto loop;
    c = 4;
    d = 3;
    __CopyMapTiles(0, 0x3b, 0xf, 0x26, c, d);
    __CopyMapTiles(4, 0x3b, 0x11, 0x26, c, d);
    __CopyMapTiles(8, 0x3c, 0x11, 0x27, 2, 2);
    s1 = 0x11;
    s2 = 8;
    __Func_8010704(0, 0, 2, 1, s1, s2);
    __SetFlag(0x207);
    OvlFunc_891_20096dc();
}
