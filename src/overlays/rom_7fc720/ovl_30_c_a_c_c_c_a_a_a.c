/* OvlFunc_973_20080c0 (RaisePartyLevels)  --  LANDS, byte-identical
 *
 *   OK OvlFunc_973_20080c0 -- 44 bytes, 20 encodings and 2 relocations identical
 * --func and --whole both green.
 *
 * INSTALL AT: src/overlays/rom_7fc720/ovl_30_c_a_c_c_c_a_a_a.c
 *   - delete asm/overlays/rom_7fc720/ovl_30_c_a_c_c_c_a_a_a.s (the next build
 *     writes a generated .s to the same path; that file belongs in the commit)
 *   - NO linker change: exactly one .ld row names the stem,
 *     overlays/rom_7fc720/overlay.ld:24
 *         asm/overlays/rom_7fc720/ovl_30_c_a_c_c_c_a_a_a.o(.text)
 *     and the build puts the object there for either source form.
 *   - NO fakematch.txt row: tools/shimcount.py reports nothing -- zero pins,
 *     no inline-asm barrier.  `static inline` is ordinary C.
 *   - delete the park src/non_matching/ovl_7fc720/20080c0.c
 *
 * SPLIT SHAPE: none needed.
 *   python3 tools/datacheck.py asm/overlays/rom_7fc720/ovl_30_c_a_c_c_c_a_a_a.s
 *     -> (silent; no data section)
 *   python3 tools/split_s.py ... OvlFunc_973_20080c0 --dry-run
 *     -> "holds only OvlFunc_973_20080c0 and no data; convert it directly,
 *         no split needed"
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/overlays/rom_7fc720/ovl_30_c_a_c_c_c_a_a_a.c \
 *     asm/overlays/rom_7fc720/ovl_30_c_a_c_c_c_a_a_a.s --func OvlFunc_973_20080c0
 *   (and the same with --whole)
 *
 * WHAT IT DOES
 * r0 = a number of levels.  Widens the party roster into a 32-byte stack array
 * via __Func_80796c4 and applies OvlFunc_973_20080a0(member, levels) to each of
 * the n members it reports.
 *
 * ------------------------------------------------------------------
 * THE FIGURE, AND THE PARK'S TWO ERRORS
 *
 * Measured: 7 of 20, first at index 2, AND `objcmp --whole` reports
 *     XX RELOCATIONS differ
 *        ref  [['00000008', 'R_ARM_THM_CALL', '__Func_80796c4'], ...]
 *        ours [['0000000a', 'R_ARM_THM_CALL', '__Func_80796c4'], ...]
 * The park says "7 of 22, same length".  It is 7 of 20 -- the .s has twenty
 * instructions, not twenty-two -- and the relocation OFFSET differs because the
 * misplaced prologue insn pushes the `bl` two bytes along.  (That second one is
 * a consequence of the instruction residue, not an independent fault; it closes
 * with it.  But "7 of 22" was never a distance from anything.)
 *
 * The park's NAME for the blocker is right -- the frame address is computed once
 * and shared across the call where the ROM rebuilds it -- and its "SOLVED, and
 * worth reusing: THE POST-INCREMENT BELONGS IN THE CALL" paragraph is correct
 * and is kept below.  What it got wrong is the verdict: it closes with "Same
 * family as rom_15000/801c954.c and ovl_7b2078/2008388.c: a value the ROM
 * rebuilds and gcc shares across a call boundary, WHERE THE DOCUMENTED REMEDY
 * DOES NOT APPLY."  There is a documented remedy, it is in the tree, and it is
 * in a landed file that names this exact blocker (see THE LEVER below).
 *
 * ------------------------------------------------------------------
 * THE MECHANISM, read at the instruction
 *
 * `-da` on the park body.  In `.00.rtl` there are TWO INDEPENDENT frame-address
 * insns, which is already the ROM's shape:
 *     insn 14: (set (reg 36) (plus (reg 28 virtual-stack-vars) (const_int -32)))
 *     insn 16: (set (reg r0) (reg 36))        <- the call argument
 *     call 17 ; insn 19: (set (reg 35) (reg r0))   <- n
 *     jump 21  (the ble)
 *     insn 25: k = n
 *     insn 27: (set (reg 33) (plus (reg 28 virtual-stack-vars) (const_int -32)))
 *                                             <- p = buf, ITS OWN plus
 * and in `.03.cse` insn 27 has become
 *     insn 27: (set (reg 33) (reg 36))  REG_EQUAL (plus (reg 25 sfp) -32)
 *
 * So the blocker is CSE1, and it is ONE PASS EARLIER than any allocator story:
 * cse's extended basic block runs from the function start through `p = buf`,
 * finds `(plus sfp -32)` already in the table with reg36 as its cheapest
 * equivalent, and substitutes.  reg36 is a PSEUDO, so unlike a hard register it
 * is not invalidated at the call; it therefore lives from before the call to
 * after the branch, must have a callee-saved register, takes r6, and the
 * argument becomes a copy `mov r0, r6`.  Two insns, in the wrong two places.
 *
 * Three things were ruled out on the dumps rather than guessed:
 *   - it is NOT gcse and NOT cse2: `-fno-gcse` is exactly inert (6 at the same
 *     six indices) and `-fno-rerun-cse-after-loop` is WORSE (11).
 *   - it is NOT cse's jump following: `-fno-cse-follow-jumps` and
 *     `-fno-cse-skip-blocks` are both exactly inert.  cse reaches `p = buf` by
 *     plain fall-through, not by following the branch.
 *   - an intervening CODE_LABEL does NOT stop it.  A probe with a real extra
 *     branch inside the `if` puts `code_label 29` ahead of `p = buf` and
 *     `.03.cse` STILL rewrites it to `(reg 36)`.
 *   Also inert: -fno-expensive-optimizations, -fno-thread-jumps,
 *   -fno-strength-reduce, -fno-regmove, -fno-force-mem, -fno-caller-saves.
 *   (-fno-schedule-insns2 is 9, -fno-omit-frame-pointer 25 -- both irrelevant.)
 *
 * ------------------------------------------------------------------
 * THE LEVER: ROUTE THE BUFFER THROUGH A `static inline` PARAMETER
 *
 * Already discovered and written up in the tree, in a LANDED file that names
 * this blocker in the same words -- src/rom_c9000/rom_e3958_c_c_c_c_b.c
 * (Func_80e46f0):
 *
 *   "THE INLINE-PARAMETER LEVER: without it gcc computes &buf for the stack
 *    array once, before the first call, and keeps it alive to the last -- a
 *    wasted callee-saved register that shifts everything.  The ROM passes
 *    `mov r0,sp` / `mov r1,sp` at each call and makes a fresh copy only after
 *    the first call.  Routing BOTH calls' buf argument through a `static
 *    inline` parameter gives a new (set reg (plus sfp -128)) after the call.
 *    Only one site inlined leaves 11.  do{}while(0), register pins on the
 *    argument, a pointer alias, *buf or &buf[0] were all inert."
 *
 * Every one of that note's inert items measured inert here too, which is how the
 * lead was found: the park's residue is the same mechanism at a smaller scale.
 * The inline parameter works because the array decays to a pointer at the CALL
 * SITE of the inline function, so the inlined body sees an ARGUMENT, and the
 * frame address is re-expanded at each site instead of being one expression cse
 * can common.
 *
 * AND IT HAS TO BE DONE AT BOTH SITES -- this is the batch's law exactly:
 *
 *     the call alone wrapped (GetRoster)      6 of 20, at [7,8,9,11,12,14]
 *                                            relocations now OK, prologue EXACT
 *                                            -- the residue MOVED to the loop
 *     the loop alone wrapped (Apply)         20 of 20, and 22 instructions
 *                                            -- a clear regression, and on its
 *                                               own it would be rejected
 *     BOTH wrapped  (this file)               0        LANDS
 *
 * One half reads as "no progress, just somewhere else" and the other half reads
 * as "much worse, two instructions long".  A one-at-a-time sweep rejects both.
 *
 * ------------------------------------------------------------------
 * KEPT FROM THE PARK, AND STILL TRUE
 *
 * THE POST-INCREMENT BELONGS IN THE CALL.  The ROM advances the pointer BEFORE
 * the call -- `ldrh r0,[r6] / mov r1,r7 / sub r5,#1 / add r6,#2 / bl`.  Written
 * `OvlFunc_973_20080a0(*b, lv); b++;` gcc emits the increment after the call;
 * written `OvlFunc_973_20080a0(*b++, lv)` the increment moves ahead of it.
 *
 * NEW, AND THE ONLY OTHER THING THAT MOVED: the loop counter must be a SECOND
 * local, declared with an initialiser INSIDE the guarded block.  The ROM copies
 * the call result into r5 AFTER the branch (`ble / mov r6,sp / mov r5,r0`),
 * which a single `n` cannot do -- the copy out of the return register is emitted
 * by expand_call, before the test.  `{ int k = n; ... k--; }` gives `n` its own
 * short-lived pseudo that dies at the `cmp`, so it keeps r0 and its copy
 * vanishes, while `k` is born inside the block.  7 -> 6 and the whole loop
 * region goes exact.  The ORDER matters and the DECLARATION matters:
 * `{ int k; p = buf; k = n; ... }` is 20 of 20 at 22 instructions.
 *
 * ------------------------------------------------------------------
 * MEASURED INERT OR WORSE.  Forty-two of these came from one systematic cross
 * of six modifiers taken in pairs and triples; EVERY cell read 6 at exactly
 * indices [2,3,4,5,6,7], so the residue is flat in all six dimensions:
 *     A  OvlFunc_973_20080a0 declared `int` instead of `void`
 *     B  index loop `buf[i]` with i++ instead of the pointer walk
 *     C  the array inside a UNION, argument `&u`, walk `u.h`
 *     D  the loop pointer pinned `register unsigned short *p __asm__("r6")`
 *     E  the argument pinned `register void *a __asm__("r0")`
 *     G  `if (n <= 0) goto out;` instead of `if (n > 0) { }`
 * and, singly: `&buf[0]`, `buf + 0`, `(char *)buf`, `(void *)buf`, `&buf`,
 * a struct wrapper, the argument built in a nested block, the loop-invariant
 * forms `{ unsigned short *q = buf; q[i] }` and `{ q = buf + i; *q }`,
 * `p` declared inside the block, declaration order permutations of lv/n/p.
 * WORSE: `p = buf; n = f(p);` with p re-assigned in the block (11);
 * `while (n != 0)` with p before the loop (11); `while (n > 0)` (12);
 * `for (p = buf, k = n; ...)` (20 of 20, 22 insns); a dead `do{p=buf;break;}
 * while(0)` (11).
 * The two pins (D, E) are not merely inert, they are ERASED -- both compile to
 * byte-identical output with the plateau, because expand_expr forces the decayed
 * array address into a fresh pseudo before any copy to the pinned register, so
 * the pseudo cse records exists either way.
 */
extern int __Func_80796c4(void *buf);
extern void OvlFunc_973_20080a0(int id, int levels);

static inline int GetRoster(void *b)
{
    return __Func_80796c4(b);
}

static inline void RaiseEach(unsigned short *b, int n, int levels)
{
    do {
        OvlFunc_973_20080a0(*b++, levels);
        n--;
    } while (n != 0);
}

void OvlFunc_973_20080c0(int levels)
{
    unsigned short roster[0x10];
    int lv;
    int n;

    lv = levels;
    n = GetRoster(roster);
    if (n > 0)
        RaiseEach(roster, n, lv);
}
