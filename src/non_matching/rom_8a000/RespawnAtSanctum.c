/* RespawnAtSanctum (0x0808a6e4) -- NON-MATCHING, 113 of 236 encodings differ.
 * 234 instructions, reference asm/rom_8a000/rom_8a5f8_a_a.s (line 131).
 *
 * 113 IS NOT A DISTANCE: size 508 against 492 (+16) and count 244 against 236
 * (+8), so the figure is dominated by register-name differences.  tryc --align
 * reads 116 of 256.  The useful signal is the RELOCATIONS: the first seven match
 * at identical byte offsets, so the whole revive block and the whole party loop
 * are already the right length and the residue is entirely in the coordinate tail.
 *
 * ============================ FIGURES ============================
 * objcmp (authority), this file:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *       scratch_elev/b297b/RespawnAtSanctum/park_RespawnAtSanctum.c \
 *       asm/rom_8a000/rom_8a5f8_a_a.s --func RespawnAtSanctum
 *
 *   XX SIZE  ref 492 bytes, ours 508
 *   XX ENCODINGS differ in 113 place(s) (ref 236, ours 244)
 *      first at index 1: ref 4978  ours 497c
 *   XX RELOCATIONS differ
 *
 * SIZE:              ref 492 bytes,  ours 508  (+16)
 * INSTRUCTION COUNT: ref 236 encodings, ours 244  (+8)
 * Neither matches, so 113 is NOT a distance -- it is dominated by
 * register-NAME differences on short-lived scratch pseudos.
 * tryc --align: "116 instruction(s) in disagreeing regions, of 256",
 * first diff at line 28 of 256.
 *
 * RELOCATIONS: the first SEVEN relocations match at IDENTICAL byte offsets
 * (_GetUnit 0x1c, __divsi3 0x36, __divsi3 0x68, _GetFlag 0x92, _GetUnit 0x9e,
 * __divsi3 0xb2, __divsi3 0xe4).  Everything through 0xe4 -- the whole revive
 * block and the whole party loop -- is the right LENGTH.  Only _SetFlag
 * (ref 0x1da / ours 0x1b6) and the gState pool word (ref 0x1e4 / ours 0x1f4)
 * differ, i.e. the residue is entirely in the coordinate tail.
 *
 * SHIMS (tools/shimcount.py): register pins 1, .equ 0, empty barriers 0.
 * The one pin is `register int t5 __asm__("r5")` and it is NOT cosmetic --
 * see LEVER 5 below.  Needs a fakematch.txt row if landed as-is.
 *
 * SPLIT SHAPE.  asm/rom_8a000/rom_8a5f8_a_a.s holds TWO functions, counted
 * with the anchored pattern:
 *     11:.thumb_func_start Func_808a5f8      @ 0x0808a5f8
 *    131:.thumb_func_start RespawnAtSanctum  @ 0x0808a6e4
 * so landing needs a split:
 *     asm/rom_8a000/rom_8a5f8_a_a_a.s   Func_808a5f8      (stays asm)
 *     src/rom_8a000/rom_8a5f8_a_a_b.c   RespawnAtSanctum  (this file)
 * and stage1.ld:769 `asm/rom_8a000/rom_8a5f8_a_a.o(.text)` becomes the two
 * halves in that order, ahead of the existing rom_8a5f8_a_b.o line.
 *
 * DATA EXPORTS.  `python3 tools/datacheck.py asm/rom_8a000/rom_8a5f8_a_a.s`
 * prints NOTHING and exits 0: no data section and no required `.global` data
 * export is lost when the hand-written .s is deleted.  Both functions are
 * reached by plain `bl` from asm/rom_8a000/rom_8a5f8_a_c_a.s (lines 210, 267),
 * same bank, so no veneer is involved.
 *
 * ======================= WHAT THE FUNCTION IS =======================
 * The `.s` prose calls it SetCurrentEncounterSet; that comment is WRONG for
 * this body (it describes the 0x236 store only).  The body is a sanctum
 * respawn:
 *   *(s16 *)(gState+0x236) = set;
 *   if (set == -1) {
 *       u = GetUnit(*(int *)(gState+0x1f4));      // party leader
 *       if (u->hp == 0) { u->hp = 1; recompute HP and PP bars }
 *       if (GetFlag(0x20)) for (i=0;i<=1;i++) { unit i full-heal + bars }
 *       move the party to the 0x1d2/0x1d4 respawn coords, else 0x1c4/0x1c6
 *   } else {
 *       move the party to the 0x1ce/0x1d0 coords, else 0x1c8/0x1ca + SetFlag(0x109)
 *   }
 * Unit fields, all `short`: 0x14 HP-bar, 0x16 PP-bar, 0x34 maxHP, 0x36 maxPP,
 * 0x38 HP, 0x3a PP.  gState halfwords: 0x1c0/0x1c2 current map+entrance,
 * 0x1c4/0x1c6, 0x1c8/0x1ca, 0x1ce/0x1d0, 0x1d2/0x1d4 candidate pairs.
 *
 * ================== THE EXEMPLAR THAT UNLOCKED IT ==================
 * The four `(x << 14) / y` clamp-to-0x4000 blocks are the SAME code as the
 * solved trio in src/rom_77000/: UpdateStatBarPercent (rom_77320_c_b.c),
 * Func_80782a0 (rom_77320_c_c_a.c) and Func_8078320 (rom_77320_c_c_b.c).
 * Same fields, same clamp, same `if ((v << 16) != 0) goto ...; if (f38 == 0)
 * goto ...; v = 1` tail.  Their register-named pseudo-C form is what this file
 * uses, and it reproduces clamps 2, 3 and 4 BYTE-FOR-BYTE with no edits.
 * docs/elevation.md:11313 and :21526 are about these same three functions.
 *
 * ===================== LEVERS THAT PAID (measured) =====================
 * Deltas are tryc --align's "N instruction(s) in disagreeing regions, of 256".
 *
 * 1. SIGNED halfword gState reads want the offset as a LITERAL IN THE ADDRESS
 *    EXPRESSION, not as a named offset variable (docs/elevation.md:13814).
 *    Thumb `ldrsh` has no immediate form, so `*(short *)(g + 0x1d2)` emits
 *    `mov rN,#0xe9 / lsl rN,#1 / add r3,r1,rN / mov rZ,#0 / ldrsh r2,[r3,rZ]`
 *    -- and each site gets its OWN zero scratch, which is what the ROM has
 *    (four separate `mov rZ,#0`).  Writing the offset as a variable CSEs the
 *    zeros into one.  184 -> 141.
 *    UNSIGNED halfword reads want the opposite: a pointer pseudo, which is
 *    why the 0x1c4/0x1c6/0x1c8/0x1ca copies go through a temp.
 *
 * 2. ONE `int m = -1;` shared by the top `set == m` test and branch 2's
 *    `a == m` test.  The ROM builds -1 in r4 before the 0x236 store and still
 *    has it at 0x8a846 (`cmp r2, r4`) because no call intervenes on that path;
 *    on the other path it rematerialises.  Per-site literals give the wrong
 *    prologue interleave.  First diff 2 -> 16.
 *
 * 3. The gState halfword copies must be LOAD-INTO-A-TEMP THEN STORE, not
 *    `*(u16*)(g+A) = *(u16*)(g+B)`.  With the assignment form gcc creates the
 *    STORE address pseudo first and chains its constant derivations off that;
 *    with the temp it creates the LOAD address first, which is the ROM's
 *    chain.  This is what produces `sub r4, #0x10` (0x1d4->0x1c4),
 *    `sub r0, #6` (0x1c8->0x1c2) and `sub r0, #0xb9` (0x1c2->0x109, the
 *    SetFlag argument).  Also stopped gcc cross-jumping the 0x1ca copy with
 *    the 0x1c6 copy through a register offset (`add r4,#6` / `add r4,#0xa`).
 *
 * 4. Branch 1's both-(-1) arm wants the temp; branch 2's does NOT.
 *    Applying the temp to BOTH arms moves the gState pool value from r1 to r2
 *    and rotates every `add r3, r1, rX` in the tail.  Measured: branch-1 only
 *    = gState in r1 (this file); both = gState in r2 (cand41/cand51, 137).
 *
 * 5. *** ONE PIN BUYS TWO FIXES ***  `register int t5 __asm__("r5")` on the
 *    first clamp's value variable closes, simultaneously:
 *      (a) the r5/r6 SWAP.  Unpinned, `u` (38 refs / 85 insns, loop-weighted)
 *          outranks the clamp value (10 / 32) in allocno_compare, is allocated
 *          4th and takes r5, so `u` lands in r5 and the clamp value in r6 --
 *          the ROM has them the other way round.  Pinning the clamp value to
 *          r5 forces `u` to r6 everywhere.  docs/elevation.md:24783.
 *      (b) the COMMONED 0x4000.  The ROM materialises 0x4000 THREE ways in
 *          clamp 1: `mov r5,#1 / lsl r5,#14` for the numerator/max, and a
 *          FRESH `mov r1,#0x80 / lsl r1,#7` for the `cmp`.  With a pseudo,
 *          cse always knows the numerator's register already holds 0x4000 and
 *          emits `cmp r0, r5`, two instructions short.  With the hard-register
 *          pin no pseudo is formed and the compare's constant is rebuilt --
 *          exactly docs/elevation.md:15121 ("the shim that does work is a bare
 *          register pin on the DOMINATING use").  Every pseudo spelling is a
 *          measured negative, see below.
 *
 * 6. Because the pinned r5 must still HOLD 0x4000 on the `>` arm, clamp 1 is
 *    written with NO assignment there:
 *        if (t0 <= t1) { if (t0 < 0) t5 = 0; else t5 = t0; }
 *    Clamps 2-4 keep the exemplar's `if (t0 > t3) t3 = 0x80 << 7; else ...`
 *    because there the bound variable and the result variable are the same.
 *
 * 7. `t2 = t5 << 16;` as its own statement AHEAD of the store reproduces the
 *    ROM's non-destructive `lsl r3, r5, #0x10 / strh r5, [r6, #0x14]`.
 *    Inline as `if ((t5 << 16) == 0)` gcc emits `strh` first and then destroys
 *    r5.  (Clamps 2-4 want the inline form; only clamp 1 is pinned.)
 *
 * ================ THE RESIDUE, BY THE PASS RESPONSIBLE ================
 * A. +8 instructions / +16 bytes, ALL in the coordinate tail, and it is TWO
 *    coupled defects:
 *      A1  cse, constant derivation.  In branch 1's both-(-1) arm the ROM
 *          derives 0x1c2 as `sub r4, #0x2` from the 0x1c4 it already holds;
 *          we materialise it canonically (`mov r2,#0xe1 / lsl r2,#1`).  We DO
 *          get the ROM's other three derivations (see LEVER 3), so the
 *          mechanism is reachable -- just not steerable here.
 *          docs/elevation.md:3017 says outright "gcc derives the second offset
 *          by itself ... Do not force it."
 *      A2  jump.c cross-jumping, post-reload (`.25.jump2`).  BECAUSE our 0x1c2
 *          offset is canonical it is register-identical to the `b != -1` arm's,
 *          so gcc merges the two final `strh`s into one block; the ROM, whose
 *          offset came out of A1's `sub`, cannot merge and keeps both copies.
 *          We are SHORT there, which is the "no known fix" direction at
 *          docs/elevation.md:1191.  A1 is the cause; A2 is downstream.
 *          Consequence: the ROM shares one `strh r2,[r3]` + `b == -1` test
 *          across all FOUR arms (L15/L16/L20); we share it only within each
 *          top-level branch.
 *
 * B. sched2 (`schedule_insns` after reload), ONE pair.  In the party loop the
 *    ROM has
 *        strh r1,[r6,#0x38] / strh r3,[r6,#0x3a] / lsl r1,#16 / asr r1,#16
 *    and we sink the second store two slots:
 *        strh r1,[r6,#0x38] / lsl r1,#16 / asr r1,#16 / strh r3,[r6,#0x3a]
 *    READ OUT OF THE DUMPS, not guessed: insn 291 is the `strh` to 0x3a.
 *        cand60.c.19.flow2 : (insn 287 283 291 ...) (insn 291 287 294 ...)
 *        cand60.c.23.sched2: (insn 287 283 294 ...) (insn 291 297 300 ...)
 *    i.e. it is in the ROM's place through flow2 and sched2 moves it.
 *    FLAG-CONDITIONAL measurement, labelled as such: `-fno-schedule-insns2`
 *    puts that pair right, but it ALSO drops the prologue's `ldr r2, =0x236`
 *    below the `strh` (the ROM interleaves it) and moves three tail constant
 *    materialisations.  By docs/elevation.md:11346's signature rule -- "a flag
 *    that improves but does not close a small residue is still the wrong
 *    answer" -- this is not it.  No SCHED2_CFLAGS row is warranted.
 *
 * C. reload / local_alloc spill-register choice, ~5 sites, NAME ONLY, no
 *    length change.  The short-lived register-offset scratches rotate:
 *        ROM  mov r1,#0x80  | mov r2,#0x38 | mov r3,#0x3a / mov r4,#0x36
 *             mov r5,#0x3a  | mov r1,#0x3a / mov r2,#0x36
 *        ours mov r2,#0x80  | mov r4,#0x38 | mov r5,#0x3a / mov r2,#0x36
 *             mov r4,#0x3a  | mov r2,#0x3a / mov r3,#0x36
 *    `.18.greg` dispositions for this file: gState 33 -> r1 (correct),
 *    u 39 -> r6 (correct, via the pin).  These five are below global_alloc.
 *
 * ================= NEGATIVES MEASURED (do not re-run) =================
 * On the commoned 0x4000 (all of these still emit `cmp r0, r5`, i.e. 2 short,
 * with an unpinned clamp variable -- probe2.c/probe3.c/probe4.c/probe5.c here):
 *   - `t1 = 0x80; t1 <<= 7;` then `if (t0 > t1)`            (the exemplar form)
 *   - a plain `if (t0 > 0x4000)` literal
 *   - the bound variable kept live past the compare (gives `mov r1, r5`)
 *   - `if (t0 <= t1) { ... }` with no assignment on the `>` arm
 *   - the clamp value declared `short` (adds `lsl/asr` on the `else` arm)
 *   - the max assigned from the numerator variable (`t5 = n` -> `mov r3, r5`)
 *   - the max in a variable distinct from the numerator (coalesced back)
 *   - `t5 *= 0x4000` instead of `t5 <<= 14`
 *   - reading the stored halfword back (`t2 = *(short *)(u + 0x38)`): gcc
 *     folds it AND pools the HImode 1 (`ldrh r3,.L12 / .word 1`), so the
 *     ROM's `mov r5,#1 / strh r5` disappears too
 *   - deriving the stored 1 from the guard value (`t5 = t3 + 1`): record_jump_
 *     equiv folds it to 1 anyway
 * Elsewhere:
 *   - `(char *)&gState + 0x1ca` written inline -> `ldr r3, =gState+458`, a
 *     FOLDED-ADDRESS relocation (the classic trap): must go through a pointer
 *     local.  Cost when wrong: 264 lines, 177 disagreeing.
 *   - a dedicated copy temp (`int c`) pushes r7; a loop-scoped second unit
 *     pointer pushes r7 (and r8 at branch scope).  Both break `push {r5,r6,lr}`.
 *   - a separate pointer local for the tail gState reload: 262/264 lines, worse.
 *   - pinning gState to r1 as well: makes `add r1, r3` destructive and rotates
 *     the scratches; 264 lines.
 *   - `for (i = 0; i <= 1; i++)` vs `i = 0; do { } while (i <= 1);`: INERT.
 *   - the loop counter sharing the clamp variable: INERT.
 *   - `a`/`b` declared once for both top-level branches instead of per-branch:
 *     INERT (gcc already merges them).
 *   - natural `*(short *)(u+0x38) = *(short *)(u+0x34);` field copies in the
 *     loop instead of int temps: does NOT fix residue B, 1 worse.
 *   - two distinct temps for branch 1's two copies: 242 lines, 98 encodings
 *     differ -- FEWER differences but shorter; kept as `cand62.c`.
 *
 * ================== BEST ALTERNATES LEFT IN THIS DIR ==================
 *   cand60.c == this file (508 bytes / 244 enc / 113 differ; gState r1)
 *   cand62.c  472 bytes / 226 enc /  98 differ   (branch-1 two temps)
 *   cand50.c  460 bytes / 220 enc / 113 differ   (no tail temps at all)
 *   cand41.c  508 bytes / 244 enc / 108 differ   (tail temps both arms, gState r2)
 *   ref.s     asm/rom_8a000/rom_8a5f8_a_a.s trimmed to RespawnAtSanctum, for tryc
 *   probe*.c  the clamp-spelling sweeps, cand60.c.*  the -da dumps
 *
 * VERDICT: PARK.  The revive block and the party loop (through byte 0xe4) are
 * structurally exact; the whole residue is the coordinate tail plus one sched2
 * pair and five scratch names.
 */
