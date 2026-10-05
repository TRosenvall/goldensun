/* Anim_Djinni -- 0x080de2f8, 696 ROM instructions (738 encodings).
 *
 * NON-MATCHING, 16 of 738 encodings differ.   [batch 310c: was 26]
 *
 * MEASUREMENT -- THIS COUNT IS A TRUE DISTANCE, NOT A SATURATED ONE.
 * SIZE IS EXACT and the instruction COUNT IS EXACT (ref 738, ours 738), so the
 * 16 is meaningful and ranks directly.  objcmp prints no SIZE line and the
 * first differing index is 107.  aligncmp ranks within that, SEPARATELY:
 *
 *     aligned-equal 730 of 738 = 98.9%,  15 differing/ins/del in 10 hunks
 *     (was 724 of 738 = 98.1%, 25 differing in 12 hunks)
 *
 * RELOCATIONS: 76 rows both sides, AND THEY NOW MATCH EXACTLY -- objcmp prints
 * no RELOCATIONS line at all.  Every callee, every pooled symbol, both
 * `__divsi3` sites, the five jump-table `.text` words and all four `_call_via`
 * veneers are present once, in the ROM's order, AT THE ROM'S OFFSETS, with the
 * ROM'S VENEER REGISTERS.  The two `_call_via_r5` / `_call_via_r6` rows that
 * were the whole recorded relocation discrepancy are closed -- see BATCH 310C
 * below.
 *
 * Verify with (the delivered park body, runnable as written):
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_c9000/Anim_Djinni.c \
 *     asm/rom_c9000/rom_dd2ac_c_c_c.s --func Anim_Djinni
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/aligncmp.py \
 *     src/non_matching/rom_c9000/Anim_Djinni.c \
 *     asm/rom_c9000/rom_dd2ac_c_c_c.s Anim_Djinni -v
 * Installed path is src/non_matching/rom_c9000/Anim_Djinni.c; substitute it for
 * the scratch path once this body replaces that file.
 *
 * ================================================================
 * BATCH 310C -- THE CLOSED BLOCKER, AND THE MECHANISM, WHICH IS NEW
 * ================================================================
 *
 * THE RECORDED BLOCKER WAS THE EPILOGUE, 12 ENCODINGS AND BOTH VENEER ROWS.
 * It is now 2 encodings and zero relocation rows.  The change is THREE TOKENS:
 *
 *     int clen = 0x80 << 7;          <-- a new local, INITIALISED AT ITS
 *                                        DECLARATION
 *     ...
 *     cl((void *)0x6004000, clen);
 *     cl(ctx, clen);
 *
 * > AN INT CARRIER INITIALISED AT ITS DECLARATION MAKES ITS PSEUDO LIVE FROM
 * > FUNCTION ENTRY, WHICH IS HOW YOU MAKE A QUANTITY LOSE A HARD REGISTER.
 * > allocno_compare's priority is log2(n_refs) * freq / LIVE_LENGTH, so a range
 * > that starts at entry is the LONGEST range and therefore the LOWEST
 * > priority.  The pseudo is allocated last, loses, and because its REG_EQUIV
 * > is a constant, reload REMATERIALISES it at each reference instead of
 * > spilling it.  That is exactly the ROM's form.
 *
 * Before: cse unified the two `0x80 << 7` argument constants into one pseudo,
 * that pseudo WON r5 and was held across `bl StopTask`, the function pointer
 * was pushed to r6, and both veneers came out `_call_via_r6`.  After: the
 * carrier loses, reload emits `mov r1,#128 / lsl r1,#7` at BOTH call sites, the
 * pointer takes r5, and both veneers are `_call_via_r5`.  Our epilogue is now
 * instruction-for-instruction the ROM's except for ONE transposition.
 *
 * THE PRECONDITION IS THE INITIALISER POSITION AND NOTHING ELSE -- measured:
 *   `int clen;` declared, then `clen = 0x80 << 7;` assigned in the epilogue
 *       -- 26, veneers still r6.  INERT.  This is why the recorded
 *       "`n << 7` off a block-scoped local" and "two separately-scoped locals"
 *       rows read inert: a body assignment keeps the range SHORT, which is the
 *       opposite of what is wanted.
 *   `clen = 0x80 << 7;` assigned before `StopTask(Task_BlitAnim)`  -- 26, r6.
 *   `clen = 0x80 << 7;` assigned before `StopTask(Func_80cd4b4)`   -- 26, r6.
 *   `int clen = 0x80 << 7;` initialised at declaration             -- 16, r5.
 * DECLARATION RANK IS FREE, measured: the carrier placed before `cl`, after
 * `cl`, and first in the whole declaration list all give 16 with relocations
 * exact.  It never reaches the frame, so it perturbs no spill offset.
 * THE SPELLING IS FREE: `0x4000` gives 16 as well.
 * A SECOND CARRIER IS INERT: `void *vram = (void *)0x6004000;` for the first
 * call's destination, declared either side of `clen`, gives 16.
 *
 * ISOLATED ON A SIX-FUNCTION PROBE FIRST (scratch_elev/b310c/probe1.c and
 * probe2.c), which is what made the rule legible: with the two clear calls and
 * one intervening `StopTask`, the carrier assigned AFTER the StopTask gives
 * r6, and the carrier initialised at declaration -- or assigned BEFORE the
 * StopTask -- gives r5.  Writing the two calls as direct calls (no pointer)
 * also materialises the constant once, so the shared pseudo is not an artefact
 * of the indirect call.
 *
 * ================================================================
 * BATCH 325H -- HUNK 1 IS A TRUE DEPENDENCE, NOT A SCHEDULING CHOICE, AND
 * THAT IS WHY SIXTEEN STATEMENT ORDERINGS WERE ALL FLAT
 * ================================================================
 *
 * Reproduced exactly: 16 of 738, ref 738 / ours 738, no SIZE line, no
 * RELOCATIONS line, first differing index 107.  aligncmp: 730 of 738 aligned,
 * 15 differing/ins/del in 10 hunks.
 *
 * THE PARK SAID of hunk 1: "the ROM's early placement is sched2 hoisting it
 * above the two frame STORES -- which it may only do if sched2's aliasing lets
 * a sp load cross a sp store.  No source statement order reaches it."
 * The CONCLUSION is right and the park is right to say stop spending compiles
 * on statement order.  The MECHANISM is the other way round, and it matters
 * because it says what WOULD move it.
 *
 * Compiled with `-da -fsched-verbose=6`, the post-reload RTL of this block
 * (basic block 16) carries the dependence explicitly:
 *
 *     (insn 1962 ... (set (mem:SI (plus (reg 13 sp) (const_int 28))  0 )
 *                        (reg:SI 2 r2)) ...          <- the d0 spill store
 *     (insn  264 ... (set (reg:SI 3 r3) (mem/s:SI (reg:SI 3 r3) 6 ))
 *                        ... (insn_list 1962 (insn_list 262 (nil)))
 *
 * -- insn 264, the `ldr r3,[r3]` that finishes the 0xbc chain, has a TRUE
 * dependence on insn 1962, the spill store.  The two alias sets are printed
 * right there: the spill MEM is **alias set 0** and the gPtrs load is alias set
 * 6.
 *
 * > A RELOAD SPILL SLOT CARRIES ALIAS SET 0, SO `DIFFERENT_ALIAS_SETS_P` CAN
 * > NEVER DISQUALIFY IT (alias.c:1573, and it is checked FIRST), and
 * > `memrefs_conflict_p` cannot disprove the overlap either, because one
 * > address is `sp+28` and the other is a pointer in a pseudo with no known
 * > base.  So every load after a spill store is ORDERED AFTER IT.
 *
 * sched2 therefore had no choice to make: once the d0 chain is expanded first
 * and spilled, the d1 chain's load is PINNED behind that store, and
 * `ldr r4,[sp,#0x28]`, `ldr r7,=0x7828` and `add r5` are pinned behind it in
 * turn.  The whole hunk is one forced ordering, which is exactly why all
 * sixteen statement orderings produced one RTL: the orderings change which
 * STATEMENT comes first, and reload still spills the first-expanded value
 * before the second chain's load.  `-fsched-verbose` is the wrong instrument
 * for this hunk; the RTL dependence list is the right one.
 *
 * WHAT WOULD MOVE IT, stated so it can be tested rather than re-swept: the
 * reference's first spill store is the 0xbc value (`str r3,[sp,#0x20]`,
 * ref idx 113) and its 0xb8 load (ref idx 116) comes AFTER it -- so the
 * reference has the SAME forced ordering, with the two chains exchanged.  Both
 * streams put the 0xbc chain in the pooled register r3 and the 0xb8 chain in
 * the reload copy `mov r2,r3`; the question is which chain reload copies.  That
 * is decided before sched2, in reload, by which value is live when the base
 * register is destroyed -- i.e. by the SPILL MAP (d1 at 0x20, d0 at 0x1c),
 * which the park correctly identifies as fixed by DECLARATION order and NOT by
 * assignment order.  SO I TESTED THE HALF THE PARK'S SIXTEEN ROWS ALL HOLD
 * CONSTANT -- the DECLARATION order -- and it is WORSE, which closes this
 * route rather than leaving it as a suggestion:
 *
 *     declare `d0` before `d1` (slots exchanged)                 19  (base 16)
 *     declare `d0` first AND assign `d1` first                   19
 *     assign `d1` first, declaration order unchanged             16  INERT
 *
 * all three at 738 / 738 encodings, size exact, relocations exact.  So the
 * spill map is not a lever either: exchanging the two slots costs 3 and moves
 * nothing in this hunk.  Hunk 1 now has NINETEEN measured orderings against it
 * and one forced RTL.  **The next move on it is not an ordering at all** -- it
 * is a form in which the d0 value is NOT spilled before the d1 chain's load,
 * i.e. one fewer live value across that block, and that is a question about the
 * whole function's locals, not about these two statements.
 *
 * HUNKS 2 AND 3 ARE THE SAME SHAPE AS Anim_CriticalHit'S FOUR ONE-SLOT HUNKS:
 * in each, ours schedules a `ldr rX,[pc,...]` pool load exactly ONE slot
 * earlier than the reference.  On Anim_CriticalHit the matching decision was
 * read off `.23.sched2` and is a pure INSN_LUID tie (`Ready list (t =193):
 * 2268 352` -> picks 352, equal priority 2, equal dependent count 1), and the
 * ladder's last rung prefers the LOWER LUID, so the reference's pre-sched
 * stream had the shift before the pool load.  `load_register_parameters`
 * (calls.c) emits register arguments in index order 0..N-1, which is ours;
 * reaching the reference's order means the r1 value must be materialised during
 * ARGUMENT EVALUATION rather than in the load phase.  That is the one lever
 * worth a compile on hunks 2 and 3, and it is the same lever on both functions.
 *
 * ONE CORRECTION TO THE BRIEF THAT SENT ME HERE.  It described this bank as
 * "155 landed sources ... many already landed" and asked for a landed-`Anim_*`
 * THE CLAIM BELOW THAT NO `Anim_*` SOURCES HAVE LANDED IS FALSE, and it came
 * from me, not from this park's author.  **148 `Anim_*`/`BaseAnim_*` functions are
 * defined in landed sources** under `src/rom_c9000/` against 52 parked.  The error
 * was a FILENAME check standing in for a DEFINITION check: every landed file in
 * that bank is split-named (`rom_XXXXXX_*.c`), so a scan for `Anim_*.c` finds
 * none of them.  `Anim_Hail` is landed in `Anim_Venus`'s own upstream module and
 * `Anim_Confuse` in `AnimEnd`'s; `tools/upstream_module.py <Func>` prints the
 * landed, parked and still-asm siblings of any function's module.
 *
 * A false NEGATIVE is the expensive direction, because it tells the next reader
 * not to look.  Treat the paragraph below as retracted.
 *
 * sibling port.  THERE ARE NO LANDED `Anim_*` OR `BaseAnim_*` SOURCES -- all
 * 155 `.c` files in `src/rom_c9000/` are split-named `rom_XXXXXX_*.c` and all
 * 46 named animations are parked.
 *
 * ================================================================
 * BATCH 325H -- THE .18.greg RELOAD TRIAGE: HUNK 1 SITS ON THE ONE INSN IN
 * THIS REGION WHERE A CURSOR READING IS LIVE
 * ================================================================
 * Triage (see Anim_CriticalHit's header for the source lines): one `Using reg`
 * per `Spilling for insn` block => `REG_ALLOC_ORDER` blamed CORRECTLY, act on
 * `spill_cost`; two or more, or inheritance => the round-robin cursor reading;
 * none => an ALLOCNO question.
 *
 * This function: 170 blocks -- 71 with no reload, 88 with one, 11 with two,
 * ZERO `Reusing reg`.  In hunk 1, insns 253, 255 and 264 have NO reload, and
 *
 *     Spilling for insn 246.
 *       Using reg 3 for reload 0
 *       Using reg 2 for reload 1
 *
 * -- insn 246 is `add r5,r4,r7`, and its two reload-fed operands are
 * `ldr r4,[sp,#0x28]` and `ldr r7,=0x7828`: PRECISELY the two instructions the
 * reference places early in this hunk.  So the two readings are not in
 * conflict, they divide the hunk: the alias-set-0 spill dependence above FIXES
 * THE ORDER OF THE TWO gPtrs CHAINS and cannot be moved, while the remaining
 * freedom in the hunk is the TWO reloads on insn 246, which is the one place
 * here the cursor reading applies.  That is the half to work next, and it is
 * NOT an ordering -- nineteen orderings have now been measured against it.
 *
 * ================================================================
 * THE REMAINING 16, BY HUNK -- ALL THREE ARE SCHEDULE ORDER, NO PROGRAM CHANGE
 * ================================================================
 *
 * HUNK 1 (12 encodings, ref[107:119]).  The `gPtrs` / `base + 0x7828` block.
 * CORRECTION TO THE OLD PARK BODY, WHICH WAS WRONG ABOUT THIS: the registers
 * are NOT exchanged.  Both streams put the 0xbc chain in the ORIGINAL pooled
 * register r3 and the 0xb8 chain in the copy r2, with the same `mov r2, r3`:
 *
 *   ref   ldr r3,=gPtrs / ldr r4,[sp,#0x28] / mov r2,r3 / ldr r7,=0x7828 /
 *         add r3,#0xbc / ldr r3,[r3] / add r5,r4,r7 / str r3,[sp,#0x20] /
 *         add r2,#0xb8 / ldr r2,[r2] / ldr r3,[r5] / ldr r0,[r3,#8] /
 *         str r2,[sp,#0x1c]
 *   ours  ldr r3,=gPtrs / mov r2,r3 / add r2,#0xb8 / ldr r2,[r2] /
 *         str r2,[sp,#0x1c] / add r3,#0xbc / ldr r3,[r3] / ldr r4,[sp,#0x28] /
 *         ldr r7,=0x7828 / str r3,[sp,#0x20] / add r5,r4,r7 / ldr r3,[r5] /
 *         ldr r0,[r3,#8]
 *
 * SAME THIRTEEN INSTRUCTIONS, SAME REGISTERS, SAME ROLES.  The ROM interleaves
 * the `base` reload, the 0x7828 pool load and `add r5` EARLY, between the two
 * gPtrs chains; we run the gPtrs chains to completion first.  `ldr r4,[sp,#0x28]`
 * is a RELOAD, so its RTL position is fixed by reload immediately before
 * `add r5,r4,r7`, and the ROM's early placement is sched2 hoisting it above the
 * two frame STORES -- which it may only do if sched2's aliasing lets a sp load
 * cross a sp store.  No source statement order reaches it; five further
 * orderings measured in this batch, ALL 16 and all relocations exact:
 *     d1 read before d0, slot first                     16
 *     d0, d1, then slot                                 16
 *     d0, slot, d1                                      16
 *     d1, slot, d0                                      16
 *     d1, d0, then slot                                 16
 * That is five on top of the eleven already recorded -- SIXTEEN orderings, one
 * RTL.  Do not spend another compile on statement order here.
 *
 * HUNK 2 (2 encodings, ref[381:383]).  `ldr r6,=gBuffer` and the `str r4,[sp,#8]`
 * that spills `-t` are transposed in the draw loop's preheader.  Unchanged.
 *
 * HUNK 3 (2 encodings, ref[704:706]).  ALL THAT IS LEFT OF THE OLD BLOCKER.
 * In the FIRST clear call the ROM puts `lsl r1,#7` before `ldr r0,=0x6004000`
 * and we put the pool load first:
 *   ref   mov r1,#0x80 / ldr r5,=Func_80008d4 / lsl r1,#7 / ldr r0,=0x6004000
 *   ours  mov r1,#128  / ldr r5,=Func_80008d4 / ldr r0,=0x6004000 / lsl r1,#7
 * The SECOND clear call is byte-exact, and so is the WHOLE equivalent block in
 * Anim_CriticalHit.  The discriminator read off both references: a POOL LOAD as
 * argument 0 is scheduled AFTER the `lsl`, a STACK RELOAD as argument 0 BEFORE
 * it -- Anim_CriticalHit's two calls take arg 0 from the stack then from the
 * pool and show both orders, and we match both there.  Here the pool load
 * follows `ldr r5`, so the ROM's choice is sched2 declining a second
 * back-to-back load; ours takes it.  Two encodings of sched2 state inside
 * reload-emitted code, downstream of hunks 1 and 2 -- not a source handle.
 *
 * SPLIT SHAPE: TEXT/DATA SPLIT, THREE WAYS, AND NINE NEW EXPORTS -- NONE OF
 * THEM FOR THIS FUNCTION.  `tools/datacheck.py` says
 * "Anim_Djinni reads no data label -> split needs NO new export", and
 * `tools/split_s.py ... Anim_Djinni --dry-run` REFUSES until nine `.global`
 * lines exist, all of them for the two functions that STAY IN ASM and lose
 * file-local access to the .rodata tail when it leaves with the _c piece:
 *
 *   asm/rom_c9000/rom_dd2ac_c_c_c_a.s  Anim_Thorn + Anim_Bolt   (asm as-is)
 *   src/rom_c9000/rom_dd2ac_c_c_c_b.c  THIS FILE
 *   asm/rom_c9000/rom_dd2ac_c_c_c_c.s  the .rodata tail
 *
 *   for Anim_Thorn:  .global .Leeba6  .Leebae  .Leebb6  .Leebb9
 *                            .Leebc0  .Leebc8
 *   for Anim_Bolt:   .global .Leebd6  .Leebe2  .Leebe6
 *
 * Twelve further labels in that tail (.Leeb48 ... .Leeb96) are ALREADY
 * `.global` from the split that landed Anim_Vine, so they need nothing.  DO NOT
 * emit any blob from C: they sit in the middle of a 21-blob run and moving one
 * out of the middle moves its address -- the rule Anim_Vine's landed note
 * states for .Leeb96 in this very file.
 *
 * stage1.ld names this object TWICE -- line 1876 `(.text)` and line 1976
 * `(.rodata)`.  The .rodata line must move to the _c piece; the batch-300
 * hazard is live here and was checked by grepping the script for the stem.
 * `split_s.py` was run ONLY with `--dry-run`.
 *
 * SHIMS: NONE.  `python3 tools/shimcount.py` reports zero rows for this file --
 * PIN-FREE, no barriers, no per-file flag override, no fakematch.txt row.
 *
 * ================================================================
 * THE ONE THAT MATTERS: THE SPILL-SLOT MAP IS A READOUT OF THE ROM'S
 * DECLARATION ORDER, AND THE REGISTER ALLOCATOR THEN FOLLOWS
 * ================================================================
 *
 * First candidate: 530 of 738, size and count ALREADY EXACT, relocations
 * already complete and in order.  Everything after that was ordering, and the
 * frame told me the order.  `sub sp, #0x6c` decomposes as
 *
 *   0x00-0x07  outgoing args        0x30-0x3b  v      (Func_80e3944 output)
 *   0x08       a loop temp (-t)     0x3c-0x47  v2     (the burst's sin input)
 *   0x0c       a compiler temp      0x48-0x53  delta
 *              (&delta)             0x54-0x5f  tgt
 *   0x10-0x2c  reload spill slots   0x60-0x6b  cur
 *
 * FRAME_GROWS_DOWNWARD IS IN FORCE IN THIS BUILD, so the FIRST-declared local
 * gets the HIGHEST address.  That is checkable against a landed file rather
 * than assumed: src/rom_c9000/rom_dd2ac_c_c_b.c (Anim_Vine, matching) declares
 * `vec3_t a; vec3_t b; int p1; int p2; DrawFn d[2];` and its generated .s puts
 * them at sp+36, sp+24, sp+20, sp+16, sp+8 -- declaration order, descending.
 *
 * So the five address-taken aggregates must be declared cur, tgt, delta, v2, v
 * -- highest address first -- and that was right on the first candidate.
 *
 * THE NEW PART IS THE SPILL REGION.  Reload calls `alter_reg` in increasing
 * PSEUDO number, pseudos are created by `expand_decl` in DECLARATION ORDER
 * after the parms, and the frame grows down -- so the spill slots at
 * 0x2c downward are the ROM's declaration order, one variable per slot, and
 * they can simply be READ OFF THE REFERENCE:
 *
 *   0x2c  the `sel` parameter (parms get pseudos before locals)
 *   0x28  base        0x1c  d0 (gPtrs[0x2e])
 *   0x24  ctx         0x18  base2 (iwram_3001eec[2])
 *   0x20  d1 (gPtrs[0x2f])   0x14  frames        0x10  a2 (the second actor)
 *
 * My first candidate declared base, ctx, base2, a2, d0, d1, frames and landed
 * them at 0x28, 0x24, 0x20, 0x1c, 0x18, 0x14, 0x10 -- SAME RULE, WRONG ORDER,
 * which is what confirmed the mechanism.  Reordering the declarations to
 * base, ctx, d1, d0, base2, frames, a2 was worth 530 -> 527 and moved the first
 * differing index from 18 to 29.  Small in encodings, decisive as a lever: it
 * pins every spill offset in the function, and nothing else can move after.
 *
 * > A SPILLED SCALAR'S FRAME OFFSET IS ITS DECLARATION RANK.  Read the offsets
 * > off the reference, sort them descending, and that IS the declaration list
 * > -- parameters first.  Doing this before touching registers is what made
 * > everything below measurable.
 *
 * ================================================================
 * THE LEVERS THAT PAID, IN ORDER, WITH FIGURES
 * ================================================================
 *
 * (1) THE ORACLE FIRST, AND IT IS WORTH MORE THAN ANY LEVER.
 *     src/rom_c9000/rom_dd2ac_c_c_b.c (Anim_Vine) is the SAME STEM, matching,
 *     and calls this function -- so its `Anim_Djinni(context, 4, f4, 4, &p1,
 *     &p2)` fixes the signature for free, including that the 5th and 6th
 *     arguments are the write-only `int *` at sp+0x8c and sp+0x90.
 *     src/rom_c9000/rom_d9ab8_c_c_c_c_c_b.c (Anim_Fireball, matching) is the
 *     same animation family and supplied, verbatim in shape: the
 *     `pp = g; base = *pp++; ctx = *pp;` ldmia idiom, `(int *)*_GetBattleActor
 *     (...)` with `extern int *_GetBattleActor(int)`, `extern void *gPtrs[]`
 *     with `gPtrs[0x2e]` / `gPtrs[0x2f]` (NOT `iwram_3001f0c`, which would pool
 *     the wrong symbol), `MatrixSetLook(cam, (char *)cam + 0xc)`,
 *     `sin(a) * amp >> 6` with the magnitude named SECOND, the explicit
 *     `(unsigned)(frame - K) <= N` range test, `w = C - (v.z - K) / 64` with
 *     `Data_ede48[w - 1]` and `w * 2` last, and `REG_BLDALPHA = 0x1010`
 *     written DIRECTLY -- no `int` carrier, which settles the recorded
 *     contradiction for THIS function by measurement on a matching sibling.
 *     First candidate on that base: 530 of 738 with size, count AND the whole
 *     relocation sequence already exact, for one compile.
 *
 * (2) TWO WALKING POINTERS WHERE I HAD ONE, TWICE OVER: 527 -> 56, i.e.
 *     471 encodings in a single edit, aligncmp 72.5% -> 94.0%.  THE LARGEST
 *     STEP ON THIS FUNCTION BY AN ORDER OF MAGNITUDE.
 *
 *     This function walks `gBuffer` twice (the 0x40-entry seeding loop before
 *     the frame loop, and the 0x20-entry draw loop inside it) and the
 *     `base + (0xe1 << 7)` array twice (the t == 0x40 reseed and the t > 0x3f
 *     sweep).  I gave each array ONE walker.  The reference gives the seeding
 *     loop r7, the draw loop r6, the reseed r7 and the sweep r5 -- FOUR
 *     quantities in three registers, which is not what one pseudo per array
 *     can produce.
 *
 *     THE TELL IS THAT A REGISTER APPEARS TWICE AND A THIRD VALUE DOES NOT
 *     FIT: reuse across the seeding loop and the reseed (both r7) is real and
 *     wanted -- disjoint ranges, one pseudo, the recorded reuse lever -- but
 *     the draw loop and the sweep each need a pseudo OF THEIR OWN, because
 *     they are live simultaneously with the accumulators around them.  The
 *     split is:
 *
 *       p   seeding loop AND the t == 0x40 reseed   (r7, inherited)
 *       b   the 0x20-entry draw loop                (r6, block-scoped)
 *       sw  the t > 0x3f sweep                      (r5)
 *
 *     Splitting only the draw loop was worth 3 (527 -> 526, 75.2%); splitting
 *     only the sweep was not measured alone; BOTH TOGETHER were worth 470 more.
 *     THAT IS THE RESULT: the two splits are not additive, they are a single
 *     allocation, and a walker split measured one at a time reads as inert
 *     when its partner is missing.  This is the sixth converse pair in the
 *     record and the first where the pair had to be applied SIMULTANEOUSLY.
 *
 * (3) `i = 0;` BEFORE THE `base + K` WALKER, at both reseed and sweep, plus
 *     `kind -= 4;` before `frames = 0x54;`, plus naming the second `sx`/`sz`
 *     pair in declaration order sz-then-sx while assigning sx first:
 *     56 -> 37, 94.0% -> 96.6%.  The first is Anim_Vine's recorded
 *     "assign the base + K pointer LAST"; the reference materialises `mov r8,
 *     rN` for the counter BEFORE the `adds r7, r2, r3` that forms the pointer,
 *     at both sites.  The sz/sx part is the declaration-rank rule again applied
 *     to two pseudos that never reach the frame: the reference puts the FIRST
 *     shift computed (sx) in the HIGHER register r5 and the second (sz) in r4,
 *     which is what declaring sz first and assigning sx first produces.
 *
 * (4) NAMING THE THIRD SHIFT: 37 -> 26, 96.6% -> 97.8%.  The velocity
 *     integration reads `-p->x >> 7`, `-p->z >> 7` and `-p->y >> 7`; the x and
 *     z values are used twice each (once in the add, once in the +-0x7ff range
 *     test) so they were locals from the start, and the y value is used ONCE
 *     and was inline.  Naming it anyway -- `sy` -- is worth eleven encodings.
 *     THE MECHANISM IS EXPAND POSITION, NOT ALLOCATION: inline, the `ldr
 *     [r6,#4]` is expanded inside the `p->vy = p->vy + ...` statement and so
 *     lands after the vx work; named, it is its own statement next to the z
 *     shift and sched2 pairs the two loads the way the ROM does.
 *
 *     > A ONE-USE SUBEXPRESSION CAN STILL WANT A NAME.  The usual rule is that
 *     > a value used twice is a local and a value used once is not; here the
 *     > name buys nothing in allocation and everything in EXPAND ORDER, which
 *     > is sched2's tie-break.  Worth trying wherever the residue is two loads
 *     > off one base appearing in the wrong order.
 *
 * (5) READING `d0` AND `d1` IN THE REFERENCE'S ORDER, i.e. `d0 = gPtrs[0x2e];`
 *     BEFORE `d1 = gPtrs[0x2f];` while DECLARING d1 first: objcmp stays at 26
 *     but aligncmp improves 722 -> 724 and the differing count 28 -> 25.
 *     The declaration order is fixed by the spill map (d1 at 0x20, d0 at 0x1c)
 *     and is NOT the same as the assignment order; separating the two is the
 *     point.  Kept because objcmp cannot distinguish it and aligncmp can.
 *
 * ================================================================
 * MECHANISMS READ OFF THE REFERENCE AND CONFIRMED BY THE FIRST CANDIDATE
 * ================================================================
 *
 *   - THE FRAME LOOP IS A `while`, NOT A GUARDED do-while.  Its entry test is
 *     `cmp r4, #0` against `frames` with t = 0 and its bottom test is
 *     `cmp r10, r1`: that is jump.c's `duplicate_loop_exit_test` on a `for`/
 *     `while`, exactly the recorded signature.  All FOUR inner loops are
 *     un-guarded do-whiles and were written as such.
 *
 *   - `cam = iwram_3001e80;` IS DECLARED INSIDE THE FRAME LOOP BODY, one per
 *     loop, and it is the recorded strongest lever doing its job on the first
 *     candidate: the reference re-loads `ldr r5, [r3]` off the pooled global at
 *     the TOP OF EVERY ITERATION and holds it in a callee-saved register across
 *     `_PlaySound` and `InitMatrixStack`.  cse can never reuse a global's load
 *     across a call, so only a pseudo can hold it, and only a pseudo declared
 *     in the loop body is that pseudo.
 *
 *   - `Func_80d6888(st->f8, 7, -1, -1, 0)` INSIDE `if (t == 0)` COMPILES THE
 *     LITERAL 0 OUT OF THE COMPARED REGISTER.  The reference emits
 *     `str r1, [sp]` with r1 still holding t from the `cmp r1, #0`, while the
 *     structurally identical call inside `if (t == 0x18)` emits
 *     `mov r3, #0 / str r3, [sp]` and then `sub r3, #1` to build its -1.  That
 *     is cse's `record_jump_equiv` making t == 0 a known equivalence inside the
 *     then-block.  Writing the literal `0` at BOTH sites reproduces both forms;
 *     writing `t` at the first site would be a different program.
 *
 *   - THE TWO ANGLE ACCUMULATORS IN THE DRAW LOOP ARE loop.c GIVS, not
 *     accumulators, and the recorded `j *` rule gives them: the reference holds
 *     `-t * (j * 32 + 256)` in r7 and `t * (j * 32 + 256)` in r9, each
 *     incremented by a value RE-DERIVED IN THE LOOP (`ldr r4, [sp,#8]` for -t,
 *     `mov r1, r10` for t, then `lsl #5`), which is what a giv with a variable
 *     increment looks like.  `-t` occupies the sp+0x8 spill slot for exactly
 *     this reason.
 *
 *   - `j / 8` IS RE-COMPUTED, NOT NAMED.  The reference emits the signed-
 *     division correction `cmp r3,#0 / add r3,#7 / asr r3,#3` TWICE in the same
 *     loop body, once for the entry guard and once for `+ 0x18`.  A named local
 *     would emit it once.
 *
 *   - `(q->t >> 3) + 2` IS A SHIFT, NOT A DIVISION: `asr r0, #3` with no
 *     correction, in a block guarded by `q->t >= 0`.  gcc-2.96 does not derive
 *     non-negativity from that branch, so a `/ 8` here would add the
 *     correction.  The neighbouring `- w / 2` DOES carry its correction
 *     (`lsr r3, r0, #31 / add / asr #1`) and is therefore a real `/ 2`.  BOTH
 *     SPELLINGS IN ONE EXPRESSION, and the printed correction says which.
 *
 *   - THE HIGH HALVES: `((short *)q)[1]` and `((short *)q)[3]`.  Thumb-1 has no
 *     immediate-offset `ldrsh`, so the reference manufactures the offset --
 *     `movs r7, #2 / ldrsh r2, [r5, r7]` -- and that register-offset form is
 *     the tell for a signed short load at a non-zero constant offset.
 *
 *   - `bl _call_via_rN` OFF A POOLED SYMBOL ADDRESS MEANS THE SOURCE CALLED
 *     THROUGH A POINTER.  Three of them here: the palette copy
 *     (`Func_8001af8`), the two draw helpers out of `gPtrs`, and the epilogue
 *     clear helper (`Func_80008d4`).  Each needs a local function-pointer
 *     variable; a direct call would emit `bl Func_...`.
 *
 *   - `0xa0 << 19`, `0xef << 7`, `0xe1 << 7`, `0xf0 << 14`, `0x80 << 7` and
 *     `0x90 << 3` are written as shifts because gcc-2.96's thumb constant
 *     splitter emits `movs`+`lsls` for a shifted byte; the pooled ones
 *     (0x77b4, 0x77b8, 0x7784, 0x7824, 0x7828, 0x6004000, 0xffff, 0x7ff,
 *     0xffe, 0x27a, 0x1000, 0xa8, 0x1010) are not shifted bytes and reach the
 *     pool on their own.  0xa8 IS a shifted byte and pools anyway; it is left
 *     as a plain `0xa8 - t * 2` and is byte-exact, so nothing is owed there.
 *
 *   - `REG_BLDALPHA` twice, never `REG_BLDCNT`: 0x4000052 in this tree.
 *
 * ================================================================
 * MEASURED AND INERT -- UNTESTED IS NOT DISPROVED, BUT THESE ARE TESTED
 * ================================================================
 * All of the following compiled to BYTE-IDENTICAL output to the base, 26 of
 * 738 with the same 11-hunk aligncmp profile, i.e. the RTL never changed:
 *   - `slot = (State **)(base + 0x7828);` moved before, between and after the
 *     two `gPtrs` reads, and removed entirely in favour of writing
 *     `(*(State **)(base + 0x7828))->f8` out at both uses.
 *   - Anim_Fireball's explicit `ax`/`ay`/`az` locals for the three velocity
 *     sums instead of `p->vx = p->vx + sx;` -- cse normalises them.
 *   - `void **gp = gPtrs;` as a named base for the two pointer reads.
 *   - the epilogue length as `0x4000`, as `n << 7` off a block-scoped local,
 *     and as two separately-scoped locals.
 *   - `ClearFn cl;` declared early rather than last.
 * MEASURED WORSE: assigning `cl = Func_80008d4;` before the StopTask/gfree
 * sequence instead of after it -- 26 -> 28.  That is the COMPLEMENT of the
 * batch-310c fix and confirms its direction: lengthening the POINTER's range
 * lowers the POINTER, lengthening the CONSTANT's range lowers the CONSTANT,
 * and only the second is wanted.
 *
  *
 * ================================================================
 * BATCH 327B -- RE-DERIVED ONLY, AND ONE CORRECTION TO THIS HEADER
 * ================================================================
 * Baseline re-derived as installed: 16 of 738, counts exact (738 = 738), no
 * SIZE line and no RELOCATIONS line, first differing index 107.  The 16 stands
 * and no new measurement was made against it this batch -- hunk 1 has sixteen
 * orderings and one RTL against it, and hunks 2 and 3 are two encodings each of
 * sched2 state inside reload-emitted code.  Brief B's compiles went to the two
 * targets with an open cross.
 *
 * CORRECTION TO THIS HEADER, which repeats a claim the coordinator has since
 * retracted: "THERE ARE NO LANDED `Anim_*` OR `BaseAnim_*` SOURCES -- all 155
 * `.c` files in `src/rom_c9000/` are split-named `rom_XXXXXX_*.c` and all 46
 * named animations are parked."  **148 ARE LANDED.**  The claim came from
 * checking FILENAMES rather than definitions; every landed animation is in a
 * split-named file.  `tools/upstream_module.py Anim_Djinni` prints this
 * unprompted, and for this function it names three landed module-mates:
 *     src/rom_c9000/rom_dd2ac_b.c      [Anim_Growth]
 *     src/rom_c9000/rom_dd2ac_c_b.c    [Anim_Punji]
 *     src/rom_c9000/rom_dd2ac_c_c_b.c  [Anim_Vine]
 * against 0 parks in the module.  Run that tool before concluding no landed
 * sibling exists.
*/
#include "gba/types.h"
#include "gba/io.h"
#include "file_table.h"

