/* OvlFunc_932_200abe0 -- NON-MATCHING, 3 differing STREAM encodings of 108.
 *
 * BODY RECOVERED FROM scratch/ (batch 338).  This park was PROSE -- a header,
 * a diagnosis, and no compilable body -- so it had no figure, was NO CLAIM in
 * parkcheck, and was unrankable in every band.  But its own header named a
 * candidate at scratch/Dabe0_best.c, that file still existed, and it
 * compiles.  The work had been done and left in a GITIGNORED directory instead
 * of installed, so nothing in the tree could see it.
 *
 * ref 108 / ours 108 stream encodings; 87/87 16-bit instructions -- IN PHASE, so a true DISTANCE.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/ovl_7b9cb4/200abe0.c \
 *     asm/overlays/rom_7b9cb4/ovl_30_a_c_c_a_c_c_a_c_a.s --func OvlFunc_932_200abe0
 *
 * 70 of the 133 prose parks were in this state; tools/scratchfill.py found them.
 * THE FIGURE IS EVIDENCE; the prose below it is a HYPOTHESIS, and across pass
 * two a park's stated blocker has been wrong roughly 40 times in 42.  The
 * original park text is preserved underneath.
 * BOTH MECHANICAL AXES ARE CLOSED -- measured batch 347 harvest pass, no agent.
 *   base 3 of 108, instruction counts EQUAL, byte size and pool
 *   count equal too (this park is in phase on all three length dimensions, so
 *   its figure IS a real distance).
 *   FLAG AXIS: all eight per-file groups measured, none improves the aligned
 *   residue.  ANCHOR AXIS: 25 placements swept, but 20 WERE BIT-IDENTICAL to
 *   the base, so only 5 DISTINCT PROGRAMS were actually tested.
 *   Those 5 are inert; the other 20 measured nothing.  No variant was capped and none failed to
 *   compile.
 *
 *   SO DO NOT RE-SCREEN THESE.  Every brief so far has spent part of a round on
 *   exactly that.  The residue here is NOT a placement or flag question; it is a
 *   register choice, a tie, or a pass-level effect, and it needs a hypothesis.
 *
 *   Read as "inert against THIS body".  A recorded negative expires silently
 *   when the body changes -- six batches running, the decisive lever has turned
 *   up inside a park's own negative list.
 */
extern unsigned int iwram_3001e40;
extern unsigned char gScript_932__0200c01c[];

extern unsigned int __Random(void);
extern void __PlaySound(int id);
extern unsigned char *__CreateActor(int a, int b, int c, int d);
extern void __Actor_SetSpriteFlags(unsigned char *a, int f);
extern void __Actor_SetAnim(unsigned char *a, int n);
extern void __Actor_SetScript(unsigned char *a, unsigned char *s);

void OvlFunc_932_200abe0(void)
{
    unsigned char *a;
    unsigned char *p;
    unsigned int r;
    int v;
    int c, d;

    c = 0xffc00000;
    d = 0x98 << 18;
    if (iwram_3001e40 % 3 != 0)
        return;
    a = __CreateActor(0xc8, (((__Random() * 3) << 4) >> 16 << 16) + 0x2fd0000,
                      c, d);
    if (a == 0)
        return;
    if (iwram_3001e40 % 9 == 0) {
        if ((__Random() << 1) >> 16)
            __PlaySound(0x91);
        else
            __PlaySound(0x90);
    }
    a[0x55] = 0;
    r = ((__Random() << 15) >> 16) + 0x4ccc;
    *(int *)(a + 0x48) = 0x6666;
    *(int *)(a + 0x1c) = r;
    *(int *)(a + 0x18) = r;
    a[0x61] = 1;
    __Actor_SetSpriteFlags(a, 0);
    a[0x23] &= 0xfe;
    p = *(unsigned char **)(a + 0x50);
    v = p[9];
    v = v & -13;
    v = v | 4;
    p[9] = v;
    __Actor_SetAnim(a, 1);
    __Actor_SetScript(a, gScript_932__0200c01c);
    *(int *)(a + 0x24) = ((((__Random() * 3) << 1) >> 16) - 3) << 16;
    *(int *)(a + 0x28) = 0x80 << 12;
    *(int *)(a + 0x2c) = (((__Random() * 3) << 9) >> 16) + 0xfffffd00;
}

#if 0
/* ===== ORIGINAL PROSE PARK, PRESERVED =====
(* OvlFunc_932_200abe0 -- 0x0200abe0, asm/overlays/rom_7b9cb4/ovl_30_a_c_c_a_c_c_a_c_a.s
 *
 * 103 of 103 lines, FIVE differing -- and only THREE are real.
 * Candidate: scratch/Dabe0_best.c.  Came down from 51 on the first screen.
 *
 * TWO OF THE FIVE ARE A MISSING LINKER ALIAS, not C.  Both `%` operations emit
 * `bl __umodsi3` where the ROM calls `bl _umodsi3_RAM`.  Other overlays carry
 * `__umodsi3 = _umodsi3_RAM;` in overlay.ld -- rom_7c7b9c does, and
 * src/overlays/rom_7a5214/ovl_17ec_c_b.c documents the pattern for __divsi3 --
 * but rom_7b9cb4's script has NO such alias.  Adding one is safe (each overlay
 * links separately) and would close those two lines.  It is NOT added here,
 * because an alias that does not complete a match is a link-script change that
 * does not pay for itself; add it in the round that closes the other three.
 *
 * FOUR FINDINGS GOT IT FROM 51 TO 5, all reusable:
 *
 *   1. THE MODULO HELPER SAYS UNSIGNED.  `_umodsi3_RAM`, not `_modsi3_RAM`, so
 *      iwram_3001e40 and __Random are `unsigned`.  The signedness oracle again.
 *   2. THE FIRST ARGUMENT MUST BE INLINE.  Naming the shifted __Random
 *      expression before __CreateActor costs a pseudo and turns the ROM's
 *      `add r1, r2` into `add r1, r0, r2`.  51 -> 44.
 *   3. STATEMENT ORDER IS THE STORE ORDER.  The ROM writes +0x1c and +0x18
 *      BEFORE the byte at +0x61; writing the byte first costs four positions.
 *   4. THE NEGATED MASK NEEDS AN INT INTERMEDIATE, and this one was worth 33
 *      lines by itself.  `p[9] = (p[9] & -13) | 4` truncates to a byte and gcc
 *      emits a single `mov r3, #0xf3`; the ROM builds -13 in SImode with
 *      `mov r3, #0xd / neg r3, r3`.  Through an int local it builds it, and
 *      because that is TWO instructions rather than one it also fixed the
 *      length -- the whole tail was a one-instruction shift.  38 -> 5.
 *
 * BLOCKER: the commutative register-role swap, the same wall as 200a5c0.c.
 *      rom   mov r3, #0xd / ldrb r2, [r1, #9] / neg r3, r3
 *      ours  mov r2, #0xd / ldrb r3, [r1, #9] / neg r2, r2
 * The `and` and the `strb` are identical; only which register holds the constant
 * and which the loaded byte differs.  Both assignment orders were screened
 * (constant first, load first) and both give the identical 5, which is the
 * documented boundary -- source order picks registers for two INDEPENDENT
 * values, not for the operands of a commutative operator.
 *)

*/
#endif
