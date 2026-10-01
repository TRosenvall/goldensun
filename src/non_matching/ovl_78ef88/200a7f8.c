/* OvlFunc_896_200a7f8 -- TRIAGE ONLY, NO CANDIDATE WRITTEN, NO objcmp FIGURE.
 *
 * Nothing was compiled, so there is deliberately no `N of M` line and no
 * objcmp, aligncmp or shimcount figure in this file.  Do not read one in.
 *
 * Source asm: asm/overlays/rom_78ef88/ovl_314_c_c_c_a_a.s
 *
 * When a candidate exists, verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/ovl_78ef88/200a7f8.c \
 *     asm/overlays/rom_78ef88/ovl_314_c_c_c_a_a.s --func OvlFunc_896_200a7f8
 *
 * SPLIT SHAPE: NONE NEEDED.  One function in the .s, datacheck.py silent,
 * shimcount.py no shims.  Whole-file conversion.
 *
 * THIS IS THE HARDEST OF BATCH 313's FOUR, NOT THE EASIEST.  The brief ranked
 * it first at 3.4% work density.  Dataflow re-measurement: 2227 instructions =
 * 1464 argument fill + 556 calls + 142 WORK + 42 genuine copies + 17 branch + 6
 * prologue, i.e. 6.4% and ONE HUNDRED AND FORTY-TWO work instructions against
 * 884's thirty.  By unresolved draft lines -- the axis that ranks this
 * population best -- it needs ~214 hand fixes against 884's 44:
 *       function  draft  calls  mem-ops  arity  raw-reg  label marks  HAND FIXES
 *       884         322    293      24      7       13        3            44
 *       969         457    363      80      0       37       12           117
 *       883         639    513     111     11       49       13           171
 *       896         717    556     143      2       69       16           214
 * The source of the brief's error is an ADJACENCY test for constant building:
 * these scripts interleave their fills, so `mov r1,#imm / mov r2,#imm /
 * mov r0,#imm / lsl r1,#8 / lsl r2,#7 / bl` has neither `lsl` adjacent to its
 * own `mov`, and 173 of this function's 196 `lsl` read as work to a peephole.
 * They are argument fill.  The dataflow pass (constant-origin set per basic
 * block) is what separates them, and it reverses the batch's whole ranking.
 *
 * FRAME, ALL FOUR GREPS:
 *   sub sp,#imm 1 (`sub sp, #8`) | (add|sub) sp,rN 0 | mov rX,sp 0 |
 *   add rX,sp,#K 0 | str rX,[sp] offset 0: TWO, with no matching load
 * The 8 bytes are outgoing argument space, so at least one callee takes five or
 * more arguments -- and unlike 884, which has ONE such call, there are two
 * such store sites here.  Nothing is spilled and there is no stack aggregate.
 *
 * EVERY WIDE CONSTANT IS REBUILT: ZERO ARE HELD.  Build-multiplicity screen
 * over all 47 distinct `mov #imm8 / lsl #n` values: NOT ONE is built once into
 * a callee-saved register.  Thirty are rebuilt more than once, heavily --
 * 0x8000 x20, 0x4000 x15, 0x102 x14, 0x2000 x12, 0xb000 x10, 0x6000 x9,
 * 0xc000 x9, 0xd000 x8, 0x5000 x8, 0x100 x7.
 * SO THE BRIEF'S STEP 1 IS RIGHT FOR THIS FUNCTION AND WAS WRONG FOR 884.  A
 * blanket pin pass over the wide-literal sites is the correct starting shape
 * here; there is nothing held to name, and the named-long-lived-constant step
 * that 884 needed has no subject.  The two functions sit at opposite ends of
 * the same axis and the brief's high-register column cannot tell them apart:
 * 896 has 42 high-register mentions and 884 has 37, yet 884 holds eight
 * constants and 896 holds none.  BUILD MULTIPLICITY IS THE DISCRIMINATOR and
 * it is one pass over the reference.
 *
 * WHAT THE 42 HIGH-REGISTER MENTIONS ARE, since they are not held constants:
 * the high-register SETS partition gives 6 computed ADDRESSES, 1 symbol
 * address, 1 imm8, 3 prologue and 2 unclassified -- addresses and short-lived
 * quantities, not the wide-constant cache that 884 keeps.  docs/elevation.md's
 * warning that the raw copy count "carries almost none" of the signal is
 * confirmed again: of the 29 `mov rlo,rhigh`, only 42 are genuine non-constant
 * copies tree-wide and the single largest group is `mov r0, r11` x8.
 *
 * POOLED MULTISET: 55 distinct, 143 load sites -- the largest of the four.
 *   54 numeric, 1 symbolic (iwram_3001ebc, loaded 5 times).
 *   Top multiplicities: 0x1d70000 x15, 0x101 x13, 0x1d7 x11, 0x4ccc x6,
 *   0x13333 x5, 0x2666 x5, 0x18b x5, 0x1999 x4, 0x9999 x4, 0xcccc x4,
 *   0xa009 x4, 0x19b x4.
 *   THE EIGHT-BIT-MOVABLE SCREEN IS INERT: smallest pooled numeric is 0x101,
 *   ZERO candidate sites, so no symbol spelling is in question here.
 *
 * A WALKED-BASE CANDIDATE THAT IS *NOT* ONE, AND A SHIFT RELATION THAT MIGHT
 * BE.  98 pairs of pooled values differ by 0x40 or less, which looks exactly
 * like a walked base -- but batch 312 established that POOL MULTIPLICITY is the
 * evidence and value adjacency is not, and the 0x101/0x105/0x107 and
 * 0x18b/0x19b/0x1a7 runs here are each pooled in their own right.  The pair
 * worth reading instead is 0x1d70000 (x15) against 0x1d7 (x11): that is one
 * value and the same value shifted left 16, the two most-loaded entries in the
 * pool, 26 load sites between them.  Whether gcc can be made to produce one
 * from the other -- rather than pooling both -- is the single highest-value
 * question this function poses, and it is a multiplicity fact available before
 * writing a line.  Note the mode gate when testing it: the pool-vs-`mov`
 * decision in *thumb_movsi_insn is made on the VALUE, and a shifted carrier
 * must be ADJACENT to its use or constant propagation pushes it back down.
 *
 * CONTROL FLOW: 16 labels, FIVE of them pool skips, ELEVEN real branches --
 *   bne at ref 111, 140, 554, 574, 994, 1424, 1512, 2022; beq at 194, 203, 2212.
 * All eleven are forward, none is a backward branch, so there is NO LOOP: the
 * polarity pattern (eight `bne`, three `beq`) is eleven if-skips.  No loop flag
 * may be cited for it, and `-fno-rerun-cse-after-loop` must be swept per
 * function regardless, since it is now measured five ways with no correlation
 * to loop presence in either direction.
 *
 * NEXT.  Take it LAST of the four.  When taken: blanket pin pass as step 1 (no
 * named-constant step), read the eleven branch polarities first, and test the
 * 0x1d7/0x1d70000 shift relation early because it is worth up to 26 load sites.
 * Do NOT pin any site carrying iwram_3001ebc -- on 884 the single unpinned
 * held-pointer site was worth 553 differing positions, and that is Lever 1's
 * pointer-versus-constant discriminator.
 */