typedef int (*DrawFn)(void *ctx, void *src, int x, int y, int w, int h);
typedef void (*CopyFn)(void *dst, void *src, int len);
typedef void (*ClearFn)(void *dst, int len);

typedef struct {
    int f0, f4, f8, fc, f10, f14, f18, f1c, f20;
    short ids[4];
} State;

typedef struct {
    int x, y, z, vx, vy, vz, t;
} Part;

extern void *iwram_3001eec[];
extern void *iwram_3001e80;
extern void *gPtrs[];
extern Part gBuffer[];
extern unsigned short Data_ede48[];

extern void AnimStart(int n);
/* GetFile comes from file_table.h */
extern int DecompressLZ(void *src, void *dst);
extern void LoadVFXFile(int id, void *dst, int a, int b);
extern int BuildDraw2DFuncEx(int idx, int a, int b, int c, int d);
extern int *_GetBattleActor(int id);
extern int Random(void);
extern int sin(int a);
extern int cos(int a);
extern void StartTask(void *fn, int arg);
extern void StopTask(void *fn);
extern void Task_BlitAnim(void);
extern void Func_80cd4b4(void);
extern void InitMatrixStack(void);
extern void MatrixSetLook(void *a, void *b);
extern void MatrixTranslatev(vec3_t *v);
extern void MatrixPush(void);
extern void MatrixPop(void);
extern void MatrixYaw(int a);
extern void MatrixPitch(int a);
extern void MatrixRoll(int a);
extern int Func_80e3944(vec3_t *in, vec3_t *out);
extern void Func_80e38b8(Part *p, int a, int b);
extern void Func_80e3908(Part *p, int a, int b);
extern void Func_80d6888(int id, int a, int b, int c, int d);
extern void _PlaySound(int id);
extern void WaitFrames(unsigned int n);
extern void gfree(int tag);
extern void Func_8001af8(void *dst, void *src, int len);
extern void Func_80008d4(void *dst, int len);

