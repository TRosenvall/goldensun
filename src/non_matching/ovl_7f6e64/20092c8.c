/* OvlFunc_969_20092c8 -- TRIAGE ONLY, NO CANDIDATE WRITTEN, NO objcmp FIGURE.
 *
 * Nothing was compiled for this function, so there is deliberately no `N of M`
 * line and no objcmp, aligncmp or shimcount figure anywhere in this file.  Do
 * not read one in.  Everything below is measured from the reference only.
 *
 * Source asm: asm/overlays/rom_7f6e64/ovl_314_c_a_c_c_a_c_c.s
 *
 * When a candidate exists, verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/ovl_7f6e64/20092c8.c \
 *     asm/overlays/rom_7f6e64/ovl_314_c_a_c_c_a_c_c.s --func OvlFunc_969_20092c8
 *
 * SPLIT SHAPE: NONE NEEDED.  One function in the .s (`grep -c
 * thumb_func_start` = 1), datacheck.py silent, shimcount.py reports no shims.
 * Whole-file conversion.  split_s.py not required and not run destructively.
 *
 * FRAME, ALL FOUR GREPS: GENUINELY FRAMELESS.
 *   sub sp,#imm 0 | (add|sub) sp,rN 0 | mov rX,sp 0 | add rX,sp,#K 0
 *   str rX,[sp] at offset 0: 0
 * All four are zero, so the brief's "frame: none" is CONFIRMED rather than a
 * 508-byte blind spot -- worth stating explicitly because batch 312 found two
 * functions whose frames were invisible to the un-corrected triad.  No
 * outgoing-argument space either, so no callee takes five or more arguments.
 *
 * WORK DENSITY 6.8%, NOT 4.9%.  Dataflow re-measurement (constant-origin set
 * per basic block): 1361 instructions = 860 argument fill + 363 calls + 92
 * WORK + 28 genuine copies + 13 branch + 5 prologue.  The brief's 4.9% and its
 * `mov rlo,rhigh` count of 25 both come from peephole tests; 28 is the genuine
 * non-constant copy count.
 *
 * IT IS A REBUILD FUNCTION, AND THAT IS THE OPPOSITE OF ITS BATCH-MATE 884.
 * Build-multiplicity screen (every `mov #imm8 / lsl #n` chain, by where the
 * value lands).  Only TWO values ever reach a callee-saved register, and BOTH
 * are also rebuilt elsewhere in the same function:
 *       0x8000  built THIRTEEN times -- 12 straight to an argument register,
 *               1 to r10
 *       0x3000  built 3 times -- 2 to an argument register, 1 to r8
 * Everything else (0x102 x5, 0x2000 x5, 0xe000 x5, 0xb000 x5, 0x10000 x3,
 * 0xd000 x3, 0xa000 x3, 0x6000 x3, 0x104 x3, and ~15 more) goes STRAIGHT TO AN
 * ARGUMENT REGISTER at every site.  So, unlike 884, there is almost nothing to
 * name: the lever here really is the blanket pin pass the batch brief
 * prescribes, with at most two partial long-lived constants.
 *
 * THIS IS WHY THE BRIEF'S HIGH-REGISTER COLUMN MISLEADS.  32 high-register
 * mentions and 25 `mov rlo,rhigh` look like heavy constant reuse, the same
 * reading that made 884 look like 969.  The high-register SETS partition says
 * otherwise: 4 are ADDRESSES (`=0x2013`, `=0x2014` loaded then moved to r8/r10
 * -- these are data addresses, not constants), 2 are multi-instruction
 * constants, 1 is a computed address.  BUILD MULTIPLICITY is the instrument
 * that separates hold from rebuild, and it is one pass over the reference.
 * See PARK_OvlFunc_884_20097c8.c, which turns that screen into eight named
 * constants and the batch's one real figure.
 *
 * POOLED MULTISET: 25 distinct, 55 load sites.
 *   20 numeric: 0x103 x8, 0xcccc x5, 0x101 x5, 0x105 x5, 0x2014 x4, 0x2013 x3,
 *     0x6666 x3, 0x1999 x2, and 0xbb, 0x22b, 0x345, 0x278e, 0x27ba, 0x4ccc,
 *     0x9015, 0x9999, 0xb333, 0x13333, 0x16666, 0x26666 once each.
 *   5 symbolic: OvlFunc_969_200a350, gScript_969__0200e004,
 *     gScript_969__0200e03c, gState, iwram_3001ebc.
 * THE EIGHT-BIT-MOVABLE SCREEN HAS EXACTLY ONE SITE: 0xbb, pooled once.  Per
 * batch 312 that is a SITE WORTH READING and NOT a symbol: `force_const_mem`
 * on a spilled constant pseudo explains it as well as a relocation does, and
 * both must be excluded first.  NO `.sym` ENTRY MAY BE WRITTEN FOR IT.  Note
 * also the mode gate -- check whether 0xbb is stored through a halfword or byte
 * destination before concluding anything, since `*(short *)x = 0xa` pools where
 * a reference emits `movs r3,#10`.
 *
 * CONTROL FLOW IS REAL, AND THERE IS A LOOP.  12 labels, of which THREE are
 * pool skips (the `b` at ref 406, 738 and 1146, each immediately before a
 * `.pool_aligned`).  That leaves NINE real branch targets:
 *   ref  75 / 84 / 93   three short forward `beq` skips, back to back
 *   ref 391-399         `.L16a2` with a BACKWARD `bls` -- a genuine LOOP, and
 *                       `bls` means the counter is UNSIGNED
 *   ref 652 + 659       `bne .L197c` then `b .L199c` -- an if/ELSE
 *   ref 679             `beq .L19b6`
 *   ref 731 + 738       `bne .L1a50` then `b .L1a70` -- a second if/ELSE
 *   ref 761             `beq .L1a8a`
 * So this is NOT the straight-line shape 884 turned out to be.  Two if/else
 * pairs and an unsigned-counter loop have to be read off branch polarity
 * before any argument fill matters, and docs/band-800plus.md section 1 says a
 * target with real branch structure wants the ordinary 500-instruction lever
 * set rather than straight-line constant-reuse material.
 *
 * NO FLAG MAY BE CITED FOR THE LOOP.  `-fno-rerun-cse-after-loop` is now
 * measured five ways with NO correlation to loop presence (two of the
 * byte-identical cases HAVE loops, the actively-worse case has two).  Sweep it
 * per function; having found a loop here is not evidence for it.
 *
 * COST, BY UNRESOLVED DRAFT LINES -- the axis that actually ranks this
 * population.  tools/draft_script.py resolves the mechanical part; what is left
 * is hand work:
 *       function  draft  calls  mem-ops  arity  raw-reg  label marks  HAND FIXES
 *       884         322    293      24      7       13        3            44
 *       969         457    363      80      0       37       12           117
 *       883         639    513     111     11       49       13           171
 *       896         717    556     143      2       69       16           214
 * 969 is 2.7x the hand work of 884 and carries a loop and two if/elses that
 * 884 does not.  That is why this batch spent its depth on 884 and left this
 * one triaged: the brief's own instruction-count ordering (1361 < 2126 < 2227)
 * agrees on rank but badly understates the gap to 884.
 *
 * NEXT.  Run the 884 pipeline in this order: resolve the two if/else polarities
 * and the loop bound FIRST (they gate every later argument fill), then a
 * BLANKET pin pass over the wide-literal sites -- and expect no named-constant
 * step, because build multiplicity says there is nothing held to name.  Do not
 * pin any site carrying one of the five symbols; on 884 that single exception
 * was worth 553 differing positions.
 */
