/*
 * Func_80a1a40 -- PlaceCursor -- PARK at 8 differing STREAM ENCODINGS of 57; 48
 * INSTRUCTIONS against 48 (lengths agree, so a DISTANCE).  SIZE equal.  The
 * RELOCATIONS line is the pool-order CONSEQUENCE, not a second blocker.  PIN-FREE.
 *
 * Verify with: docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work goldensun-build python3 tools/objcmp.py src/non_matching/rom_a1000/80a1a40.c asm/rom_a1000/rom_a1814_a_c.s --func Func_80a1a40
 *
 * PIECE, verified with tools/piece.py this batch: asm/rom_a1000/rom_a1814_a_c.s
 * holds TWO functions -- this one at 8 of 57 and Func_80a1ac0 at 67 of 123
 * (src/non_matching/rom_a1000/80a1ac0.c).  datacheck.py is silent, so the split
 * is pure text and THIS FUNCTION CUTS OUT ALONE; nothing here is hostage to the
 * mate at 67.
 *
 * Func_80a1a40 -- PlaceCursor -- 0x080a1a40, asm/rom_a1000/rom_a1814_a_c.s
 *
 * PARK, 8 differing encodings of 57.  RE-MEASURED batch 325, brief F (was
 * measured 8 of 57 in batch 322 and the figure reproduces exactly).  PINS: 0.
 * ref 128 bytes / 57 encodings / 53 instruction rows; ours 128 bytes / 57
 * encodings / 53 instruction rows; per-opcode memory profile IDENTICAL
 * (push 1, ldr 12, ldrb 2, ldrh 3, strh 3, strb 1, pop 2).  Relocations differ:
 * the same four symbols, three of them 4 bytes earlier -- a CONSEQUENCE of the
 * pool order, not a second blocker.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work goldensun-build python3 tools/objcmp.py src/non_matching/rom_a1000/80a1a40.c asm/rom_a1000/rom_a1814_a_c.s --func Func_80a1a40
 *
 * SPLIT SHAPE: a pure text split, two ways.  tools/datacheck.py
 * asm/rom_a1000/rom_a1814_a_c.s is silent (exit 0).  tools/split_s.py
 * asm/rom_a1000/rom_a1814_a_c.s Func_80a1a40 --dry-run:
 *     would write asm/rom_a1000/rom_a1814_a_c_b.s  (1 function, 69 lines)  <- this
 *     would write asm/rom_a1000/rom_a1814_a_c_c.s  (1 function, 148 lines)
 *     would REMOVE asm/rom_a1000/rom_a1814_a_c.s, would rewrite stage1.ld
 * Install path on a landing: src/rom_a1000/rom_a1814_a_c_b.c.  Exports: none --
 * .Laf294 and .Laf29d are already `.global` in
 * asm/rom_a1000/rom_a1814_c_c_c_c.s, so the __asm__-named externs link as-is.
 *
 * THE INSTRUCTION STREAM IS EXACT: both streams hold the same 53 instructions
 * and the function assembles to 0x80 bytes, the ROM's exact size, WITH the
 * literal pool before the epilogue and the `b` that jumps over it reproduced.
 * THE RESIDUE IS POOL WORD ORDER AND NOTHING ELSE: 0xffff sorts first in the ROM
 * and fifth here, which moves three pool words and five `ldr` pc-offsets.
 *
 * ============== THE MECHANISM, WITH THE DUMP THAT SHOWS IT ==============
 * `push_minipool_fix` (arm.c:5380) prints every pool fix into `<base>.c.26.mach`
 * in ascending `addr + range` (arm.c:4820).  For this body, verbatim:
 *
 *   SImode i14  addr  0 range (0,1020) `iwram_3001f2c'  -> 1020
 *   SImode i169 addr  4 range (0,1020) `iwram_3001e40'  -> 1024
 *   SImode i22  addr 18 range (0,1020) `*.Laf294'       -> 1038
 *   SImode i66  addr 36 range (0,1020) 0xffff           -> 1056
 *   HImode i71  addr 40 range (0,  64) 0x1ff            ->  104
 *   SImode i82  addr 54 range (0,1020) 0xfffffe00       -> 1074
 *   SImode i95  addr 66 range (0,1020) `*.Laf29d'       -> 1086
 *
 * The ROM needs key(0xffff) < key(0x1ff) = 104 with the 0xffff reference at
 * addr 36, so its `pool_range` must be under 68.  **HImode's 64 is the only
 * narrow range a CONSTANT can reach on Thumb**, and that is now read off every
 * candidate rather than assumed: `*thumb_movqi_insn` alternative 1 is constraint
 * `m` with NO `n` (arm.md:4634, :4638, range 32), `*thumb_zero_extendqisi2`
 * takes a `memory_operand` (arm.md:3155, range 32), and the ranges 60 and 32,32
 * belong to `*arm_zero_extendhisi2` and `extendsfdf2` -- ARM-only and float.
 * `*thumb_movhi_insn` alternative 1 is `mn`, range 64 (arm.md:4318, :4353),
 * which is why it is the one that accepts a `const_int`.
 *
 * ===== CORRECTION, batch 325: THE OLD HEADER NAMED THE WRONG PRODUCER =====
 * The previous header offered `widen_operand`'s
 * `gen_lowpart (SImode, force_reg (HImode, op))` as "the general recipe for a
 * narrow pool fix".  **widen_operand can never produce one.**  Its first branch
 * is `! no_extend || GET_MODE (op) == VOIDmode || SUBREG_PROMOTED_VAR_P`, and
 * EVERY CONST_INT has VOIDmode, so a constant operand always returns through
 * `convert_modes` and stays a constant.  This function's own `.02.jump` is the
 * counterexample: insn 71 is already `(set (reg:HI 60) (const_int 511))` BEFORE
 * the `(and:SI (subreg:SI (reg:HI 60) 0) (reg:SI 63))` at insn 76.
 *
 * THE REAL PRODUCER is `expand_binop`'s expensive-constant force_reg:
 *     optabs.c:636  if (CONSTANT_P (op0) && preserve_subexpressions_p () ...
 *     optabs.c:640  if (CONSTANT_P (op1) && preserve_subexpressions_p ()
 *                       && ! shift_op && rtx_cost (op1, binoptab->code) > 2)
 *                     op1 = force_reg (mode, op1);
 * with `mode` the binop's own mode.  `preserve_subexpressions_p` (stmt.c)
 * returns 1 OUTRIGHT when `flag_expensive_optimizations`, so it is
 * unconditionally true at -O2 and the loop/insn-count heuristic under it never
 * runs.  So the general recipe is:
 *
 *   ANY HImode binop -- and, ior, xor, plus, minus, mult; NOT a shift, which
 *   `shift_op` excludes -- with a constant operand of `rtx_cost > 2` emits
 *   `(set (reg:HI) (const_int))` and therefore a HImode pool fix at range 64.
 *
 * The `& 0x1ff` this body relies on is one instance of that, not of widen_operand.
 *
 * ============ WHY 0xffff SPECIFICALLY IS STILL UNREACHABLE ============
 * The old verdict stands, re-read in this gcc, at both levels:
 *
 *   TREE: `fold`'s `case BIT_AND_EXPR:` opens with
 *   `if (integer_all_onesp (arg1)) return non_lvalue (convert (type, arg0));`
 *   -- unconditional, no side-effect guard, and fold canonicalises the constant
 *   to arg1.  A HImode AND means a 16-bit result type, for which 0xffff IS all
 *   ones.  The same rule kills `| 0xffff` (returns the constant) and `^ 0xffff`
 *   (rewritten to BIT_NOT_EXPR).  C's integer promotions make every 16-bit
 *   arithmetic expression an `int` expression unless `convert_to_integer`
 *   SHORTENS it -- and the shortening path builds the narrow tree and calls
 *   `fold` on it, which applies the same rule.  MEASURED: the
 *   `unsigned short t; t = vx & 0xffff;` shape compiles to 44 instruction rows
 *   against the reference's 53 -- nine instructions GONE, which is the fold rule
 *   firing.  (scratch_elev/b325/F/work/b4.c)
 *
 *   RTL: `store_fixed_bit_field` (expmed.c) emits exactly ONE value mask,
 *   `mask_rtx (mode, 0, bitsize, 0)`, under
 *   `must_and = (GET_MODE_BITSIZE (GET_MODE (value)) != bitsize
 *                && bitpos + bitsize != GET_MODE_BITSIZE (mode))`.
 *   0xffff needs bitsize 16, and in a HImode unit bitsize 16 forces bitpos 0, so
 *   the second conjunct is false and no mask is emitted at all.  The ROM has TWO
 *   masks on the value (0xffff then 0x1ff), so the 0xffff is a SOURCE-level mask.
 *   The only other HImode binop that could carry 0xffff is add/sub/mult, and the
 *   ROM's instructions are `and r2, r5` and `and r3, r5` -- the opcode names the
 *   optab.
 *
 * REACHABILITY of the narrow fix in general (denominator printed, batch 322): of
 * 4,418 files under asm/ whose first line is `@ Generated by gcc 2.96`, 359 lines
 * are `ldrh rN, .L<pool>` carrying 108 distinct pooled values (0 x96, 511 x18,
 * -16384 x15, 1 x15, 31 x12, 1023 x10, 255 x8, ...).  **65535 is not among the
 * 108**, which is the corpus agreeing with the two bounds above.
 *
 * ========= THE INSTRUMENT THAT CLOSES THE ARITHMETIC (batch 325) =========
 * scratch_elev/b325/F/work/b3.c replaces both masks with
 * `unsigned short tx; tx = vx & 0xfffe;` -- the same shape with a mask that is
 * NOT all-ones, so fold leaves it and convert_to_integer shortens it to a HImode
 * AND.  `.26.mach` then reads a **HImode fixup at addr 36, range (0,64),
 * 0xfffffffe -> 100**, ahead of 0x1ff, with the five symbols after it: THE ROM'S
 * POOL ORDER EXACTLY.  As a figure it is worthless and is recorded only as a
 * figure ABOUT THE BLOCKER -- 47 of 57 at 132 bytes against 128 and 55
 * instruction rows against 53, because the HImode temp costs two instructions and
 * 0xfffe is the wrong value.  What it proves is that NOTHING BUT THE MODE OF THE
 * 0xffff LOAD stands between this body and a match.
 *
 * ============ WHAT WAS WON, and it is most of the function ============
 * Three levers took this from 47 to 0 on the instruction stream:
 *
 *   - THE BITFIELD IS WHAT PLACES THE POOL.  Writing the attribute merge by hand
 *     as `(attr & 0xfffffe00) | (v & 0x1ff)` lets convert_to_integer shorten the
 *     mask to 0xfe00.  An `int` local for the merge defeats the shortening and
 *     restores `.word 0xfffffe00`, but leaves both masks SImode, so the pool goes
 *     to the END of the function.  Declaring the field `unsigned short a16 : 9`
 *     makes store_bit_field emit the mask at HImode -- range 64 -- which drags
 *     the minipool UP and makes gcc manufacture the `b` over it.
 *   - AN EAGERLY-LOADED POINTER IS FIXED BY GIVING A LATER POINTER AN EARLIER
 *     LIVE RANGE.  Assigning `cur = st->cur;` immediately after `st` -- fourteen
 *     instructions before the ROM loads it -- makes the two conflict, forces st
 *     to r5, makes reload pick r6 rather than r5 as the low temp for the constant
 *     7, and that anti-dependence stops sched2 hoisting `ldr r6, [r5, #0x10]`.
 *     ONE STATEMENT MOVE: 12 to 2.
 *   - Splitting `a |= vx & 0x1ff;` into `vx &= 0x1ff;` as its own statement was
 *     worth 20 to 12 -- statement splitting, here on a mask chain.
 *
 * iwram_3001e40 must be `volatile`: the ROM keeps its address in r14, reloads
 * the value, and recomputes the `>> 1 & 7`.
 *
 * TRIED AND LOST, so nobody repeats them: a field read-back with a hand-written
 * merge (47); unsigned short locals, where PROMOTE_MODE gives lsl/lsr rather than
 * an and (41); int locals with an explicit &= 0xffff (25); win and cur as named
 * locals in six placements (20-30); the constant-as-destination spelling (20);
 * multiply-by-8 rather than a shift by 3 (20); compound accumulation (20); u16
 * locals or casts on the bitfield store (29-31); -fno-schedule-insns2, WORSE at
 * 31 -- sched2 is helping here; and a `unsigned short m; m = 0xffff;` variable
 * used TWICE, which puts the right value in the pool but combine promotes the
 * standalone `(set (reg:HI m) (const_int 65535))` to SImode, so the fix is SImode
 * at range 1020 and the order is unchanged -- exactly inert at 8
 * (scratch_elev/b322/H/work/a1.c).
 *
 * SO THE PARK STANDS.  It is a compiler question, not a spelling search: a HImode
 * `and` whose constant operand is 0xffff.  Both producers are closed with a file
 * and a line each, the narrow-range alternatives are enumerated and closed, and
 * the pool arithmetic is confirmed by an instrument that reproduces the ROM order.
 *
 * A NOTE FOR neighbour.py: it returned an 8-way tie on iwram_3001e40, all in a
 * different directory with different conventions.  Applying the N-way-tie rule to
 * iwram_3001f2c instead gave ~40 same-directory siblings, one of which already
 * declared the exact struct this function needs.  SUGGESTED REFINEMENT: rank a
 * shared global by how FEW files use it, and prefer same-directory hits.
 *
 * ========== BATCH 330, brief F: THE BOUND SURVIVES, ITS CAUSES DO NOT ==========
 *
 * The park's conclusion -- that this is a compiler question and not a spelling
 * search -- reproduced and stands.  Three of the statements underneath it are
 * wrong or incomplete, and the corrections move where the next attempt should
 * aim, so they are what is recorded here.
 *
 * (1) THE PATTERN THAT OWNS A POOL RANGE OF SIXTY IS A THUMB PATTERN, NOT AN
 * ARM ONE.  This park reads "the ranges 60 and 32,32 belong to
 * `*arm_zero_extendhisi2' and `extendsfdf2' -- ARM-only and float".  Range sixty
 * belongs to `*thumb_zero_extendhisi2' (arm.md:3030-3069), whose condition is
 * TARGET_THUMB, and which prints a plain WORD load -- not a halfword load --
 * whenever its memory operand is a LABEL_REF (arm.md:3038-3040), i.e. exactly
 * in the literal-pool case.  `*arm_zero_extendhisi2' is the NEXT pattern
 * (arm.md:3071-3080) and its range is two hundred and fifty-six.
 *   THE CONCLUSION SURVIVES ANYWAY, for a reason the park did not give: that
 *   pattern's operand one is a `memory_operand', and `note_invalid_constants'
 *   (arm.c:5424-5452) pushes a fix only for an operand that is CONSTANT_P in a
 *   memory-ok alternative, or for a MEM whose address is a SYMBOL_REF with
 *   CONSTANT_POOL_ADDRESS_P.  A const_int can never satisfy `m', so the pattern
 *   cannot carry one.  `*thumb_movhi_insn' alternative one (arm.md:4318, :4353)
 *   really is the only narrow carrier a constant can reach -- but check the
 *   PATTERN next time, not the range.
 *
 * (2) A THIRD PRODUCER OF A NARROW MASK EXISTS AND IS CLOSED BY ARITHMETIC.  The
 * park enumerates two producers (store_fixed_bit_field's value mask, and
 * expand_binop's expensive-constant force_reg).  There is also
 * `extract_fixed_bit_field's unsigned-field mask, expmed.c:1621-1637 -- the one
 * a bitfield READ goes through.  It cannot help, twice over: the clause that
 * would fire it on a mode change, `|| tmode != mode', sits inside an `#if 0'
 * (expmed.c:1625-1631), so the only live guard is
 * `GET_MODE_BITSIZE (mode) != bitpos + bitsize', which is FALSE for a sixteen-bit
 * field at bitpos zero of a HImode unit -- no mask is emitted at all; and when
 * the guard does fire, op0 has already been converted to tmode at expmed.c:1619,
 * so the `expand_binop (GET_MODE (op0), ...)' that follows is SImode.  So no
 * bitfield read, of any width, in any unit, can put this constant in a narrow fix.
 *
 * (3) THE CAUSE OF THE MODE LOSS IS THE USE, NOT THE DECLARATION -- AND THIS IS
 * THE CORRECTION THAT MATTERS.  The park records a named halfword mask used
 * twice as inert and explains it as "combine promotes the standalone HImode set
 * to SImode".  The outcome is right; the cause is not, and the park's phrasing
 * points the next reader at the declaration.  Measured here with the TYPE
 * CONSTRUCTOR varied, the dimension that landed two overlay functions in this
 * same batch:
 *     struct { unsigned short v; } / union / `v : 16' / `unsigned short v[1]'
 * all dodge promote_mode exactly as they should -- its switch, explow.c:897-901,
 * covers INTEGER_TYPE and friends and lets RECORD_TYPE, UNION_TYPE and
 * ARRAY_TYPE fall out of the `default:' at :911 -- and the HImode constant set IS
 * created and DOES survive: it is present as
 *     (set (reg/v:HI 36) (const_int 65535))   `*thumb_movhi_insn' 180
 * in the zeroth, the second and the twelfth dump.  It dies in the THIRTEENTH.
 * What kills it is reading the member into an `int': Thumb has no register
 * zero_extendhisi2, so the read expands to the shift pair
 *     (set (reg:SI 61) (ashift (subreg:SI (reg:HI 36) 0) (const_int 16)))
 *     (set (reg:SI 60) (lshiftrt (reg:SI 61) (const_int 16)))
 * and combine constant-folds the pair into `(set (reg:SI 60) (const_int 65535))',
 * `*thumb_movsi_insn', which is the wide fix.  PROMOTE_MODE never got a turn.
 *   SO THE LEVER IS ON THE USE SIDE: the mask has to be CONSUMED in HImode, with
 *   no widening between its set and its use, the way this body's own narrow mask
 *   already is -- the thirteenth dump holds
 *     (set (reg:SI 62) (and:SI (reg/v:SI 36) (subreg:SI (reg:HI 60) 0)))
 *   which is precisely the shape wanted, and it is reached because
 *   store_fixed_bit_field masks at the UNIT mode (expmed.c:743-756) and never
 *   widens the constant it force_regs.
 *   And that is also why every use-side spelling tried here failed: consuming
 *   this mask in HImode means a sixteen-bit AND, and a sixteen-bit AND with this
 *   value is deleted -- by fold at tree level, or by combine at RTL level if it
 *   gets that far.  The two meet in the middle.  That is the real shape of the
 *   wall, and it is narrower than "no spelling exists": what is missing is a
 *   HImode CONSUMER of the mask that is not an AND.
 *
 * MEASURED THIS BATCH, every one against the claim at the top of this file and
 * none of them better:
 *   INERT, byte-identical to the body below -- a one-member struct mask; a
 *   one-member union mask; a `v : 16' mask; a one-element array mask; each of
 *   those with the mask assignment both before and after the pointer setup; a
 *   two-member struct with a separate mask per use; a plain halfword scalar mask
 *   (the park's own row, reproduced); and the mask applied with `=' rather than
 *   with `&='.
 *   WORSE, the fold rule firing and instructions GONE: the mask folded into the
 *   bitfield-store expression (fifty-one differing, four bytes short); a
 *   one-member-struct holder for the masked value (forty-nine differing, four
 *   bytes short).
 *   WORSE, the mask forced to the stack: a volatile member (sixty differing,
 *   eight bytes long); the two-member struct (sixty differing, twelve bytes
 *   long).
 
 *

 * ===== BATCH 337, BRIEF E: THE WALL IS NOW STATED AT THE PATTERN LEVEL, AND IT
 * ===== INVERTS BATCH 330's CONCLUSION.  PLUS THE FIRST FOLD-IMMUNE BASE.
 *
 * RE-MEASURED: 8 of 57, 48/48 instructions, size equal, RELOCATIONS differ.  The
 * .26.mach fix table reproduces the header's verbatim.
 *
 * THE REFERENCE'S OWN POOL, which no earlier header quoted and which settles the
 * target:
 *     ldr r5, .La1a9c  @ 0xffff      .La1a9c: .word 0xffff
 *     ldr r3, .La1aa0  @ 0x1ff       .La1aa0: .word 0x1ff      .pool
 * BOTH numeric words sit AHEAD OF EVERY `=symbol', so in the ROM
 * key(0xffff) < key(0x1ff) < 1020 and BOTH constants are narrow fixes.  With the
 * 0xffff reference pinned at addr 36 by an exact instruction stream, its range
 * must be under 68, and HImode's 64 is the only one.  The park's bound is sound.
 *
 * BATCH 330 CONCLUDED "what is missing is a HImode CONSUMER of the mask that is
 * not an AND."  **THE AND IS THE ONLY THING THAT PROTECTS IT.**  Off the dumps:
 *
 *  WHERE THE 0x1ff FIX SURVIVES (this body; .12.life and .13.combine identical):
 *     (insn 71 (set (reg:HI 60) (const_int 511)) 180 {*thumb_movhi_insn})
 *     (insn 76 (set (reg:SI 62) (and:SI (reg/v:SI 36) <(subreg:SI (reg:HI 60) 0)>)))
 *                                                      78 {*thumb_andsi3_insn}
 *  The HImode register flows DIRECTLY into the AND as an inline paradoxical
 *  subreg.  Combine's substitution would yield (and:SI (reg) (const_int 511)) and
 *  *thumb_andsi3_insn IS REGISTER-ONLY -- Thumb's `and' has no immediate
 *  alternative -- so the substitution fails to match and combine abandons it.
 *
 *  WHERE IT DIES, through a widening MOVE (scratch_elev/b337/E/w4/b1_hi_var.c):
 *     .12.life  (insn 78 (set (reg:HI 63) (const_int 511)) 180 {*thumb_movhi_insn})
 *               (insn 80 (set (reg:SI 65) (subreg:SI (reg:HI 63) 0)) 173 {*thumb_movsi_insn})
 *               (insn 81 (set (reg:SI 64) (and:SI (reg:SI 61) (reg:SI 65))))
 *     .13       insn 78 -> NOTE_INSN_DELETED;  insn 80 -> (set (reg:SI 65) (const_int 511))
 *  *thumb_movsi_insn ACCEPTS the constant, so 78-into-80 matches and the narrow
 *  set becomes dead.  Same story when the widening is a shift pair -- the park's
 *  own "variable mask used twice" row (w4/a7_struct_andeq.c):
 *     .12.life  (insn 22 (set (reg/v:HI 36) (const_int 65535)) 180 {*thumb_movhi_insn})
 *               (insn 68 (set (reg:SI 61) (ashift (subreg:SI (reg:HI 36) 0) 16)))
 *               (insn 69 (set (reg:SI 60) (lshiftrt (reg:SI 61) 16)))  REG_EQUAL 65535
 *     .13       68 deleted;  69 -> (set (reg:SI 60) (const_int 65535)) {*thumb_movsi_insn}
 *
 * **RESTATED RULE, the useful form: a narrow (HImode) minipool fix survives
 * combine IF AND ONLY IF the HImode constant register's only use is an operand
 * position whose pattern has NO IMMEDIATE ALTERNATIVE.**  On Thumb at -O2 that
 * position is the second operand of *thumb_andsi3_insn.  So what has to be
 * engineered is not a non-AND consumer -- it is an AND with NO WIDENING MOVE
 * between the set and the AND, which needs the AND's other operand to arrive
 * already in SImode, which needs a HImode AND tree whose other operand is an
 * SImode lowpart and whose constant is 0xffff -- and THAT tree is what fold's
 * `integer_all_onesp' rule deletes.  That is where the two walls meet, and it is
 * now a statement about two named patterns instead of about spellings.
 *
 * A NEW POSITIVE, THE FIRST ON THIS AXIS.  fold's deletion is DEFEATED by making
 * the mask a VARIABLE, because integer_all_onesp (arg1) needs an INTEGER_CST.
 * With the mask in a one-member struct (to dodge promote_mode, explow.c:897-911)
 * AND the AND performed in a 16-bit context, BOTH ANDs survive on BOTH axes --
 * which no previous body achieved.  scratch_elev/b337/E/w4/b1_hi_var.c reads
 * 17 of 57 at **47 instructions against 48**, and its first FORTY-FOUR
 * instructions are the ROM's, in order, including
 *     ldr r5,=0xffff ... strh r2,[r4,#6] / and r2,r5 / ldrh r0,[r4,#0x16] / and r2,r3
 *     ...               strh r3,[r4,#8]  / and r3,r5 / strb r3,[r4,#0x14]
 * The ONLY missing instruction is the ROM's `b' over the in-function pool: with
 * the mask a HImode VARIABLE every fix came out SImode (range 1020), the minipool
 * went to the end of the function and nothing had to be jumped over.  Its 17 is
 * therefore MEASURING MISALIGNMENT, not distance, and is a figure ABOUT THE
 * BLOCKER.  The park's two requirements are now visibly separable and
 * simultaneously unmet: a VARIABLE mask buys fold-immunity and loses the narrow
 * fix; a CONSTANT mask keeps the narrow fix (from 0x1ff) and loses the AND to fold.
 *
 * MEASURED THIS BATCH -- 17 variants, each with its instruction count, because a
 * figure at a different length is not a distance:
 *   park body (base)                                        8 of 57  48/48  SImode @36
 *   mask struct-wrapped, `vx &= m.v' (park's row)            8 of 57  48/48  SImode @36
 *   mask struct-wrapped, `cur->a16 = vx & m.v'              51 of 57  47/48  SImode @ 6
 *     ... union-wrapped / `v : 16' / one-element array      51 of 57  47/48  SImode
 *     ... plain `unsigned short' mask (promote_mode bites)  49 of 57  45/48  SImode
 *     ... struct mask assigned late                         50 of 57  45/48  SImode
 *   HImode holder + struct VARIABLE mask, both axes (b1)    17 of 57  47/48  SImode @36
 *     ... union / two holders / array mask / mask late      17 of 57  47/48
 *   HImode holder + CONSTANT mask (CONTROL)                 51 of 57  42/48  -6 insns
 *   `(unsigned short)vx & m.v' into the field, both axes    51 of 57  47/48  SImode @ 6
 *     ... vy axis only, vx left as `&= 0xffff'              49 of 57  45/48  SImode @34
 *     ... vx axis only, vy left as `&= 0xffff'              51 of 57  47/48  SImode @ 6
 *     ... array mask                                        51 of 57  47/48
 *
 * THE CONTROL ROW IS THE POINT OF THE TABLE: a CONSTANT 0xffff in the same HImode
 * position loses SIX instructions -- fold deleting both ANDs and both holder
 * moves -- while the VARIABLE-mask row loses ONE.  That is the fold-immunity,
 * isolated and measured.
 *
 * Three of the cast rows KEEP the HImode 0x1ff fix at addr 42, range (0,64) AND
 * use the variable mask, and are still one instruction short: there the 0xffff
 * AND is folded into the following 0x1ff AND.  So the two requirements have now
 * been shown jointly unsatisfiable by every spelling of this shape, which is
 * tighter than the park's statement.
 * -- scratch_elev/b337/E
 * BOTH MECHANICAL AXES ARE CLOSED -- measured batch 347 harvest pass, no agent.
 *   base 8 of 57, instruction counts EQUAL, byte size and pool
 *   count equal too (this park is in phase on all three length dimensions, so
 *   its figure IS a real distance).
 *   FLAG AXIS: all eight per-file groups measured, none improves the aligned
 *   residue.  ANCHOR AXIS: 11 placements swept, but 5 WERE BIT-IDENTICAL to
 *   the base, so only 6 DISTINCT PROGRAMS were actually tested.
 *   Those 6 are inert; the other 5 measured nothing.  No variant was capped and none failed to
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
struct Win {
    unsigned char pad00[0xc];
    unsigned short fc;
    unsigned short fe;
};

struct Cur {
    unsigned char pad00[6];
    unsigned short x;
    unsigned short y;
    unsigned char pad0a[0xa];
    unsigned short b14 : 8;
    unsigned short : 8;
    unsigned short a16 : 9;
};

struct State {
    unsigned char pad00[0x10];
    struct Win *win;
    struct Cur *cur;
};

extern struct State *iwram_3001f2c;
extern volatile unsigned int iwram_3001e40;
extern unsigned char Laf294[] __asm__(".Laf294");
extern unsigned char Laf29d[] __asm__(".Laf29d");

void Func_80a1a40(int x, int y)
{
    struct State *st;
    struct Cur *cur;
    int vx;
    int vy;

    st = iwram_3001f2c;
    cur = st->cur;
    vx = Laf294[(iwram_3001e40 >> 1) & 7] + x + (st->win->fc << 3) + 8;
    cur->x = vx;
    vx &= 0xffff;
    cur->a16 = vx;
    vy = Laf29d[(iwram_3001e40 >> 1) & 7] + y + (st->win->fe << 3) + 8;
    cur->y = vy;
    vy &= 0xffff;
    cur->b14 = vy;
}