void Anim_Djinni(void *context, int kind, int mode, int sel, int *outx, int *outy)
{
    vec3_t cur;
    vec3_t tgt;
    vec3_t delta;
    vec3_t v2;
    vec3_t v;
    void **g;
    void **pp;
    unsigned char *base;
    void *ctx;
    DrawFn d1;
    DrawFn d0;
    void *base2;
    int frames;
    State **slot;
    int *a1;
    int *a2;
    void *f;
    int fid;
    int arg;
    int t;
    int i;
    Part *p;
    Part *q;
    Part *sw;
    CopyFn cp;
    int clen = 0x80 << 7;
    ClearFn cl;

    g = iwram_3001eec;
    pp = g;
    base = (unsigned char *)*pp++;
    ctx = *pp;
    base2 = g[2];
    *(State **)(base + 0x7828) = (State *)context;
    AnimStart(0);
    *(int *)(base + 0x77b4) = 0x18;
    *(int *)(base + 0x77b8) = 0;
    if (kind > 3) {
        kind -= 4;
        frames = 0x54;
    } else {
        frames = 0x40;
    }
    switch (kind) {
    case 0:
        fid = FILE_94;
        break;
    case 1:
        fid = FILE_92;
        break;
    case 2:
        fid = FILE_8e;
        break;
    default:
        fid = FILE_90;
        break;
    }
    f = GetFile(fid);
    cp = Func_8001af8;
    cp((void *)(0xa0 << 19), f, 0x80);
    f = (char *)f + 0x80;
    DecompressLZ(f, base);
    LoadVFXFile(FILE_73, base2, 0, 0);
    if (mode == 1) {
        BuildDraw2DFuncEx(0x2e, 7, 7, 7, 3);
        BuildDraw2DFuncEx(0x2f, 7, 7, 7, 2);
    } else {
        BuildDraw2DFuncEx(0x2e, 7, 7, 3, 3);
        BuildDraw2DFuncEx(0x2f, 7, 7, 3, 2);
    }
    slot = (State **)(base + 0x7828);
    d0 = (DrawFn)gPtrs[0x2e];
    d1 = (DrawFn)gPtrs[0x2f];
    a1 = (int *)*_GetBattleActor((*slot)->f8);
    a2 = (int *)*_GetBattleActor((*slot)->ids[0]);
    p = gBuffer;
    i = 0;
    do {
        int ang = Random() & 0xffff;
        int mag = (Random() & 0xff) + 0x80;
        p->x = 0;
        p->y = ((Random() & 0x1f) + 0x14) << 16;
        p->z = 0;
        p->vx = sin(ang) * mag >> 5;
        p->vy = 0;
        p->vz = cos(ang) * mag >> 5;
        p->t = 0;
        i++;
        p++;
    } while (i != 0x40);
    *(int *)(base + (0xef << 7)) = 2;
    *(int *)(base + 0x7784) = 0x4b;
    arg = 0x90;
    arg <<= 3;
    StartTask(Task_BlitAnim, arg);
    cur.x = a1[2];
    cur.y = 0;
    cur.z = a1[4];
    switch (sel) {
    case 0:
        tgt.x = a2[2];
        tgt.y = 0xf0 << 14;
        tgt.z = a2[4];
        break;
    case 1:
        tgt.x = a2[2];
        tgt.y = 0xf0 << 14;
        tgt.z = 0;
        break;
    case 2:
        tgt.x = a1[2];
        tgt.y = 0xf0 << 14;
        tgt.z = a1[4];
        break;
    case 3:
        tgt.x = a1[2];
        tgt.y = 0xf0 << 14;
        tgt.z = 0;
        break;
    case 4:
        tgt.x = 0;
        tgt.y = 0xf0 << 14;
        tgt.z = 0;
        break;
    }
    delta.x = (tgt.x - cur.x) / 0x28;
    delta.y = (tgt.y - cur.y) / 0x28;
    delta.z = (tgt.z - cur.z) / 0x28;
    t = 0;
    while (t != frames) {
        void *cam = iwram_3001e80;
        Part *b;
        int j;
        if (t > 0x4b) {
            REG_BLDALPHA = (0xa8 - t * 2) | 0x1000;
        }
        if (t == 8) {
            _PlaySound(0xd4);
        }
        InitMatrixStack();
        MatrixSetLook(cam, (char *)cam + 0xc);
        if ((unsigned)(t - 6) <= 0x27) {
            cur.x += delta.x;
            cur.y += delta.y;
            cur.z += delta.z;
        }
        MatrixTranslatev(&cur);
        if (t == 0) {
            Func_80d6888((*(State **)(base + 0x7828))->f8, 7, -1, -1, 0);
        }
        if (t == 0x18) {
            Func_80d6888((*(State **)(base + 0x7828))->f8, 0, -1, -1, 0);
        }
        j = 0;
        b = gBuffer;
        do {
            if (t >= j / 8 && b->t == 0) {
                int w;
                MatrixPush();
                switch (j & 3) {
                case 0:
                    MatrixYaw(t * (j * 32 + 256));
                    break;
                case 1:
                    MatrixPitch(-t * (j * 32 + 256));
                    break;
                case 2:
                    MatrixRoll(-t * (j * 32 + 256));
                    break;
                case 3:
                    MatrixPitch(-t * (j * 32 + 256));
                    MatrixRoll(-t * (j * 32 + 256));
                    break;
                }
                Func_80e3944((vec3_t *)b, &v);
                v.x >>= 1;
                MatrixPop();
                if (v.z <= 0xf9) {
                    v.z = 0xfa;
                }
                if (v.z > 0x27a) {
                    v.z = 0x27a;
                }
                w = 8 - (v.z - 0xfa) / 64;
                d1(ctx, (char *)base2 + Data_ede48[w - 1], v.x - w / 2, v.y - w, w,
                   w * 2);
                Func_80e38b8(b, 0x3c, 0);
                if (t >= j / 8 + 0x18) {
                    int sz;
                    int sx;
                    int sy;
                    sx = -b->x >> 7;
                    sz = -b->z >> 7;
                    sy = -b->y >> 7;
                    b->vx = b->vx + sx;
                    b->vy = b->vy + sy;
                    b->vz = b->vz + sz;
                    b->vx = b->vx * 62 / 64;
                    b->vy = b->vy * 62 / 64;
                    b->vz = b->vz * 62 / 64;
                    if ((unsigned)(sx + 0x7ff) <= 0xffe
                        && (unsigned)(sz + 0x7ff) <= 0xffe) {
                        b->t = -1;
                    }
                }
            }
            j++;
            b++;
        } while (j != 0x20);
        if ((unsigned)(t - 0x36) <= 0xf) {
            v2.x = sin(t << 10) << 2;
            v2.y = 0;
            v2.z = 0;
            Func_80e3944(&v2, &v);
            *outx = v.x;
            *outy = v.y;
            v.x >>= 1;
            d0(ctx, base, v.x - 0xa, v.y - 0x14, 0x14, 0x28);
        }
        if (t == 0x40) {
            i = 0;
            q = (Part *)(base + (0xe1 << 7));
            do {
                int ang = Random() & 0xffff;
                int mag = (Random() & 0xff) + 0x80;
                q->x = *outx << 15;
                q->y = *outy << 16;
                q->vx = sin(ang) * mag >> 6;
                q->vy = cos(ang) * mag >> 5;
                q->t = (Random() & 0xf) + 8;
                i++;
                q++;
            } while (i != 0x40);
        }
        if (t > 0x3f) {
            i = 0;
            sw = (Part *)(base + (0xe1 << 7));
            do {
                if (sw->t >= 0) {
                    int w = (sw->t >> 3) + 2;
                    d0(ctx, (char *)base2 + Data_ede48[w - 1],
                       ((short *)sw)[1] - w / 2, ((short *)sw)[3] - w, w, w * 2);
                    Func_80e3908(sw, 0x3c, 0);
                    sw->t = sw->t - 1;
                }
                i++;
                sw++;
            } while (i != 0x40);
        }
        *(int *)(base + 0x7824) = 1;
        WaitFrames(1);
        t++;
    }
    StopTask(Task_BlitAnim);
    gfree(0x2f);
    gfree(0x2e);
    StopTask(Func_80cd4b4);
    cl = Func_80008d4;
    cl((void *)0x6004000, clen);
    cl(ctx, clen);
    REG_BLDALPHA = 0x1010;
}