extern unsigned int gState;
extern void *_GetUnit(int id);
extern int _GetFlag(int id);
extern void _SetFlag(int id);

void RespawnAtSanctum(int set)
{
    char *g;
    int m;

    g = (char *)&gState;
    m = -1;
    *(short *)(g + 0x236) = set;
    if (set == m) {
        char *u;
        int t0, t1, t2, t3;
        register int t5 __asm__("r5");
        int i;
        int a, b;

        u = (char *)_GetUnit(*(int *)(g + 0x1f4));
        t5 = 0x38;
        t3 = *(short *)(u + t5);
        if (t3 == 0) {
            t5 = 1;
            *(short *)(u + 0x38) = t5;
            t5 <<= 14;
            t0 = 0x34;
            t1 = *(short *)(u + t0);
            t0 = t5;
            t0 /= t1;
            t1 = 0x80;
            t1 <<= 7;
            if (t0 <= t1) {
                if (t0 < 0) {
                    t5 = 0;
                } else {
                    t5 = t0;
                }
            }
            t2 = t5 << 16;
            *(short *)(u + 0x14) = t5;
            if (t2 == 0) {
                t2 = 0x38;
                t3 = *(short *)(u + t2);
                if (t3 != 0) {
                    t3 = 1;
                    *(short *)(u + 0x14) = t3;
                }
            }
            t3 = 0x3a;
            t0 = *(short *)(u + t3);
            t2 = 0x36;
            t1 = *(short *)(u + t2);
            t0 <<= 14;
            t0 /= t1;
            t3 = 0x80;
            t3 <<= 7;
            if (t0 > t3) {
                t3 = 0x80 << 7;
            } else if (t0 < 0) {
                t3 = 0;
            } else {
                t3 = t0;
            }
            *(short *)(u + 0x16) = t3;
            if ((t3 << 16) == 0) {
                t5 = 0x3a;
                t3 = *(short *)(u + t5);
                if (t3 != 0) {
                    t3 = 1;
                    *(short *)(u + 0x16) = t3;
                }
            }
        }
        if (_GetFlag(0x20) != 0) {
            i = 0;
            do {
                u = (char *)_GetUnit(i);
                t1 = *(unsigned short *)(u + 0x34);
                t3 = *(unsigned short *)(u + 0x36);
                *(short *)(u + 0x38) = t1;
                *(short *)(u + 0x3a) = t3;
                t1 <<= 16;
                t1 >>= 16;
                t0 = t1 << 14;
                t0 /= t1;
                t3 = 0x80;
                t3 <<= 7;
                if (t0 > t3) {
                    t3 = 0x80 << 7;
                } else if (t0 < 0) {
                    t3 = 0;
                } else {
                    t3 = t0;
                }
                *(short *)(u + 0x14) = t3;
                if ((t3 << 16) == 0) {
                    t0 = 0x38;
                    t3 = *(short *)(u + t0);
                    if (t3 != 0) {
                        t3 = 1;
                        *(short *)(u + 0x14) = t3;
                    }
                }
                t1 = 0x3a;
                t0 = *(short *)(u + t1);
                t2 = 0x36;
                t1 = *(short *)(u + t2);
                t0 <<= 14;
                t0 /= t1;
                t3 = 0x80;
                t3 <<= 7;
                if (t0 > t3) {
                    t3 = 0x80 << 7;
                } else if (t0 < 0) {
                    t3 = 0;
                } else {
                    t3 = t0;
                }
                *(short *)(u + 0x16) = t3;
                if ((t3 << 16) == 0) {
                    t5 = 0x3a;
                    t3 = *(short *)(u + t5);
                    if (t3 != 0) {
                        t3 = 1;
                        *(short *)(u + 0x16) = t3;
                    }
                }
                i++;
            } while (i <= 1);
        }
        g = (char *)&gState;
        a = *(short *)(g + 0x1d2);
        b = *(short *)(g + 0x1d4);
        if (a == -1) {
            if (b == a) {
                t0 = *(unsigned short *)(g + 0x1c4);
                *(unsigned short *)(g + 0x1c0) = t0;
                t0 = *(unsigned short *)(g + 0x1c6);
                *(unsigned short *)(g + 0x1c2) = t0;
                return;
            }
            a = *(unsigned short *)(g + 0x1c8);
        }
        *(unsigned short *)(g + 0x1c0) = a;
        if (b == -1) {
            *(unsigned short *)(g + 0x1c2) = *(unsigned short *)(g + 0x1ca);
        } else {
            *(unsigned short *)(g + 0x1c2) = b;
        }
    } else {
        int a, b;

        a = *(short *)(g + 0x1ce);
        b = *(short *)(g + 0x1d0);
        if (a == m) {
            if (b == a) {
                *(unsigned short *)(g + 0x1c0) = *(unsigned short *)(g + 0x1c8);
                *(unsigned short *)(g + 0x1c2) = *(unsigned short *)(g + 0x1ca);
                _SetFlag(0x109);
                return;
            }
            a = *(unsigned short *)(g + 0x1c8);
        }
        *(unsigned short *)(g + 0x1c0) = a;
        if (b == -1) {
            *(unsigned short *)(g + 0x1c2) = *(unsigned short *)(g + 0x1ca);
        } else {
            *(unsigned short *)(g + 0x1c2) = b;
        }
    }
}
