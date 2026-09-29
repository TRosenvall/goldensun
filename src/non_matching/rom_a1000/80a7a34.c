/* Func_80a7a34 -- the equipment/party picker row of the status screen,
 * 0x080a7a34, 336 ROM instructions.
 * NON-MATCHING, 261 of 358 encodings differ.
 *
 * 261 is NOT a true distance: size differs (824 vs 820) and the count differs
 * (359 vs 358), so objcmp's index-by-index count saturates after the single
 * defect.  The honest figure is the aligned one -- tools/aligncmp.py reads
 * 349 of 358 aligned-equal (97.5%), 12 differing in 11 hunks, and NINE of those
 * eleven hunks are the branch/pool-offset cascade behind ONE defect.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/rom_a1000/80a7a34.c \
 *     asm/rom_a1000/rom_a7380_a_c_a_c_c.s --func Func_80a7a34
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/aligncmp.py src/non_matching/rom_a1000/80a7a34.c \
 *     asm/rom_a1000/rom_a7380_a_c_a_c_c.s Func_80a7a34 -v
 *
 * THE SPLIT.  asm/rom_a1000/rom_a7380_a_c_a_c_c.s holds THREE functions
 * (Func_80a7850 at 0x080a7850, Func_80a7a34, Func_80a7d68 at 0x080a7d68) and no
 * .section .data; tools/datacheck.py is silent, so the split is pure text and
 * needs NO `.global`.  Func_80a7850 and Func_80a7d68 stay in asm/.
 *
 * SHIMS: ZERO in this file.  The four palette transfers use DMA3_COPY16_RW from
 * include/dma.h (promoted there in batch 299 for Func_80a7478), so the four
 * register pins live in the shared header exactly as the other helpers' do.  The
 * ROM does NOT preserve src or count across the stmia -- it reloads both from the
 * pool at every transfer and derives only the DESTINATION (`add r1,#0x1c`,
 * `add r1,#4`, `add r1,#0x1c`) -- which is precisely what the _RW form's `"+l"`
 * outputs buy.  Plain DMA3_COPY16 would let reload_cse_move2add strength-reduce
 * the sources; it does not here, and the first attempt with _RW was exact on all
 * twelve encodings.
 *
 * ============================================================
 * THE BLOCKER: ONE INSTRUCTION.  gcse's CPROP FOLDS THE NAMED MESSAGE BASE.
 * Pass: gcse.c / one_cprop_pass.  NOT cse.c -- see the dump evidence below.
 *
 * The ROM prints three strings through _Func_801e7c0 and derives the third id
 * from the first:
 *
 *     ldr  r6, =0xb0d          <- the base, in a CALLEE-SAVED register
 *     ldr  r1, [r5]
 *     mov  r0, r6              <- first use is a COPY, not a pool load
 *     ...  bl _Func_801e7c0
 *     mov  r0, #0x30 / bl _GetFlag / cmp r0,#0 / beq .La7b20
 *     ldr  r1, [r5] / ldr r0, =0xb16 / ... / bl _Func_801e7c0
 *   .La7b20:
 *     ldr  r1, [r5]
 *     sub  r0, r6, #3          <- 0xb0a derived from the base ACROSS the join
 *
 * Ours emits `ldr r0,=0xb0d` and `ldr r0,=0xb0a`: two pool loads, one pool word
 * too many, the `mov r0,r6` and the `sub r0,r6,#3` both gone.  That is the whole
 * residue -- aligncmp's eleven hunks are one register field (r6 vs r0 on the same
 * pool word), one deleted `adds r0,r6,#0`, one deleted `subs r0,r6,#3`, one
 * inserted `ldr r0,=0xb0a`, the extra `.word 0x00000b0a`, one alignment `.short`,
 * and five branch/PC-load offsets that move because the function got 4 bytes
 * longer.
 *
 * IT CANNOT BE LITERALS.  docs/elevation.md, "gcc-2.96 NEVER chains plain
 * CONST_INTs": cse's use_related_value chains a SYMBOL_REF plus an offset, never
 * two CONST_INTs.  `sub r0, r6, #3` after a CODE_LABEL is also not
 * reload_cse_move2add, which resets move2add_last_label_luid at every label and
 * so tracks no register constant across `.La7b20`.  So the base was NAMED in the
 * source, and `int msg = 0xb0d; ... f(msg); ... f(msg - 3);` is the spelling --
 * which is what this file has.
 *
 * WHICH PASS FOLDS IT, MEASURED, NOT GUESSED.  `-da` on this very file, one
 * docker invocation:
 *
 *     grep -c "const_int -3 "   a7a34_v2.c.02.jump       -> 1
 *                               a7a34_v2.c.03.cse        -> 1
 *                               a7a34_v2.c.04.addressof  -> 1
 *                               a7a34_v2.c.07.gcse       -> 0
 *
 * and .03.cse still carries, verbatim:
 *
 *     (insn 289 ... (set (reg:SI 107) (plus:SI (reg/v:SI 92) (const_int -3)))
 *                   5 {*thumb_addsi3} (expr_list:REG_EQUAL (const_int 2826)))
 *     (insn 243 ... (set (reg:SI 0 r0) (reg/v:SI 92))
 *                   173 {*thumb_movsi_insn} (expr_list:REG_EQUAL (const_int 2829)))
 *
 * i.e. **cse PRESERVES the ROM's exact shape** -- both the copy and the sub -- and
 * its cost model is the reason (a REG is cheaper than a pooled CONST_INT, and a
 * thumb_addsi3 is cheaper than a pool load).  gcse's cprop then destroys it:
 * try_replace_reg's simplify_replace_rtx turns (plus 2829 -3) into (const_int
 * 2826), validate_change accepts it, reg 92 drops to a single use, and a later
 * pass copy-propagates that one too -- which is why even the FIRST use loses its
 * `mov r0,r6`.
 *
 * CORRECTS docs/elevation.md.  The batch-298 entry on Func_80a8914 says the
 * `t = 0xb0e; t - 0x17` spelling "reproduces the ROM's sub forms ... but ONLY
 * under -fno-gcse", and blames gcse.  The attribution is right and is now proved
 * by dump rather than by flag response -- but the entry's implied remedy is
 * WRONG for this function: -fno-gcse here reads 354 encodings against 358 with
 * the first difference at index 7, deep in the prologue.  The flag removes the
 * fold and breaks four other things.  So "-fno-gcse rescues the named base" is
 * NOT a general escape; on Func_80a8914 it happened to be harmless.
 *
 * NEW LEVER, AND IT HALF-WORKS: PUT THE DERIVED EXPRESSION IN THE SAME BASIC
 * BLOCK AS THE BASE'S DEFINITION.  cprop_insn's first gate is
 * `oprs_not_set_p (reg, insn)` -- if the register is set anywhere in insn's own
 * block before insn, cprop SKIPS that use outright, no availability analysis.
 * Calls do not end a basic block in C, so the three _Func_801e7c0 / _GetFlag
 * calls and the base's definition are all in ONE block, and hoisting the
 * subtraction into it is legal:
 *
 *     int msg = 0xb0d;
 *     int tail = msg - 3;            // same block as the definition
 *     ...
 *     _Func_801e7c0(msg, win, 0, 0);
 *     if (_GetFlag(0x30)) _Func_801e7c0(0xb16, win, 0, 0x10);
 *     _Func_801e7c0(tail, win, 0, 8);
 *
 * MEASURED: this KEEPS the sub -- `ldr r5,=0xb0d / mov r0,r5 / sub r7,r5,#3 /
 * ... / mov r0,r7` -- so the mechanism is confirmed in both directions.  It is
 * still not the ROM, because the sub is now computed BEFORE the branch and its
 * result needs a SECOND callee-saved register (r7) to cross the join, where the
 * ROM computes it AFTER the join from the one live base.  360 encodings, 824
 * bytes, 275 differing: one instruction better placed, one register worse.
 * Recorded because on a reference whose sub IS in the base's own block this
 * should be exact, and it is the only production-flag way found to stop cprop.
 *
 * AND IT IS NOT THE SAME-BLOCK LEVER'S FAULT -- THE BOUND IS *TWO USES* IN THAT
 * BLOCK.  The sibling Func_80a8114, landed EXACT in this same batch, has the
 * identical shape one function away: `ldr r5,=0xb06 / mov r0,r5 / ... /
 * add r0,r5,#1`, and there `int msg = 0xb06; f(msg, ...); f(msg + 1, ...);`
 * reproduces the ROM on the first try -- because BOTH uses sit in the one basic
 * block that the definition is in, so cprop's `oprs_not_set_p` gate skips both.
 * Moving this function's definition down into the join block instead (so the
 * `- 3` use is in the definition's own block) does NOT work: measured at
 * 349/358, identical to this file, because msg then has only ONE use and
 * cse/combine propagate the constant into it and delete the base.  So the rule
 * that came out of the pair is: NAMED BASE, SAME BLOCK, TWO OR MORE USES -- and
 * this function fails the middle condition irreducibly, because the ROM's own
 * first use is before the `_GetFlag(0x30)` branch and its second after the join.
 *
 * THE UNRESOLVED PART, stated precisely.  The ROM has the base's definition in
 * block B1 and the `plus` in the join block B3, with the definition available at
 * B3's entry -- the exact configuration cprop was written to fold.  Blocking it
 * needs the set to be unavailable at B3's entry, i.e. a second, differently
 * valued definition of `msg` on the if-body path, which no plausible source
 * spells.  Eight spellings measured, all landing in one of three classes:
 *   both uses folded (824 / 359 / 261)  -- msg - 3; block-scoped tail; msg + 9
 *                                          for 0xb16; tail in an inner block
 *   first use kept, second re-pooled (828 / 361 / 266)
 *                                       -- msg -= 3; msg = msg - 3; msg -= 3
 *                                          with msg + 9; msg -= 3 then msg += 3
 *   sub kept, extra register (824 / 360 / 275)
 *                                       -- the same-block lever above, either
 *                                          declaration position
 *   definition moved into the join block  (824 / 359 / 261, 349 aligned)
 *                                       -- no better than this file; one use
 *                                          left, so the base is propagated away
 * Flag probes, diagnostic only: -fno-gcse 354/816, -fno-rerun-cse-after-loop
 * 352/812, -fno-cse-follow-jumps 354/816 -- every one WORSE and every one with
 * its first difference at index 7.
 *
 * ============================================================
 * WHAT CLOSED THE OTHER 349, in the order it paid:
 *
 * 1. ldrsb IS THE STRUCT FIELD, NOT A CAST.  The ROM reads sel and count as
 *    `mov r1,#0x1c / ldrsb r1,[r3,r1]`.  A four-way probe (struct `signed char`
 *    field, `*(signed char *)(p + K)` off an unsigned char*, the same cast off a
 *    struct pointer, and `((signed char *)p)[K]`) gives ldrsb with a register
 *    offset in ALL FOUR -- thumb_extendqisi2's "V" alternative wants a
 *    non-offsettable MEM and gcc forces the constant into a register to get one.
 *    So the spelling was never the problem; ours came out `ldrb / lsl #24 /
 *    asr #24` only because the two loads were scheduled LATE, and what fixed it
 *    was reading sel and count BEFORE initialising redraw and result.  Worth
 *    both loads, 6 encodings.
 *
 * 2. DECLARATION ORDER.  redraw is at [sp,#8] and result at [sp,#4], so redraw
 *    is declared FIRST (spill-slot addresses follow declaration order, first
 *    declared = highest).  [sp,#0] is the outgoing 5th argument of Func_80a1870
 *    and is below both, as the argument area always is.  Frame 0xc exact on the
 *    first try.
 *
 * 3. sel * 24 IS NOT A SEPARATE MULTIPLY.  `Func_80a1a40(sel * 24 - 10, 0x10)`
 *    comes out as `add r0,r7,r10 / lsl r0,#3 / sub r0,#0xa` because r7 already
 *    holds sel*2 for st->roster[sel]; synth_mult's 24 = (x + x*2) << 3 reuses
 *    it via cse.  Nothing had to be written to make that happen -- do not reach
 *    for an explicit index local.
 *
 * 4. THE TWO KEY-REPEAT ARMS SHARE A ZERO THAT IS A TESTED MASK.  The fifth
 *    argument of Func_80a1870 is `str r6,[sp]` in the Right arm and `str r5,[sp]`
 *    in the Left arm, where r6 is `gKeyPress & 2` and r5 is `gKeyRepeat & 0x100`
 *    -- both provably zero on their path.  The source passes a plain `0`; cse's
 *    record_jump_equiv supplies the register.  No spelling needed.
 *
 * 5. `sel--` BEFORE `redraw = 1` in the Left/B-button arm.  The ROM negates the
 *    FIRST materialised 1 (`mov r1,#1 / mov r2,#1 / neg r1,r1 / str r2,[sp,#8] /
 *    add r10,r1`); ours negated the second until the two statements were
 *    swapped.  The Right arm reuses one register for both and is order-blind.
 *
 * 6. BOTH COUNTED LOOPS ARE PLAIN FORWARD/REVERSE `for`s.  The `sub r2,#1` sits
 *    at the TOP of the body in the ROM; that is sched2 hoisting the bottom
 *    decrement, not a do/while.  `for (i = 0; i < 4; i++)` over the two u16[4]
 *    at 0x234/0x23c (check_dbra_loop reverses it, counter 3 down to -1, pointer
 *    ascending) and `for (i = 7; i >= 0; i--)` over the u16[8] at 0x144
 *    (counter IS the index, pointer descending) are exact as written, all three
 *    instances.
 *
 * STRUCT.  iwram_3001f2c as this function sees it: 0x08 u32 (written from a u16),
 * 0x10 void*, 0x1c/0x1e signed char (selection, count), 0x10c unsigned (the text
 * window), 0x144 u16[8] (row tints, 0x1e default / 0x1a selected), 0x208 u16[8]
 * (the roster), 0x21a u8, 0x220 u16 (the page, read once), 0x234 u16[4] and
 * 0x23c u16[4] (the tile/attribute pair).  The 0x10/0x1c/0x1e part agrees with
 * src/rom_a1000/rom_a7380_a_c_a_c_b.c's struct StatusState, whose Func_80a77a4
 * is this function's caller; that file also confirms sel is `signed char` by
 * getting ldrsb out of `st->sel[which]`.
 */
