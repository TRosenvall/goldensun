/* Func_8091eb0 (0x08091eb0) -- NON-MATCHING.
 *
 * NON-MATCHING, 20 of 43 encodings  (RE-MEASURED, batch 323 brief E).
 *   COUNT DIFFERS (ref 43, ours 45); SIZE ref 100 bytes, ours 104;
 *   relocations are the same symbols at a shifted offset.  All three of those
 *   are ONE CAUSE and it is named below.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_8a000/8091eb0.c \
 *     asm/rom_8a000/rom_91584_c_c_a_c_a_a_a.s --func Func_8091eb0
 *
 * PINS: 0.  DEVICES: 0.  Default flags.  The body is UNCHANGED from the
 * previous park -- what batch 323 changed is the diagnosis, and it changed it
 * from "nothing source-level found" to a measured structural claim with a
 * figure of 5 behind it and an owner decision attached.
 *
 * ========================================================================
 * THE PREVIOUS VERDICT WAS WRONG IN BOTH HALVES
 * ========================================================================
 *
 * It read: residue 1, "a branch to the next instruction ... gcc-2.96's jump
 * optimiser normally deletes this"; residue 2, value-vs-address order; and
 * then, explicitly, "the pooled `ldr r2, =0x21` is NOT a `_CONST_*` symbol:
 * gcc pools that literal by itself here, so the pooled-small-constant tell
 * has an exception when the constant is stored through a halfword pointer at
 * a large derived offset" -- and "NEXT: nothing source-level found in six
 * probes."
 *
 * RESIDUE 1 IS NOT A DEAD JOIN BRANCH.  It is the branch gcc emits to jump
 * OVER A LITERAL POOL IT DUMPED BEFORE THE EPILOGUE.  One `-da` compile says
 * so, in <base>.c.26.mach:
 *
 *     ;; SImode fixup for i14; addr  0, range (0,1020): `iwram_3001ebc'
 *     ;; SImode fixup for i41; addr 36, range (0,1020): `gState'
 *     ;; HImode fixup for i50; addr 42, range (0,64): 0x21
 *     ;; Emitting minipool after insn 153; address 84
 *
 * A HImode pool fix has pool_range 64 (arm.md:4353, `*thumb_movhi_insn`
 * alternative 1 `mn`), so a reach of 64 bytes from address 42 cannot put the
 * pool after the epilogue.  gcc dumps it at 84 and emits `b .L` around it,
 * plus a 2-byte alignment `nop`.  THAT is the extra instruction, the extra
 * 4 bytes and the relocation shift -- all of it, one cause.
 *
 * RESIDUE 2's "EXCEPTION TO THE TELL" IS THE OPPOSITE OF WHAT THE BYTES SAY.
 * gcc does pool a HImode 0x21 -- reproduced.  But it cannot pool it WHERE THE
 * ROM'S WORD IS:
 *
 *     ROM pool order            iwram_3001ebc, gState, 0x21
 *     ours, HImode literal      0x21, iwram_3001ebc, gState
 *
 * add_minipool_forward_ref (arm.c:4817-4860) inserts each fix before the
 * first entry with a larger `max_address = fix->address + fix->forwards`.
 * The two symbols are SImode: 0+1020 and 36+1020.  A HImode 0x21 at address
 * 42 is 42+64 = 106, LOWER THAN BOTH, so it goes to the HEAD of the list.
 * For it to sort last it would need max_address > 1056 -- i.e. range 1020,
 * i.e. SImode.
 *
 * And an SImode const_int 33 can NEVER reach the pool: 0x21 is 8-bit, so
 * constraint `I` matches an earlier alternative of `*thumb_movsi_insn` than
 * the pool path `mi`, and recog takes the first match.  (Same mode-dependent
 * argument as the `_MSG_182` ruling in docs/owner-decisions.md.)
 *
 *   >> NO SPELLING OF A LITERAL 0x21 CAN PRODUCE THE ROM'S BYTES.  HImode
 *   >> pools it in the WRONG PLACE; SImode does not pool it AT ALL.  The only
 *   >> thing that yields an SImode pool fix of the value 33 is a symbol_ref
 *   >> going through force_const_mem.
 *
 * MEASURED, with a device symbol as the instrument -- `extern int _CONST_21;`
 * and `*(short *)(g + 0x1d6) = (int)&_CONST_21;`:
 *
 *     DEVICE FIGURE: 5 of 43, COUNT 43 == 43, SIZE 100 == 100.
 *
 * The pool word lands at offset 0x60, the ROM's position, third; the
 * branch-over-the-pool and the nop pad are gone.  The only pool difference
 * left is the word's own content (00000000 + an R_ARM_ABS32 against the ROM's
 * 00000021), which is what measuring a .sym symbol with objcmp --func always
 * looks like -- an absolute assignment resolves at link time and emits no
 * bytes.  That is the whole of the RELOCATIONS line.
 *
 * SO THE 20 DECOMPOSES:
 *     15  the 0x21 is a SYMBOL, not a literal  (idx 1, 14, 25, 26, 33-44)
 *      4  value-vs-address order at the store  (idx 15-18)
 *      1  the device's own unrelocated pool word
 *
 * ========================================================================
 * THE OWNER DECISION -- NOT PROPOSED, RECORDED
 * ========================================================================
 *
 * docs/owner-decisions.md standard 2 is evidence quality AND completion.  This
 * does not complete: 4 encodings remain.  So this is the _FILE_e4/_FILE_e5
 * case -- structural argument accepted and recorded, entry WITHHELD on the
 * completion test.  Recorded so nobody spends a round rediscovering it:
 *
 *   - value 33 = 0x21, pooled at 0x08091f10, no relocation;
 *   - a name would live in const.sym (it belongs to no identified id space),
 *     as _CONST_21, named by value;
 *   - the argument is the pool ORDER, not the pool's existence.  That makes it
 *     a DIFFERENT and STRONGER argument than every existing .sym entry:
 *     _MSG_b24 was declined because "gcc pools 0xb24 as a literal anyway, so
 *     the bytes are equally consistent with a plain literal".  Here the bytes
 *     are NOT consistent with a literal, in either mode, and the A/B is one
 *     compile apart.
 *   - it is a COUNTER-EXAMPLE to const.sym's own "KNOWN EXCEPTION TO THE
 *     TELL".  That header says a pooled small value meeting a halfword is the
 *     literal and needs no symbol (OvlFunc_881_200b8fc, Func_80b9470).  Both
 *     of those are halfword READS where the HImode word's position happened to
 *     be consistent.  This is a halfword STORE where the HImode position is
 *     measurably wrong.  The discriminator is the POOL ORDER, not whether a
 *     halfword is involved.
 *
 * ========================================================================
 * THE REMAINING 4, with the deciding rung read out of the dump
 * ========================================================================
 *
 *     rom    ldr r3, =gState / ldr r2, =0x21 / add r1, #0x5a / add r3, r1
 *            strh r2, [r3]
 *     ours   ldr r3, =gState / add r1, #0x5a / add r2, r3, r1 / ldr r3, =0x21
 *            strh r3, [r2]
 *
 * With the device in place, `-da -fsched-verbose=6` gives block 2 as:
 *
 *       insn  code  dep prio cost   blockage units
 *         41   173   0    6    2    core : 50 48     r3 = [.LC1]  (gState)
 *        137     5   0    5    1    core : 48        r1 = r1 + 0x5a
 *         48     5   2    4    1    core : 54 50     r2 = r3 + r1
 *         50   173   2    4    2    core : 54        r3 = [.LC2]  (0x21)
 *         54   180   2    2    2    core :           [r2] = r3
 *
 * INSN 41 AND INSN 48 BOTH BLOCK INSN 50, because the two pool loads were
 * given the SAME hard register r3 -- an output dependence from 41 and an
 * anti-dependence from 48.  That is what pins the value load after the
 * address.  The ROM keeps the value in r2 and accumulates the address in
 * place in r3 (`add r3, r1`), so neither edge exists.  It is register
 * assignment, not scheduling: -fno-schedule-insns2 reads 8, worse.
 *
 * MEASURED INERT, all exactly 5 -- thirteen spellings, a hard plateau:
 *   `*(unsigned short *)` for the store; a `(short)` cast on the value;
 *   `*(volatile short *)` on the store; a `short *h` address local;
 *   A UNION MEMBER (ALIAS SET 0) FOR THE STORE -- the bank's lever 5 is
 *   EXACTLY INERT here, which is the same boundary Func_8096ddc recorded;
 *   0x1d6 spelled `(0xbe << 1) + 0x5a`; `int g` instead of `unsigned char *g`;
 *   `short *g` with `g[0xeb]`; a second dedicated pointer local for this
 *   if-block; `(int)_CONST_21` from an array-typed declaration;
 *   `0x1d6 + (int)g`.
 * MEASURED WORSE: `g = gState; g = g + 0x1d6; *(short *)g = ...` (29) and the
 *   `+=` form (29) -- the previous park measured the same thing at 30 with
 *   the literal, so that observation holds; naming the value in an int local
 *   inside the if (20); naming it at the top of the function (47, 49 insns).
 *
 * A BODY MEASURED AT 5 DEVICE-FREE AND DELIBERATELY NOT SHIPPED.
 * `int v; v = 0x21;` as the FIRST statement, with `*(short *)(g+0x1d6) = v;`,
 * reads 5 of 43 -- but at 42 instructions and 96 bytes, because an SImode
 * 0x21 becomes `movs r3, #0x21` and the pool word disappears.  It has ONE
 * FEWER ldr THAN THE REFERENCE, so it fails the per-opcode memory screen: a
 * better figure obtained by doing less work than the ROM.  Kept as
 * scratch_elev/b323/E/p3_rejected_vtop.c so it is not re-found and believed.
 *
 * WHAT IS RIGHT: everything else, including both derived offsets -- gcc
 * rebuilds 0x1d6 as 0x17c + 0x5a and 0x1f4 as 0x19e + 0x56 by itself
 * (reload_cse_move2add, on r1 and r2), exactly as the ROM does, from the
 * plain offsets.  The `g = gState;` named base is required: without it the
 * offset folds into a pooled `gState+470`.
 *
 * NEXT: the `_CONST_21` ruling.  If it is admitted this is 4 away with the
 * mechanism of the 4 named above; if it is declined this park is at 20 and
 * the 15 is unreachable, which is worth knowing either way.
 *
 * ========================================================================
 * BATCH 330 BRIEF E -- figure unchanged, device figure REPRODUCED, and the
 * remaining residue is now bounded to exactly four instructions
 * ========================================================================
 *
 * Re-derived from scratch.  The pin-free body still reads twenty of
 * forty-three with the counts differing (thirty-five reference instructions
 * against thirty-six), so that number is still the minipool misalignment the
 * park names.  The device body reads five with the counts EQUAL at
 * forty-three, which confirms the park's split of fifteen plus four plus one.
 *
 * THE FOUR ARE EXACTLY FOUR INSTRUCTIONS, AND THEY ARE ADJACENT.  Laid side
 * by side the two streams agree everywhere except the four that follow the
 * gState pool load in the first guarded arm: the reference materialises the
 * VALUE into the second register and then accumulates the address IN PLACE on
 * the base, so its store reads `strh <value>, [<base>]`; ours computes the
 * address into the second register with a three-operand add and then loads the
 * value into the base's register, so its store reads the pair the other way
 * round.  Both streams use the same three-operand add encoding; the only
 * difference is WHICH pseudo reuses the base's hard register -- the address in
 * the reference, the value in ours.
 *
 * THE SECOND GUARDED ARM ALREADY GETS IT RIGHT, which is the useful control:
 * it accumulates in place on the base exactly as the reference does, and it
 * differs from the first arm in having NO COMPETING POOL LOAD.  So the cause
 * is local, it is a local-alloc hard-register choice between two pseudos in a
 * four-pseudo block, and it is not the store's spelling.
 *
 * MEASURED EXACTLY INERT -- all five, device figure unchanged, counts equal:
 * a second dedicated pointer local for the SECOND arm (the untried half of
 * the park's own asymmetry lever -- it is inert, so that lever is closed here
 * in both directions); the base block-scoped inside each arm; nested ifs
 * instead of the short-circuit; a short-pointer base with a halfword index.
 * REFUTES ONE PARK ROW: the park records "naming the value in an int local
 * inside the if" at twenty.  A `const int` named inside a BRACED BLOCK in the
 * same arm is EXACTLY INERT instead.  The park's row is true of a
 * function-scope int and false of a block-scoped const, so the row bounds
 * only the spelling it actually used.
 * MEASURED WORSE: dropping the named `e` and reading the iwram word twice,
 * COUNT at thirty-seven instructions.
 *
 * NEXT: the completion test the park is withheld on is FOUR INSTRUCTIONS OF
 * LOCAL-ALLOC, now exactly located.  Attack which pseudos are LIVE in that
 * arm, not how the store is written -- thirteen spellings from batch 323 and
 * five more here are a plateau.
 *
 * ========================================================================
 * BATCH 333 BRIEF B -- FIGURE REPRODUCED, AND THE MODE ARGUMENT GENERALISES
 * ========================================================================
 *
 * DIRECTION, STATED BACK: this park is OURS LONGER -- thirty-five reference
 * instructions against thirty-six, forty-three stream entries against
 * forty-five, one hundred bytes against one hundred and four.  So nothing is
 * missing from the source; gcc emits one thing extra.
 *
 * REPRODUCED EXACTLY, pin-free and device-free, at twenty of forty-three, and
 * the extra material is precisely what the park says it is: an unconditional
 * branch over a minipool dumped before the epilogue, plus a two-byte alignment
 * pad, plus the pool word for the stored halfword value sitting FIRST in our
 * pool where the reference has it LAST.  Laid side by side there is nothing
 * else: every other instruction aligns.
 *
 * ONE ADDITION TO THE MECHANISM, and it makes this park's argument stronger
 * rather than changing it.  The park reasons from pool_range sixty-four at
 * arm.md:4353 and from recog's first-match rule on `*thumb_movsi_insn`.  The
 * matching fact on the HImode side is the CONSTRAINT ORDER itself:
 * `*thumb_movhi_insn` (arm.md:4318-4320) constrains operand 1 as
 * "l,mn,l,*h,*r,I".  Alternative 1 is `mn`, and `n` matches ANY const_int;
 * the 8-bit immediate alternative `I` is alternative 5.  recog takes the first
 * match, so EVERY HImode const_int in Thumb -- zero included -- is recognised
 * as the memory alternative and goes to the minipool.  There is no HImode
 * constant that does not pool, which is why the park's "HImode pools it in the
 * WRONG PLACE; SImode does not pool it AT ALL" is exhaustive and not merely
 * the behaviour of this one value.
 *
 * TWO OTHER PARKS WERE RE-PRICED IN THIS BATCH BY THAT SAME RULE, in the other
 * direction -- src/non_matching/rom_a1000/80ae9f0.c and
 * src/non_matching/ovl_7fb4a8/20087b0.c were each ONE INSTRUCTION LONG because
 * gcc built an HImode zero and pooled it where the reference reused an SImode
 * zero already in a register.  Both halved on one named local.  So this park is
 * also the reference entry for the rule: when a stream is one instruction and
 * one pool word long, look for an `ldrh` from the pool before anything else.
 *
 * ALSO CHECKED AND WORTH RECORDING ALONGSIDE THE WITHHELD RULING: area.sym
 * already carries a name for the value this function stores.  That does NOT
 * make the device admissible -- the park's own argument is that the field at
 * this offset belongs to no identified id space, and the area field this bank
 * does identify is at a different offset in the same block -- but the next
 * reader should know the name exists before proposing it, and should bring
 * evidence about the FIELD, not about the value.
 *
 * NOTHING NEW TRIED.  Eighteen spellings across batches 323 and 330 are a
 * plateau and the park's NEXT is still right: the remaining four are
 * local-alloc, already located, and the question is which pseudos are LIVE in
 * that arm.
 *
 * ========================================================================
 * BATCH 341 BRIEF B -- FIGURE RE-DERIVED, BOTH CHEAP AXES CLOSED, AND THE
 * HImode-POOL MECHANISM INDEPENDENTLY REPRODUCED IN A DIFFERENT OVERLAY
 * ========================================================================
 *
 * RE-DERIVED FROM SCRATCH with tools/objcmp.py: **20 of 43**, ref 35
 * instructions against ours 36, 7 against 7 thirty-two-bit entries, 100 bytes
 * against 104.  So this is a PHASE figure, OURS LONGER, and the park's
 * decomposition stands.
 *
 * THE RELOCATION LINE IS AN ORDERING, NOT A DEFECT, and here it is in full so
 * nobody has to re-run it: the same six symbols appear in both, and the only
 * change is that `Func_808b320`'s R_ARM_THM_CALL moves from 0x4c to 0x5c while
 * the two R_ARM_ABS32 pool words move from 0x58/0x5c to 0x50/0x54.  That is
 * exactly the early minipool dump this park names, read off the relocation
 * table instead of the instruction stream.
 *
 * tools/autosweep.py RUN.  **BOTH CHEAP AXES ARE CLOSED** -- do not re-screen
 * them:
 *   FLAG AXIS, unchanged body: -fno-gcse, -fno-strict-aliasing,
 *     -fno-strength-reduce, -fno-rerun-cse-after-loop and -fno-rerun-loop-opt
 *     all TIE at 20; -O1 and -fno-schedule-insns2 are 22; -ffixed-r7 is 48.
 *   ANCHOR AXIS: all 8 top-level statement boundaries inert at 20.
 * The five tying flags are the crossing partners for any future source change.
 *
 * THE MECHANISM IS NOW REPRODUCED ON A SECOND, UNRELATED FUNCTION, which is
 * what this batch adds.  While working ovl_7b2078/2008388 (a different overlay,
 * a different symbol, a different pool) a DIAGNOSTIC variant stored a halfword
 * constant BELOW 256 instead of 0x895, and gcc produced precisely this park's
 * signature: a minipool dumped mid-function, an unconditional `b` over it, and
 * one extra instruction -- 54 instructions against 53 with the pool split into
 * two groups.  The HImode fix forces the early dump in both cases.  So the
 * park's "pool_range 64 at arm.md:4353 cannot reach past the epilogue" is not
 * a one-function reading; it is the behaviour of EVERY HImode pool fix in
 * Thumb, and the constraint-order argument batch 333 added (`*thumb_movhi_insn`
 * constrains operand 1 as "l,mn,l,*h,*r,I", so `mn` -- which `n` makes match
 * any const_int -- is reached before the 8-bit `I`) is the reason there is no
 * HImode constant that escapes it.
 *
 * NOTHING NEW TRIED ON THE REMAINING FOUR.  Eighteen spellings across batches
 * 323, 330 and 333 are a plateau, the park's NEXT is still right, and this
 * round's budget went to the three distance figures in the same brief.  What
 * this round DOES add for the next reader is the local-alloc rule it had to
 * derive for OvlFunc_common1_1354 in the same batch, because the four look like
 * the same shape: **local-alloc ties a destination pseudo to a SOURCE pseudO
 * THAT DIES IN THE SAME INSN**, which is why `add r2, r3, r1` (three-operand,
 * r3 preserved) appears where the reference has `add r3, r1` (two-operand,
 * destination = the dying base).  Attack whether the base DIES at the add --
 * it does not in ours, because the value load was given r3 -- rather than how
 * the store is written.

 * ========================================================================
 * BATCH 344 BRIEF D -- FIGURE RE-DERIVED, THE GAP LOCALISED TO ONE REGION,
 * THE ANCHOR AXIS CLOSED AT ALL BRACE DEPTHS, AND *** ONE HALF OF THIS
 * PARK'S OWN ARITHMETIC IS NOT SUPPORTED BY THE COMPILER'S DUMP ***
 * ========================================================================
 *
 * RE-DERIVED FROM SCRATCH: 20 of 43, ref 35 instructions against ours 36,
 * 7 against 7 thirty-two-bit entries, 100 bytes against 104, relocations the
 * same symbols shifted.  Unchanged from every previous re-derivation.
 *
 * THE PER-REGION SPLIT, which no previous round recorded.  Laid side by side:
 *     indices  0-13   IDENTICAL          (one pool-load displacement aside)
 *     the FIRST GUARDED ARM, 14-18       5 instructions against 5 -- EVEN.
 *         ref   ldr r3,=gState / ldr r2,=0x21 / add r1,#0x5a / add r3,r1 / strh r2,[r3]
 *         ours  ldr r3,=gState / add r1,#0x5a / add r2,r3,r1 / ldr r3,=0x21 / strh r3,[r2]
 *     the SECOND GUARDED ARM, 19-32      IDENTICAL
 *     the POOL/EPILOGUE region           ours +1 instruction (`b.n`) and +2
 *                                        bytes of alignment pad
 * >> THE ENTIRE LENGTH GAP IS IN THE POOL REGION.  The two guarded arms are
 * >> the same length; the four encodings there are ORDER, exactly as recorded.
 * >> Nobody should look for a missing or extra statement in the body.
 *
 * THE COMPILER'S DUMP, IN FULL, so nobody has to re-run it (`-da`, .26.mach):
 *     ;; SImode fixup for i14; addr  0, range (0,1020): `iwram_3001ebc'
 *     ;; SImode fixup for i41; addr 36, range (0,1020): `gState'
 *     ;; HImode fixup for i50; addr 42, range (0,64):   0x21
 *     ;; SImode fixup for i70; addr 66, range (0,1020): `gState'
 *     ;; Emitting minipool after insn 153; address 84
 *     ;;  Offset 0, min -65536, max  106 0x21
 *     ;;  Offset 4, min -65536, max 1020 `iwram_3001ebc'
 *     ;;  Offset 8, min -65536, max 1056 `gState'
 *
 * THE ORDER HALF OF THIS PARK'S ARGUMENT IS CONFIRMED VERBATIM.  gcc prints
 * the 0x21 entry at **Offset 0** with max 106 against 1020 and 1056 -- the
 * HImode fix sorted to the HEAD of the pool, where the ROM has it LAST, exactly
 * as add_minipool_forward_ref (arm.c:4817-4860) is read above.  That half is
 * exhaustive and it stands.
 *
 * *** THE RANGE HALF IS NOT SUPPORTED BY THESE NUMBERS. ***  This park says
 * "A HImode pool fix has pool_range 64 ..., so a reach of 64 bytes from address
 * 42 cannot put the pool after the epilogue.  gcc dumps it at 84 and emits
 * `b .L` around it ... THAT is the extra instruction."  Three measurements say
 * the premise is wrong:
 *   1. gcc's own limit for this pool is **max 106** (printed above), and gcc
 *      dumped at **84** -- twenty-two bytes INSIDE its own limit, so the limit
 *      is not what forced the position.
 *   2. Our code WITHOUT the early pool is 90 bytes (104 total, minus 12 bytes
 *      of pool and 2 of pad).  An end-of-function pool would start at 90, which
 *      is BELOW 106.
 *   3. THE REFERENCE'S OWN 0x21 LOAD IS `ldr r2, [pc, #60]` -- a sixty-byte
 *      displacement, INSIDE the HImode pool_range of 64, with its pool word at
 *      0x60 and the load at offset 32.  So a HImode fix demonstrably CAN reach
 *      a pool sitting past the epilogue in this function's layout.
 *
 * WHY THAT MATTERS, and it is a live lead that is NOT the owner ruling.  If the
 * early dump has a cause other than the range -- and the three numbers above
 * say it must -- then the LENGTH GAP MAY BE CLOSEABLE WITH THE HImode FIX LEFT
 * IN PLACE.  Close it and the residue becomes the pool ORDER (three words
 * permuted) plus the four local-alloc encodings: a real distance of roughly
 * seven or eight, instead of a phase reading of twenty, and MEASURABLE for the
 * first time.  That is worth a round whatever happens to `_CONST_21`.
 * The thing to read is `arm_reorg`'s barrier bookkeeping in
 * config/arm/arm.c -- which barrier it selected and why it did not use the
 * function's final one -- NOT another spelling of the store.
 *
 * tools/autosweep.py's ANCHOR AXIS, EXTENDED.  Batch 341 recorded "all 8
 * TOP-LEVEL statement boundaries inert at 20".  Swept again at EVERY BRACE
 * DEPTH, including inside both guarded arms -- eleven placements -- and all
 * eleven are inert at 20 except one at 23.  So the anchor axis is now closed
 * at all depths, not just the top level.
 *
 * SEVEN MORE SPELLINGS OF THE GUARDED STORE, MEASURED, none of which moves the
 * figure (twenty-five across five batches now -- a hard plateau):
 *     a `const int` value local in a braced block           20  (reproduced)
 *     a `short *` address local inside the arm              20
 *     an anchor between `g = gState;` and the store         20
 *     an anchor before `g = gState;`                        20
 *     the short-circuit written as nested ifs               20  (reproduced)
 *     `*(short *)(gState + 0x1d6) = 0x21;` with no `g`      38  WORSE, and it
 *         folds the offset into a pooled `gState+470` -- confirms the park's
 *         "the `g = gState;` named base is required"
 *     a plain `int w; w = 0x21;` value-first local          21  at 36 insns and
 *         42 encodings -- it LOSES a pool word (the SImode `movs r3,#0x21`
 *         shape the park already rejected as scratch_elev/b323/E/p3_rejected_vtop.c)
 *
 * NEXT, in priority order: (1) why `arm_reorg` dumped at 84 when its own limit
 * was 106 and the epilogue sat at 90 -- that is the length gap and it is NOT
 * the ruling; (2) the `_CONST_21` ruling, which decides the fifteen; (3) the
 * four local-alloc encodings, already located.
 */
extern int iwram_3001ebc;
extern unsigned char gState[];
extern int GetEncounterGroup(int encounterID, int group);
extern int GetFieldActor(int actorID);
extern void Func_808adf0(int a);
extern void Func_808b320(int a, int b);

void Func_8091eb0(int a, int b)
{
    unsigned char *g;
    int e;

    e = iwram_3001ebc;
    *(short *)(e + (0xbe << 1)) = GetEncounterGroup(a, b);
    if (a == 0x62 && b == 0) {
        g = gState;
        *(short *)(g + 0x1d6) = 0x21;
    }
    if (*(short *)(e + (0xcf << 1)) == 3) {
        g = gState;
        Func_808adf0(GetFieldActor(*(int *)(g + 0x1f4)) + 8);
    }
    Func_808b320(a, b);
}
