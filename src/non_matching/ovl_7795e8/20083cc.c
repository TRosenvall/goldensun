/* OvlFunc_880_20083cc -- NO CANDIDATE WRITTEN, NO objcmp FIGURE CLAIMED.
 *
 * TRIAGE ONLY, deliberately.  Nothing was compiled against this reference, so
 * there is no `N of M` line and no tool figure anywhere in this file.  Batch
 * 312 brief C spent its budget reaching a real distance on its other two
 * targets; the reading below is why this one should not have been grouped with
 * them, and it is the most consequential finding in the batch.
 *
 * Verify (once a candidate exists) with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/ovl_7795e8/20083cc.c \
 *     asm/overlays/rom_7795e8/ovl_30_c_c_a_a_c_c_c.s --func OvlFunc_880_20083cc
 *
 * ================================================================
 * THE TRIAGE SAID "FRAME: NONE".  THE FRAME IS 548 BYTES WITH SIX STACK
 * AGGREGATES, AND IT IS THE LARGEST AND HARDEST FRAME OF THE THREE.
 * ================================================================
 * The brief's table recorded `OvlFunc_880_20083cc` as frame **none**, against
 * 0x18 for 2008e5c and 0x4 for 200b1ac, and grouped all three as "the smallest
 * three of the 37 unattempted".  The frame reading is inverted.
 *
 * `grep 'sub sp, #imm'` finds NOTHING in this function, and that is not because
 * there is no frame -- it is because the frame is too big for the instruction.
 * Thumb-1 `sub sp, #imm` takes a 7-bit immediate scaled by 4, so its ceiling is
 * 0x1fc = 508 bytes.  This frame is 548.  gcc therefore builds it with REGISTER
 * ARITHMETIC:
 *
 *     prologue (lines 9-11)   ldr r5, =0xfffffddc   /   add sp, r5
 *     epilogue (997-1000)     mov r3, #0x89 / lsl r3, #2 / add sp, r3
 *
 * 0x100000000 - 0xfffffddc = 0x224 = 548, and 0x89 << 2 = 0x224 as well, so
 * both ends agree.  **A frame built this way is invisible to the `sub sp, #imm`
 * grep AND to an `add sp, #imm` grep, and the ONLY thing that finds it is
 * `add sp, <register>`.**  Any function in this tree whose frame exceeds 508
 * bytes reads as frameless to the triad as the brief states it.  That is the
 * grep to add, and it is one line:
 *
 *     grep -nE '(add|sub)[[:space:]]+sp,[[:space:]]*r[0-9]+' <ref.s>
 *
 * SIX STACK AGGREGATES, which is what actually makes this function hard:
 *     add r0, sp, #0x14   (91)      add r5, sp, #0x15c  (671)
 *     add r2, sp, #0x1c   (690)     add r3, sp, #0x1c   (915)
 *     add r1, sp, #0x18   (919)     add r0, sp, #0x18   (961)
 * Addresses of stack objects are formed at six sites, one of them at +0x15c
 * (offset 348), so there are local arrays or structs spanning most of those 548
 * bytes.  2008e5c and 200b1ac have NO aggregate at all between them.  Batch 311
 * established that the FRAME TRIAD predicts difficulty and length does not;
 * applied correctly it ranks this function hardest of the three by a wide
 * margin, and it is 6 instructions SHORTER than 2008e5c.
 *
 * NOTE THE TRIAD'S SECOND AND THIRD GREPS ARE MIS-ASSIGNED AS WRITTEN.  The
 * brief gives `mov rX, sp` plus `add rX, #K` as the aggregate test and
 * `add rX, sp` plus a load as the spill-versus-staging test.  In this function
 * `mov rX, sp` NEVER APPEARS, and `add rX, sp, #K` -- nominally the third grep
 * -- is the form every aggregate address takes.  So the aggregate test as
 * stated finds nothing here while the test it is paired against finds all six.
 * The aggregate form to look for is `add rX, sp, #K`; `mov rX, sp` is only the
 * special case K == 0.
 *
 * AND THE FOURTH CHECK, which the 2008e5c park also needed: this function has
 * TEN `str rX, [sp]` with ZERO loads from `[sp]` (lines 452, 558, 700, 714, 731,
 * 745, 837, 865, 879, 886).  Those are the fifth word of 5-argument calls --
 * OUTGOING ARGUMENT SPACE, not frame.  The genuine spills are the slots with
 * matching loads, and there are FIVE of them:
 *     [sp, #4]    3 str / 5 ldr      [sp, #0x10]  2 str / 4 ldr
 *     [sp, #8]    1 str / 1 ldr      [sp, #0x14]  2 str / 2 ldr
 *     [sp, #0xc]  1 str / 3 ldr
 * Pairing is the discriminator.  A bare `[sp]` with offset 0 forms no address,
 * so argument staging is invisible to all three greps as the brief states them.
 *
 * ================================================================
 * SPLIT SHAPE -- CLEAN, AND THE EASIEST PART OF THIS FUNCTION
 * ================================================================
 * TWO functions in the file (`OvlFunc_880_2008384`, 29 instructions, and this
 * one) and NO data section: `python3 tools/datacheck.py` on the reference is
 * SILENT, and `python3 tools/shimcount.py` reports no shims.  A plain two-way
 * text split, no exports needed.  `python3 tools/split_s.py
 * asm/overlays/rom_7795e8/ovl_30_c_c_a_a_c_c_c.s OvlFunc_880_20083cc --dry-run`
 * (quoted, NOT run destructively) reports:
 *
 *     [dry-run] would write ..._a.s  (1 function(s), 40 lines)
 *     [dry-run] would write ..._b.s  (1 function(s), 1020 lines)
 *     [dry-run] would REMOVE ..._c_c_a_a_c_c.s
 *     [dry-run] would rewrite overlays/rom_7795e8/overlay.ld
 *
 * so the target lands in `_b.s` and the install path is
 * src/non_matching/ovl_7795e8/20083cc.c.  Verify `make compare` green after the
 * split and BEFORE any C lands.
 *
 * ================================================================
 * THE POOLED-CONSTANT MULTISET, AND A CORRECTION TO THE BRIEF'S READING OF IT
 * ================================================================
 * The brief flags "48 distinct pooled values in 920 instructions, the highest
 * density here" and invokes the rule that a pooled eight-bit-movable word is a
 * RELOCATION rather than a literal.  The density figure is right.  The
 * inference needs splitting into three groups, because 21 of the 48 are already
 * symbols and need no inference at all:
 *
 *   SYMBOLS, 21 distinct, 36 loads -- visible AS symbols in the reference:
 *     gState x8, gKeyPress x5, iwram_3001f64 x2, iwram_3001ebc x2,
 *     iwram_3001ca0 x2, ewram_2002024 x2, gKeyHeld, gDebugMode, gLuckyFountain-
 *     style ewram_2000000/2001000/2002080/2002224, iwram_3001d08,
 *     iwram_3001e8c, REG_SIOCNT, OvlFunc_880_20081fc, OvlFunc_880_2008154
 *   LARGER LITERALS, ordinary pool material:
 *     0x206 x4, 0x205 x4, 0x109 x2, and x1 each 0x13f, 0x20f, 0x22a, 0x3e7,
 *     0x952, 0x1004, 0xc82, 0xc83, 0xc85, 0xc87, 0xea3, 0xf128, 0xf129,
 *     0xf301, 0xf30b, 0x927bf, 0x6002500, 0x6006000, 0xfffffddc (the frame)
 *   EIGHT-BIT-MOVABLE AND STILL POOLED -- the brief's lever's target set:
 *     =8, =7, =6, =4, =2, =0xa, =0xbe  (seven, one load each)
 *     plus ONE genuine explicit pool word: `ldr r3, .L860  @ 0x30` at 480 and
 *     `ldr r2, .L860  @ 0x30` at 485 -- two loads of a pooled 0x30.
 *
 * I CHECKED THE ASSEMBLER RATHER THAN ASSUMING IT, and it matters.  I first
 * reasoned that GAS collapses `ldr rX, =K` for 8-bit K into `mov rX, #K`, which
 * would have made that whole third group a transcription artefact and the lever
 * inapplicable.  THAT IS FALSE.  Assembling
 *     ldr r0, =8 / ldr r1, =0xbe / ldr r2, =0x101 / ldr r3, =0x30
 * with this tree's `arm-none-eabi-as -mthumb-interwork -mcpu=arm7tdmi` gives
 * four `ldr rN, [pc, #off]` and four `.word`s.  GAS collapses nothing, the
 * `=K` and explicit-label spellings are equivalent, and all eight sites above
 * are REAL pool loads.  So the lever's precondition holds at eight sites, which
 * is the most of any function in this batch -- and the 200b1ac park carries the
 * same correction, because I had written the false version into it first.
 *
 * WHAT THE LEVER CAN AND CANNOT CONCLUDE THERE.  `*thumb_movsi_insn` decides
 * pool-versus-`mov` on the VALUE, so gcc will not pool 2, 4, 6, 7, 8, 0xa,
 * 0xbe or 0x30 by that route, and something else put them there.  "Relocation"
 * is one explanation and reload's `force_const_mem` on a SPILLED constant
 * pseudo is the other -- and this function, with five real spill slots and a
 * 548-byte frame, is exactly where the second explanation is most available.
 * The 200b1ac park reaches the same fork from the other side and shows the
 * second branch is real, so these eight sites are NOT symbol evidence on their
 * own.  Only the relocation sequence can say which, and it has not been
 * measured.  DO NOT WRITE A .sym ENTRY ON THE STRENGTH OF THIS LIST.
 *
 * ================================================================
 * THE COMPARISON CENSUS -- THIS FUNCTION IS THE EQUALITY POPULATION
 * ================================================================
 *     bne 30  beq 26  ble 6  blt 1  bgt 1  bge 1   (65 `cmp`, 76 labels)
 * 56 of 65 comparisons are EQUALITY and only 9 are signed-relational, with no
 * unsigned compare at all.  Its sibling in this batch, `OvlFunc_951_2008e5c`,
 * is the mirror image: 46 signed-relational of 77.  The brief's warning was
 * right and is now measured on both sides -- an `!=`-everywhere rule imported
 * into 2008e5c would corrupt it at 46 sites, and a signed-relational habit
 * brought into THIS function would corrupt it at 56.  The census is one grep
 * and it must be run per function.
 *
 * By branch density (band-800plus section 1) this is the BRANCH-DENSE
 * population -- 76 labels, 65 compares, ~12 instructions per block -- so
 * section 2 does not apply to it and the ordinary 500-instruction lever set
 * should.  The same is true of 2008e5c (79 labels).  Only 200b1ac is
 * straight-line.  The brief's levers section leads with the straight-line
 * mechanisms (cse1 cross-call commoning, blanket pins, parked-constant counts),
 * which are the wrong set for two of its three targets.
 *
 * ================================================================
 * WHAT A CANDIDATE WILL NEED, in the order it will bite
 * ================================================================
 * 1. THE 548-BYTE FRAME AND ITS SIX AGGREGATES FIRST.  Until the local arrays
 *    and structs are declared at the right sizes, nothing downstream is
 *    measurable: every `add rX, sp, #K` offset is a function of the declaration
 *    order and size of the locals, and getting the SET wrong moves all six.
 *    Read the offsets (+0x14, +0x18, +0x1c, +0x15c) and the five spill slots as
 *    a layout problem before writing a single call.
 * 2. THE FIVE SPILL SLOTS make this the one function in the batch where
 *    band-800plus section 3's batch-311 narrowing applies: for a SPILLING
 *    function the ranking instrument is the spill-slot access-count table, not
 *    the aligned figure.  The reference's table is printed above.  Build the
 *    same table for the candidate and compare entry for entry; declaration
 *    order is the lever that moves it, and on `Anim_Gaia` it corrected a
 *    seven-encoding misassignment that the aligned figure averaged away.
 * 3. 139 `bl` and 36 symbol pool loads mean the constant-set check
 *    (band-800plus section 5) will gate correctness cheaply here too.  On both
 *    of this batch's reconstructed functions it caught every transcription
 *    error on the first compile and cost one `grep | sort | uniq -c`.  Run it
 *    before reading a single hunk.
 * 4. Only then the branch-dense lever set.  Do NOT start from the
 *    straight-line band levers.
 *
 * NO FLAG WAS SWEPT ON THIS FUNCTION, because there is no candidate to sweep.
 * No row, no figure, no claim.
 */