#include "dma.h"

extern volatile unsigned int gKeyPress;
extern volatile unsigned int gKeyRepeat;

extern void _Func_8016498(unsigned int win);
extern void _Func_801e7c0(int msg, unsigned int win, int x, int y);
extern int _GetFlag(int id);
extern int _GetUnit(int id);
extern void _PlaySound(int id);
extern void WaitFrames(int n);
extern void Func_80a195c(void);
extern void Func_80a1870(void *p, int a, int b, int c, int d);
extern void Func_80a1a40(int x, int y);
extern void Func_80a2144(int a);
extern void Func_80a7850(void);
extern int Func_80a7f44(int sel, int dir);
extern void Func_80a8088(int id, int page);

struct StatusState {
    unsigned char pad0[8];
    unsigned int f8;
    unsigned char pad0c[4];
    void *f10;
    unsigned char pad14[8];
    signed char sel;
    unsigned char pad1d;
    signed char count;
    unsigned char pad1f[0xed];
    unsigned int win;
    unsigned char pad110[0x34];
    unsigned short row[8];
    unsigned char pad154[0xb4];
    unsigned short roster[8];
    unsigned char pad218[2];
    unsigned char b21a;
    unsigned char pad21b[5];
    unsigned short page;
    unsigned char pad222[0x12];
    unsigned short tile[4];
    unsigned short attr[4];
};
extern struct StatusState *iwram_3001f2c;
extern void Func_80a1804(struct StatusState *st, int id);

