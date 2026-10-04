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
