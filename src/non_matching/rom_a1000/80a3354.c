/* Func_80a3354 (0x080a3354) -- asm/rom_a1000/rom_a1814_c_a_c_c_a_c_c_c_c_c_a.s
 *
 * NON-MATCHING, 20 of 58 stream encodings, ref 51 / ours 50 16-BIT
 *   INSTRUCTIONS, ref 128 bytes against ours 128 (a trailing `nop` absorbs the
 *   one-instruction gap, so the stream LENGTHS now agree and the figure is
 *   STILL PHASE).  aligned-equal 45 of 58, 18 differing/ins/del in 10 hunks.
 *   PINS 0 (tools/shimcount.py).  Device-free.  No split (piece.py: 1 function).
 *   BODY REPLACED IN BATCH 344, BRIEF A: 25 -> 20 encodings, 34 -> 45
 *   aligned-equal, and the loop preheader's register choice now matches.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_a1000/80a3354.c \
 *     asm/rom_a1000/rom_a1814_c_a_c_c_a_c_c_c_c_c_a.s --func Func_80a3354
 *
 * ============ BATCH 344, BRIEF A: WHAT CHANGED AND WHAT IS LEFT ============
 *
 * GAP ARITHMETIC, PER REGION (the number the next batch wants):
 *   prologue + the two calls      idx 0-11    ref 12 / ours 12   EXACT
 *   loop preheader                idx 12-16   ref  5 / ours  5   EXACT COUNT,
 *                                 one ADJACENT SWAP left (`movs r2,#3` against
 *                                 `adds r3,r7,r0`); the register the offset
 *                                 lives in is now the ROM's r0, which it was
 *                                 not before
 *   loop body                     idx 17-21   ref  5 / ours  5   EXACT, BYTE-EQUAL
 *   mid-section + CreateUIBox     idx 22-31   ref 10 / ours 10   EXACT, BYTE-EQUAL
 *   the 0x110/0x111 store pair    idx 32-40   ref  9 / ours  9   EXACT COUNT,
 *                                 scheduling order differs
 *   `b` over the pool + pool      idx 41-45   ref  5 / ours  5   EXACT, BYTE-EQUAL
 *                                 (0x1e, iwram_3001f2c, 0x111 -- same order)
 *   the 0x112/0x113 store pair    idx 46-53   ref  8 / ours  7   -1  <- the gap
 *   epilogue                      idx 54-57   ref  4 / ours  4   EXACT
 * SO THE GAP IS ENTIRELY IN THE LAST STORE and it is the ROM's
 * `adds r3, r7, r0` -- the materialised address for `g[0x113]`, which combine
 * folds away in ours into the register-offset form `strb r6, [r7, r2]`.
 *
 * WHAT THE NEW BODY BUYS, AND WHY.  Hoisting BOTH address computations above
 * the four byte stores --
 *     r = g + off;  off += 2;  s = g + off;
 *     g[0x88 << 1] = z;  *r = z;  g[0x89 << 1] = 8;  *s = two;
 * -- fixes the loop PREHEADER, which the batch-342 header had filed as a
 * separate "preheader-LUID" residue.  With both addresses born before the
 * stores, the preheader comes out as the ROM's
 *     movs r0,#0xa5 / lsls r0,r0,#1 / ldr r1,[pc] (0x1e)
 * -- offset in r0, exactly the ROM -- against the old body's r1.  The two
 * residues were ONE residue.  It also leaves the mid-function pool, its `b` and
 * all three pool words byte-identical, which the old body already had.
 *
 * THE FOLD: WHAT CAN BLOCK IT, WITH LINE NUMBERS, AND WHICH ROUTES ARE
 * DEVICE-FREE.  The park named the pass (combine) and the discriminator (the
 * offset's RTL code: CONST_INT at 0x110/0x111/0x112, a runtime pseudo at 0x113
 * from `off += 2`).  Both confirmed.  The address predicate is NOT a route --
 * `GO_IF_LEGITIMATE_ADDRESS` for Thumb accepts REG+REG for any
 * `GET_MODE_SIZE (MODE) <= 4` with both regs index-ok and neither the frame
 * pointer (arm.h:2133-2147), so `(mem:QI (plus pseudo pseudo))` is legitimate
 * and `recog_for_combine` matches.  The refusals that exist are all in
 * `can_combine_p`:
 *     combine.c:1081       DEST used after SUCC but before I3   (succ is 0 here)
 *     combine.c:1103-1108  `INSN_CUID (insn) < last_call_cuid && ! CONSTANT_P (src)`
 *                          -- a CALL between the add and the store
 *     combine.c:1160-1175  INSN itself volatile / an asm
 *     combine.c:1182-1188  any volatile insn BETWEEN insn and i3
 * The last two are the park's two instruments and both are devices.  THE CALL
 * ROUTE IS DEVICE-FREE AND WAS MEASURED: moving both address births above the
 * `_CreateUIBox` call (v/t4_f.c) DOES refuse the fold -- and costs ten
 * instructions, 61 against 51, because both addresses then live across the call
 * and need callee-saved registers the ROM does not spend.  That is a figure
 * ABOUT the blocker: the mechanism at combine.c:1108 is real and reachable from
 * source, and unaffordable here.
 *
 * AND SEPARATION ALONE DOES NOT REACH IT.  Putting unrelated insns between the
 * add and the store does not block combine, which works through LOG_LINKS and
 * not adjacency.  MEASURED (encodings of 58; 16-bit counts in brackets):
 *   v/t4_d  both addresses hoisted above all four stores ..... 20 [51/50] <- THIS FILE
 *   v/t4_e  addresses hoisted, `s` born after the 0x110 pair . 20 [51/50]
 *   v/t4_a  address for 0x113 born before the 0x112 store .... 21 [51/50]
 *   v/t4_b  the same with a second address local `s` ......... 21 [51/50]
 *   v/t4_c  as t4_d plus a repeated `g+0x20` store (SEMANTIC
 *           CHANGE, measured only as a control) ............. 25 [51/51]
 *   v/t4_f  both addresses born before _CreateUIBox .......... 67 [51/61]
 * FLAG AXIS RE-CROSSED AGAINST THE NEW BODY (a_flags.sh, all eight groups, as
 * the brief asks after a real source change): -fno-gcse, -fno-strict-aliasing,
 * -fno-strength-reduce, -fno-rerun-cse-after-loop, -fno-rerun-loop-opt all
 * INERT at 18/10; -O1 and -fno-schedule-insns2 both 27/14; -ffixed-r7 34/13 and
 * 6 encodings long.  NOTHING IMPROVES.
 *
 * NEXT, UNCHANGED FROM THE PARK AND NARROWED: find a spelling in which 0x113 is
 * a CONST_INT at combine time (so the fold has no legitimate address to make)
 * WHILE 0x111 stays the pooled word and 0x113 is rematerialised from it.  The
 * four constant-offset spellings the park measured recover the instruction and
 * invert the constant layout (pooling 0x113, deriving 0x111 from 0x110 by one).
 * The pass that performs THAT derivation is still unnamed and is still the one
 * thing worth a round; the park's elimination of cse's related_value chain
 * (cse.c:1633-1661 builds it only `if (GET_CODE (x) == CONST)`) stands.
 *
 * ---------------------------------------------------------------------------
 * EVERYTHING BELOW IS THE BATCH-334/342 HEADER, CARRIED FORWARD.  Two of its
 * statements are now superseded and are flagged where they appear: the
 * "preheader-LUID class" residue is closed by the body above, and the claim
 * that the address predicate might be made to reject `(plus reg reg)` is
 * refuted by arm.h:2133-2147.
 * ---------------------------------------------------------------------------
 */