int Func_80a7a34(void)
{
    struct StatusState *st = iwram_3001f2c;
    int sel = st->sel;
    int count = st->count;
    int redraw = 1;
    int result = 0;
    int page = st->page;
    int i;

    _GetUnit(st->roster[sel]);
    for (i = 0; i < 4; i++) {
        st->tile[i] = 0x82 + i * 0x20;
        st->attr[i] = 0x80;
    }
    Func_80a2144(0xe);
    DMA3_COPY16_RW((void *)0x5000200, (void *)0x5000000, 0x40);
    DMA3_COPY16_RW((void *)0x50001c8, (void *)0x500001c, 4);
    DMA3_COPY16_RW((void *)0x5000200, (void *)0x5000020, 0x40);
    DMA3_COPY16_RW((void *)0x50001e8, (void *)0x500003c, 4);

    while (!_GetFlag(0x150)) {
        if (redraw) {
            int msg = 0xb0d;

            redraw = 0;
            _Func_8016498(st->win);
            _Func_801e7c0(msg, st->win, 0, 0);
            if (_GetFlag(0x30))
                _Func_801e7c0(0xb16, st->win, 0, 0x10);
            _Func_801e7c0(msg - 3, st->win, 0, 8);
            sel = (sel + count) % count;
            _GetUnit(st->roster[sel]);
            page = (page + 3) % 3;
            Func_80a8088(st->roster[sel], page);
            Func_80a1804(st, st->roster[sel]);
            for (i = 7; i >= 0; i--)
                st->row[i] = 0x1e;
            st->row[sel] = 0x1a;
        }
        Func_80a1a40(sel * 24 - 10, 0x10);
        WaitFrames(1);
        if (gKeyPress & 1) {
            _PlaySound(0x70);
            result = 1;
            break;
        }
        if (gKeyPress & 2) {
            _PlaySound(0x71);
            result = -1;
            break;
        }
        if (gKeyRepeat & 0x100) {
            if (Func_80a7f44(sel, 1)) {
                _PlaySound(0x70);
                sel++;
                Func_80a195c();
                Func_80a1870(st->f10, 2, 2, 8, 0);
                for (i = 7; i >= 0; i--)
                    st->row[i] = 0x1e;
                st->row[sel] = 0x1a;
            } else {
                _PlaySound(0x72);
            }
            WaitFrames(1);
        } else if (gKeyRepeat & 0x200) {
            if (Func_80a7f44(sel, 0)) {
                _PlaySound(0x70);
                sel--;
                Func_80a195c();
                Func_80a1870(st->f10, 2, 2, 8, 0);
                for (i = 7; i >= 0; i--)
                    st->row[i] = 0x1e;
                st->row[sel] = 0x1a;
            } else {
                _PlaySound(0x72);
            }
            WaitFrames(1);
        } else if ((gKeyPress & 4) && _GetFlag(0x30)) {
            Func_80a7850();
            redraw = 1;
        } else {
            if (gKeyRepeat & 0x20) {
                _PlaySound(0x6f);
                if (count > 1) {
                    sel--;
                    redraw = 1;
                }
            }
            if (gKeyRepeat & 0x10) {
                _PlaySound(0x6f);
                if (count > 1) {
                    redraw = 1;
                    sel++;
                }
            }
        }
    }
    st->sel = sel;
    st->f8 = st->roster[sel];
    st->b21a = st->roster[sel];
    return result;
}