/* Func_80a3354 (0x080a3354) -- NON-MATCHING.
 *
 * NON-MATCHING, 25 of 58 stream encodings  (RE-DERIVED, batch 342 brief C, on
 *   the body below -- UNCHANGED from the batch-334 body).
 *   16-BIT INSTRUCTION COUNTS ref 51 / ours 50, so this positional figure
 *   measures MISALIGNMENT and the honest statement is WE ARE ONE 16-BIT
 *   INSTRUCTION SHORT.  Read the counts before the figure.
 *   SIZE ref 128 bytes, ours 124.  RELOCATIONS IDENTICAL (objcmp rel=ok) --
 *   the batch-334 header did not say this and it matters: this is the only
 *   one of brief C's four targets whose relocations are clean at the base.
 *   PINS 0 (tools/shimcount.py reports no rows).
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_a1000/80a3354.c \
 *     asm/rom_a1000/rom_a1814_c_a_c_c_a_c_c_c_c_c_a.s --func Func_80a3354
 *
 * PIECE: ONE function.  tools/piece.py says `asm/rom_a1000/
 * rom_a1814_c_a_c_c_a_c_c_c_c_c_a.s -- 1 function(s)`, so this converts whole
 * with NO split.  (The reference was repointed by batch 340's split, which cut
 * Func_80a33d4 out to ..._b.s; that repoint is correct and verified here.)
 *
 * ===========================================================================
 * BATCH 342, BRIEF C: THE RESIDUE IS ONE **combine** SUBSTITUTION, AND THE
 * PARK'S NAMED DISCRIMINATOR IS WRONG.  TWO INSTRUMENTS RECOVER THE
 * INSTRUCTION; NO DEVICE-FREE SPELLING DOES YET.
 * ===========================================================================
 *
 * THE PARK'S NAMED REMAINING QUESTION was: "compare the two stores in
 * `.13.combine` and in `.19.flow2` and name the one that rewrites
 * `(set reg (plus base idx)) / (set (mem reg) v)` into
 * `(set (mem (plus base idx)) v)` for the dead-index site only."
 * DONE.  IT IS **combine**, pass 13, and the dumps are unambiguous.
 * `scratch_elev/b342/C/c_dumptail.sh <body> <ref> 13.combine 40`:
 *
 *   .12.life   (insn 130 ... (set (reg/v:SI 39) (plus:SI (reg/v:SI 32)
 *                                                        (reg/v:SI 38))))
 *              (insn 134 ... (set (mem:QI (reg/v:SI 39) 0) ...))
 *   .13.combine  (note 130 127 134 NOTE_INSN_DELETED 0)
 *              (insn 134 ... (set (mem:QI (plus:SI (reg/v:SI 32)
 *                                                  (reg/v:SI 38)) 0) ...))
 *              ;; Combiner totals: 54 attempts, 18 substitutions, 4 successes
 *
 * *** THE DISCRIMINATOR IS NOT "WHETHER THE INDEX REGISTER SURVIVES THE
 * STORE". ***  The park inferred that from the two named sites behaving
 * differently.  The real reason only the LAST site folds is the RTL CODE OF
 * THE OFFSET:
 *   - at 0x110, 0x111 and 0x112 the offset is a CONST_INT, so the address is
 *     `(plus reg (const_int N))` with N far outside QImode's 0..31 immediate
 *     field.  That is not a legitimate Thumb address, `recog_for_combine`
 *     cannot match it, and combine never had a fold available.  Those three
 *     sites are materialised FOR FREE, not by any lever.
 *   - at 0x113 the offset is a RUNTIME PSEUDO (reg 38, from `off += 2`), so
 *     the address is `(plus reg reg)`, which IS a legitimate Thumb QImode
 *     address, so combine folds and we lose the `add`.
 * So the park's batch-334 lever ("name the ADDRESS instead of the offset")
 * did not close site 0x111 either -- site 0x111 was going to materialise
 * whatever it was called, because its offset is a constant.  What the batch-334
 * edit actually bought was the 0x111 POOL WORD and the mid-function pool, which
 * is real and is kept.
 *
 * TWO INSTRUMENTS RECOVER THE MISSING INSTRUCTION, and both are recorded as
 * figures ABOUT THE BLOCKER, not as results:
 *
 *   1. A VOLATILE MEM AT THE LAST STORE ONLY --
 *        volatile unsigned char *vr;  ...  vr = g + off;  *vr = two;
 *      *** 24 of 58, LENGTH EXACT (dsize 0, ref 51 / ours 51), relocations
 *      clean. ***  `.13.combine` shows insn 130 SURVIVING and the combiner
 *      totals drop to "3 successes" -- exactly one fewer substitution.  The
 *      emitted tail is `add r2,r2,#2 / add r2,r7,r2 / ... / strb r6,[r2]`,
 *      i.e. the ROM's three instructions, AND the pool is now the ROM's own
 *      three mid-function words (`.word 30 / .word iwram_3001f2c / .word 273`)
 *      with the branch over them.  SO THE WHOLE STRUCTURAL GAP IS CLOSED BY
 *      THIS ONE SUBSTITUTION; the surviving 24 are register choice and sched2
 *      order (see below).
 *      NOTE THE PLACEMENT RULE: the volatile must be on a NAMED address
 *      variable.  `volatile unsigned char *vg = g; *(vg + off) = two;` is
 *      25 and 4 bytes short -- expand builds `(mem/v (plus vg off))` directly,
 *      so there is no separate address pseudo for combine to delete and the
 *      folded form is what expand emitted.
 *
 *   2. A MEMORY-CLOBBER ASM between the add and the store --
 *        r = g + off;  __asm__ volatile ("" ::: "memory");  *r = two;
 *      25 of 58 but LENGTH EXACT (dsize 0).  The instruction comes back and
 *      the barrier costs an ordering, so the figure does not improve.
 *
 *   AND THE NEGATIVE CONTROL HOLDS: a BARE `__asm__ volatile ("")` is 25 and
 *   STILL 4 BYTES SHORT.  That is batch 336's table reproduced exactly --
 *   `ASM_INPUT` reaches ready-list membership only, an `ASM_OPERANDS` is what
 *   reaches a combine substitution (rtlanal.c `volatile_insn_p`).
 *
 * WHAT IS LEFT AFTER THE INSTRUMENT, so the next agent knows the real target:
 *   ref   mov r0,#0xa5 / lsl r0,#1 / ldr r1,pool(0x1e) / mov r2,#3 / add r3,r7,r0
 *   ours  mov r1,#165  / lsl r1,#1 / add r3,r7,r1      / ldrh r1,pool / mov r2,#3
 * i.e. the loop PREHEADER's order (ROM: offset, stored value, counter, THEN
 * the address; ours: offset, address, value, counter) and the register the
 * offset lives in (ROM r0, ours r1).  Plus `mov r3,#0x89` sitting BEFORE our
 * `b` over the pool and AFTER the ROM's, and `add sp,#4` scheduled one slot
 * early.  This is the batch-339/340 preheader-LUID class.
 *
 * ===========================================================================
 * MEASURED THIS BATCH, EVERY ROW ref 58 / ours 56 AND ndiff 25 UNLESS NOTED
 * ===========================================================================
 * ON THE LAST STORE (the fold):
 *   `off2 = off + 2; r = g + off2;` (a second offset local) ........... 25
 *   `r = &g[off];` ................................................... 25
 *   `r = g; r += off;` ............................................... 25
 *   `h = g; r = h + off;` (a second base local) ...................... 25
 *   `gi = (int)g; r = (unsigned char *)(gi + off);` .. 24, +4 bytes, RELOCDIFF
 *   `*(unsigned char *)((char *)g + off) = two;` ..................... 25
 *   `off = (off + 2) | 0;` ........................................... 25
 *   `cr = &r; **cr = two;` (address through a pointer-to-pointer) .... 25
 *   `off += 2; g[off] = two;` (no address local) ..................... 25
 *   `r = iwram_3001f2c + off;` (fresh base pool load) . 22, +4 bytes, RELOCDIFF
 *   -- and FOUR spellings that turn the offset back into a CONSTANT, all of
 *   which recover the instruction (dsize 0) and all of which then lose the
 *   ROM's constant layout, pooling 0x113 and deriving 0x111 from 0x110 by one:
 *   `*(unsigned char *)(g + 0x113) = two;`  /  `r = g + off + 2;`  /
 *   `r += 2;`  /  `off = 0x113;`  .................... 47 each, dsize 0, RELOCDIFF
 *   ALL FOUR EMIT THE IDENTICAL STREAM.  *** THIS IS THE CROSSING THAT IS
 *   STILL OPEN: the MATERIALISATION needs a constant offset, the ROM's
 *   CONSTANT LAYOUT (0x111 pooled, 0x113 = 0x111 + 2) needs a runtime `off`,
 *   and no device-free spelling tried has both. ***
 *
 * ON THE LOOP PREHEADER (crossed against the base last store):
 *   named offset local `o = 0xa5 << 1; p = (short *)(g + o);` ........ 25
 *   counter before the pointer ....................................... 25
 *   named `int hv = 0x1e;` for the stored halfword ..... 25, -8 bytes, RELOCDIFF
 *   named `short hv = 0x1e;` ........................... 25, -8 bytes, RELOCDIFF
 *   both named, ROM's full order (o, hv, i, p) ......... 25, -8 bytes, RELOCDIFF
 *   both named, counter LAST ........................... 29, -8 bytes, RELOCDIFF
 * *** NAMING THE 0x1e IS WRONG AND THE PARK WAS RIGHT TO POOL IT. ***  It costs
 * EIGHT bytes -- the pool word and the branch over it -- taking us from 4 short
 * to 12 short.  The batch-340 piece-mate lever (name the loop-invariant
 * constant so it stops being a LICM hoist) is MEASURED INERT-TO-WORSE here.
 *
 * *** AND THE PARK'S `ldrh` VS `ldr` CLAIM IS A DIALECT ARTEFACT, NOT A
 * RESIDUE. ***  The batch-334 header says "the ROM loads the stored halfword
 * with `ldr` from a word pool entry and we load it with `ldrh` from the same
 * word ... Both pool; only the load width differs."  gas LOWERS a Thumb
 * PC-relative `ldrh` to `ldr` (there is no PC-relative `ldrh` encoding in
 * Thumb-1), so `ldrh r1, .L7` and `ldr r1, .La33b0` are THE SAME two bytes.
 * Verified by object comparison, not by reading the text -- see FINDINGS.md.
 * Do not spend a round on the load width.
 *
 * AUTOSWEEP (tools/autosweep.py, run FIRST; raw in scratch_elev/b342/C):
 *   BOTH CHEAP AXES CLOSED, nothing improved.
 *   FLAG AXIS: -fno-gcse 25, -fno-strict-aliasing 25, -fno-strength-reduce 25,
 *   -fno-rerun-cse-after-loop 25, -fno-rerun-loop-opt 25 (inert); -O1 37,
 *   -fno-schedule-insns2 37, -ffixed-r7 62 (worse).
 *   ANCHOR AXIS: 24 of 24 boundaries, NOT capped, all 25 or worse.
 *   CROSSED (c_flagcross.py): all eight groups re-run on the constant-offset
 *   body too -- still inert (47 everywhere except -O1 48, -fno-sched2 48,
 *   -ffixed-r7 67).  An anchor cannot reach this residue: it emits nothing, so
 *   it creates no ASM_OPERANDS and combine is not blocked (negative control
 *   above).
 *
 * NEXT, ONE QUESTION: find a device-free reason for `recog_for_combine` to
 * reject `(set (mem:QI (plus reg reg)) v)` at the last store, OR a spelling in
 * which 0x113 is a CONST_INT while 0x111 stays the pooled word and 0x113 is
 * rematerialised from it.  For the second half, the thing to identify is which
 * pass turns `(set r (const_int 273))` into `add r,r,#1` off the available 272
 * in the four constant-offset bodies above -- it is NOT cse's related_value
 * chain, which `cse.c:1633-1661` builds only `if (GET_CODE (x) == CONST)`,
 * i.e. only for a SYMBOL_REF plus an integer, never for a bare CONST_INT.
 *
 * ---------------------------------------------------------------------------
 * EVERYTHING BELOW THIS LINE IS THE BATCH-334 HEADER, CARRIED FORWARD.
 * ---------------------------------------------------------------------------
 *
 * NON-MATCHING, 25 of 58 encodings  (MEASURED batch 334, brief C, on the body
 *   BELOW -- see the batch-334 note further down for the arithmetic and for the
 *   body the superseded figure of TWENTY-SEVEN was measured on).
 *   COUNT DIFFERS (ref 58, ours 56) and the instruction count is ref 51 / ours
 *   50, so this positional figure still measures MISALIGNMENT and the honest
 *   statement is WE ARE ONE 16-BIT INSTRUCTION SHORT.  Read the count first.
 *   SIZE ref 128 bytes, ours 124.
 *   (The batch-319 recipe backfill measured TWENTY-SEVEN on a different body.)
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_a1000/80a3354.c \
 *     asm/rom_a1000/rom_a1814_c_a_c_c_a_c_c_c_c_c_a.s --func Func_80a3354
 *
 * This recipe was ADDED by the batch-319 backfill: the park had none, so
 * parkcheck.py could not report its figure and nothing had ever checked it.
 * Blocker class: address-birth placement in the loop setup.
 *
 * 54 lines against the ROM's 56, 27 differing, with the first eleven exact and
 * the whole loop body, the CreateUIBox call and its stack argument correct.
 *
 * The divergence is where the loop's base address is computed:
 *
 *     rom    mov r0,#0xa5 / lsl r0,#1 / ldr r1,=0x1e / mov r2,#3 / add r3,r7,r0
 *     ours   mov r1,#0xa5 / lsl r1,#1 / add r3,r7,r1 / ldr r1,=0x1e / mov r2,#3
 *
 * The ROM builds the offset, sets up the loop's stored value and counter, and
 * only THEN forms the address. Ours forms it immediately, which also costs the
 * register the ROM keeps the offset in.
 *
 * MEASURED, all 54 lines and 27 differing:
 *   the pointer assigned after `i = 3;`
 *   the offset named in its own local and the pointer assigned after `i = 3;`
 *
 * So the address-local birth-statement lever does NOT reach this one either.
 * That is now two negatives against it (see src/non_matching/ovl_797990/
 * 2008c1c.c for the first); the lever's demonstrated scope remains a pointer
 * COMPUTED from a base in straight-line code, not one feeding a loop.
 *
 * WHAT IS RIGHT: the pooled 0x1e. The ROM loads it from a mid-function pool
 * (`ldr r1, .La33b0 @ 0x1e`) for a halfword store while using `mov r2, #0x1e`
 * for the same value as a call argument a few instructions later -- the
 * halfword exception running in the ROM-pools direction, so the plain literal
 * is correct at both sites and naming either would be the error. The
 * decrement-first loop, the shared `2` between the stack argument and the last
 * byte store, and the derived 0x111/0x113 offset pair are all exact.
 *
 * NEXT: the two-line deficit is not yet attributed; the mid-function pool
 * makes tryc's line count unreliable here, so this wants a build-gated check
 * rather than more screening.
 *
 * ============== BATCH 334, BRIEF C: RE-PRICED AND ONE OF TWO CLOSED ==============
 *
 * The figure claimed at the top of this file was measured on the body below.
 * Its size is unchanged from the superseded body because the instruction gained
 * costs two bytes and the pad it displaces gave two back.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_a1000/80a3354.c \
 *     asm/rom_a1000/rom_a1814_c_a_c_c_a_c_c_c_c_c_a.s --whole
 *
 * ==================== THE DEFICIT IS REGISTER-OFFSET ADDRESSING ====================
 *
 * The previous header said "the two-line deficit is not yet attributed".  It is
 * attributed now, and it is not the loop setup at all -- the loop setup is a
 * BYTE difference, not a LENGTH one.  Both missing instructions were at the two
 * stores through the derived offset at the end of the function:
 *
 *     rom    ldr r0,=0x111 ... add r3,r7,r0 / strb r5,[r3]
 *                           ... add r0,#2 / add r3,r7,r0 / strb r6,[r3]
 *     parked                    strb r5,[r7,r2]
 *                           ... add r2,r2,#2 / strb r6,[r7,r2]
 *
 * The ROM MATERIALISES THE ADDRESS and uses an immediate-offset store; the parked
 * body uses Thumb's register-offset form and saves an instruction at each site.
 * docs/agbcc-thumb-regoffset-bug.md's scope note is the relevant text: gcc296
 * emits reg+reg freely and "the offset as a named local" is a recorded lever that
 * EARNS that form.  This park needs the INVERSE of that lever, and the inverse is
 * to name the ADDRESS instead of the offset:
 *
 *     r = g + off;  *r = z;        ->  add r3,r7,r2 / strb r5,[r3]
 *
 * That closed the FIRST site and took the figure from twenty-seven to the figure
 * claimed above, with the instruction count moving by one.  It is the brief's
 * "copy survives when its DESTINATION is a named variable" lever, measured on an
 * address rather than on a value.
 *
 * ==================== WHAT IS STILL OPEN, NARROWED TO ONE SITE ====================
 *
 * The SECOND site -- the last statement of the function -- refuses the same edit.
 * Naming its address changes NOTHING; the store stays `strb r6,[r7,r2]`.  Measured
 * this batch, every one at ref 58 / ours 56 with the instruction count at 50
 * except where noted, so these are flat and not merely equal-figure:
 *
 *   `s = g + off; *s = two;` with a SECOND pointer local .............. 25
 *   the same with the two pointer locals declared in the other order .. 25
 *   `s[0] = two;` instead of `*s = two;` .............................. 25
 *   `ad = (int)g + off; *(unsigned char *)ad = two;` .................. 25
 *   ONE pointer local reused at both sites ............................ 25
 *   `t = off + 2; s = g + t;` (the increment in its own local) ........ 25
 *   `*s = (unsigned char)two;` (narrowing the stored value) ........... 25
 *   `r = off + g` / `s = off + g` (operand order at both sites) ....... 25
 *   all FOUR stores through one named pointer ......................... 27, worse
 *   site TWO named and site ONE left as a subscript ................... 27, inert
 *   the 0x112 store moved AFTER the 0x113 store ....... 25, better aligned, wrong order
 *   both offsets written as literal constants, no `off` local ......... LENGTH EXACT
 *     at ref 58 / ours 58 -- but it relates 0x111 to 0x110 by one and pools 0x113
 *     separately, losing the ROM's mid-function pool and its branch over it.
 *     That last row is the most interesting negative here: the LENGTH is reachable
 *     by dropping the offset local, and the CONSTANT RELATION is reachable by
 *     keeping it, and no spelling tried this batch has both.
 *
 * >> NAMED REMAINING QUESTION: the two sites are spelled identically and differ
 *    only in that the first leaves `off` LIVE (it is incremented afterwards) while
 *    the second is the function's last use of everything.  So the discriminator is
 *    not the spelling of the address -- it is whether the index register survives
 *    the store.  The next thing to read is which pass performs the fold: compare
 *    the two stores in `.13.combine` and in `.19.flow2` and name the one that
 *    rewrites `(set reg (plus base idx)) / (set (mem reg) v)` into
 *    `(set (mem (plus base idx)) v)` for the dead-index site only.
 *
 * ALSO CORRECTED: the previous header's loop-setup diagnosis ("The ROM builds the
 * offset ... and only THEN forms the address") describes a real ORDER difference
 * but it accounts for ZERO of the length gap.  There is a second byte-level
 * difference in that region it did not name: the ROM loads the stored halfword
 * with `ldr` from a word pool entry and we load it with `ldrh` from the same
 * word.  That is the HImode/SImode pool contrast running in the direction the
 * previous header credited to the ROM, but on the LOAD WIDTH rather than on
 * whether the constant pools at all -- our value is HImode at the store, the
 * ROM's is a word.  Both pool; only the load width differs.
 *
 * THE BODY THE FIGURE OF TWENTY-SEVEN WAS MEASURED ON had, in place of the
 * `r = g + off; *r = z;` pair below, the plain subscripts `g[off] = z;` and
 * `g[off] = two;` with no pointer local and no `unsigned char *r` declaration.
 */
extern unsigned char *iwram_3001f2c;
extern void *Func_80a1814(void *g);
extern void Func_80a1870(void *q, int a, int b, int c, int d);
extern void *_CreateUIBox(int a, int b, int c, int d, int e);

void Func_80a3354(void)
{
    unsigned char *g;
    void *q;
    short *p;
    int i;
    int z;
    int two;
    int off;
    unsigned char *r;
    unsigned char *s;

    g = iwram_3001f2c;
    q = Func_80a1814(g);
    Func_80a1870(q, 2, 2, 8, 0);
    p = (short *)(g + (0xa5 << 1));
    i = 3;
    do {
        i--;
        *p = 0x1e;
        p--;
    } while (i >= 0);
    z = 0;
    *(int *)(g + 0x28) = z;
    *(int *)(g + 0x24) = z;
    two = 2;
    *(void **)(g + 0x2c) = _CreateUIBox(0, 0x11, 0x1e, 3, two);
    off = 0x111;
    *(int *)(g + 0x20) = z;
    r = g + off;
    off += 2;
    s = g + off;
    g[0x88 << 1] = z;
    *r = z;
    g[0x89 << 1] = 8;
    *s = two;
}
