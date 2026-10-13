/* Func_808e23c (0x0808e23c) -- NON-MATCHING, 28 of 266 encodings; ALIGNED
 * residue 23 (in 23 ref slots against 23 of ours).
 *   LENGTH GAP CLOSED: instructions 215/215.  SIZE was already exact at 632
 *   bytes and the POOL is exact -- 12 words in the reference's order, the only
 *   two R_ARM_ABS32 at 0x24c (gState) and 0x260 (iwram_3001ebc), matching.
 *   WAS: 64 of 266, instructions 215/216 -- EQUAL ENCODING-STREAM LENGTH (266
 *   both) WITH UNEQUAL INSTRUCTION COUNTS, because the reference carries a pad
 *   halfword our longer body does not.  Exactly batch 343 brief C's finding:
 *   equal stream length does NOT line positions up.  (batch 345 D)
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/rom_8a000/808e23c.c \
 *     asm/rom_8a000/rom_8d9a4_a_c_a_a_a_c_a_c_a.s --func Func_808e23c
 *
 * *** THE PARK'S SPLIT NOTE IS WRONG AND COSTS THE NEXT AGENT A STEP. ***  It
 * says "the reference .s holds TWO functions (Func_808e23c, Func_808e4b4), so
 * landing this one needs tools/split_s.py first".  It does not:
 *     asm/rom_8a000/rom_8d9a4_a_c_a_a_a_c_a_c_a.s holds only Func_808e23c and
 *     no data; convert it directly, no split needed
 * is what split_s.py --dry-run prints, there is exactly ONE .thumb_func_start
 * in the file, and datacheck.py reports nothing.  install_path is
 * src/rom_8a000/rom_8d9a4_a_c_a_a_a_c_a_c_a.c with no split and no exports.
 *
 * PER-REGION SPLIT of the old +1 -- it is ONE SITE and the regions either side
 * are exact (ref indices):
 *   prologue .. party loop .. best==0   0-87     ref 88 / ours 88   equal
 *   the e != 0 / else arms             88-203   ref 116 / ours 116  equal
 *   FieldMove_NoTarget block          204-226   ref  23 / ours  24  *** +1 ***
 *   tail                              227-end   ref  ?  / ours ?   equal
 * The extra instruction is `mov r0, r8` before the `ands`, because `one` sat in
 * the high register r8 and a Thumb `and` needs a low one.
 *
 * THE +1 WAS A LOCAL-ALLOC RACE, NOT global_alloc -- AND THE PARK'S WHOLE
 * DIAGNOSIS IS ABOUT global_alloc, SO IT WAS LOOKING IN THE WRONG PASS.
 * `one` and `zero` are pseudos 50 and 51 and they appear in NEITHER the
 * .18.greg allocno list NOR its dispositions: their definitions and uses sit in
 * one basic block (a CALL does not end a basic block in this CFG), so
 * local-alloc owns them.  Its ordering is QTY_CMP_PRI at local-alloc.c:1495-1497
 *     (floor_log2 (n_refs) * n_refs * size) / (death - birth) * 10000
 * -- the same shape as global.c's allocno_compare but over the qty's BIRTH-TO-
 * DEATH SPAN INSIDE THE BLOCK, and find_free_reg walks REG_ALLOC_ORDER
 * (local-alloc.c:2024-2042) excluding call-clobbered registers for any qty with
 * n_calls_crossed != 0 (:2031-2034), which is why both land in callee-saved
 * registers at all.  With `one` carrying THREE references -- `one = 1`,
 * `*fp = one`, and `(_GetItemInfo(item)[0xc] & one)` -- its span reaches past
 * Func_8097194 and _GetItemInfo, so 30000/span_one came out BELOW `zero`'s
 * 20000/span_zero, `zero` was allocated first, took r5, and `one` was pushed to
 * r8.  The ROM has it the other way round: `one` in r5, `zero` in r8, and it
 * pays the high-register copy on `zero`, which is used ONCE.
 *
 * THE FIX IS ONE TOKEN: `& 1` instead of `& one` in the last test.  That drops
 * `one` to two references dying at `*fp = one`, its span collapses, it outranks
 * `zero`, takes r5, and the ROM's `strb r5, [r3, #0]` appears.  64 -> 29,
 * instruction counts equal.  It is semantics-preserving by inspection: `one` is
 * assigned 1 four lines above and never written again.
 * Moving `fp = iw + 0xcc6;` above `_Func_801776c(0x91c, 1);` is then worth one
 * more POSITIONAL encoding, 29 -> 28, with the ALIGNED residue UNCHANGED at 23
 * (both bodies produce 46 differing slots in exactly the same places).  Shipped
 * because objcmp is the authority and 28 < 29, but do not read it as progress.
 *
 * THE RESIDUE IS A THREE-VALUE ROTATION AND IT IS NOW FULLY QUANTIFIED.
 * Out of .17.lreg and .18.greg for this body:
 *
 *   allocno  what                        refs/len  pri    ours   ROM
 *     151    strength-reduced gState     9/46      5869    r5     r5
 *            + 0x1f8 + j pointer (set
 *            twice, crosses 1 call)
 *      41    (unchanged, exact)          9/60      4500    r6     r6
 *      34    `e`, the Func_808e14c       6/35      3428    r5     r6
 *            result
 *      43    (unchanged, exact)         10/96      3125    r7     r7
 *      48    `msg` -- the Func_808ddec   6/56      2142    r7     r5
 *            result AND the 0x28 load,
 *            one variable (park's own
 *            merge, and it holds)
 *      98    the 0x142 constant          3/28      1071    r5     r7
 *      35    `iw`                        3/51       588    r8     r8
 *
 * The ROM allocates `msg` BEFORE `e`: msg takes r5 and e, which conflicts with
 * it, is pushed to r6.  We allocate `e` first, it takes r5, and msg lands in
 * r7.  So the lever is ONE COMPARISON -- 48 must outrank 34 -- and the park's
 * stated bound ("the ROM's order requires msg > e, i.e. msg's priority above
 * 0.343, WHICH ITS REF/LENGTH NUMBERS CANNOT REACH") is a statement about the
 * CURRENT numbers, not about reachability.  The exact alternatives:
 *     48 refs 6 -> 8   (floor_log2 8 = 3)  24/56 = 4285 > 3428
 *     48 refs 6 -> 7 AND length <= 40      14/40 = 3500 > 3428
 *     48 length 56 -> <= 34                12/34 = 3529 > 3428
 *     34 refs 6 -> 3                        3/35 =  857 < 2142
 *     34 length 35 -> >= 57                12/57 = 2105 < 2142
 *
 * AND THE PARK'S OWN "NEXT" IS THE RIGHT CLASS, PARTLY CONFIRMED AND PARTLY
 * REFUTED.  It says to find "a third live-range merge that lifts msg's
 * REG_N_REFS".  There IS one: `one` is dead-disjoint from `msg` (msg's last use
 * is Func_8096fb0(msg, 0), which precedes `one = 1`), AND THE ROM PUTS BOTH IN
 * r5 -- ref 150 `ldrh r5, [r0, #0x28]`, ref 209 `adds r0, r5, #0`, ref 213
 * `movs r5, #1`.  Reusing `msg` as `one` DOES lift it exactly as predicted:
 * 48 goes 6/56 = 2142 to 9/67 = 4029, above `e`'s 3428, and the allocation
 * order changes to ... 41, 48, 34 ...  BUT 48 STILL DOES NOT GET r5: it gets
 * r7, because allocno 151 -- the strength-reduced `gState + 0x1f8 + j` pointer
 * out of the party loop -- holds r5 across msg's SECOND live piece (the `else`
 * arm) and conflicts with it, while NOT conflicting with `e`.  Net 95 of 266
 * at -2 instructions, much worse.  So the merge is not the whole lever: 151's
 * range has to come off r5 as well, or msg's second piece must stop overlapping
 * it.  151 is live at the start AND end of the mid-function blocks (.17.lreg
 * "Registers live at start: ... 33 41 43 44 45 46 47 66 151"), and in the ROM
 * it cannot be, because the ROM's r5 is free there for msg.  THAT is the thing
 * to measure next, and it is upstream of the allocator entirely -- it is
 * whether loop.c keeps the reduced pointer live past the loop.
 *
 * MEASURED THIS BATCH, all against the parked body:
 *   `& 1` for `& one`                   29 / gap 0 / aligned 23  <-- the lever
 *   that, plus fp hoisted above the
 *     _Func_801776c call                28 / gap 0 / aligned 23  <-- THIS BODY
 *   fp hoisted above both _Func_8019908 36 / gap 0 / aligned 24  worse
 *   a named `off = 0xcc6` before the
 *     _Func_801776c call                29 / gap 0 / aligned 23  inert
 *   the same before _Func_8019908       29 / gap 0 / aligned 23  inert
 *   the same at the top of the msg arm  29 / gap 0 / aligned 23  inert
 *   `fp = (unsigned char *)0xcc6 +
 *     (int)iw` (operand order)          29 / gap 0 / aligned 23  inert
 *   `zero = 0; one = 1;` (swap)         84 / gap 0 / aligned 34  worse -- the
 *                                       park recorded this as "35 regions"
 *                                       and it reproduces as worse
 *   `zero = 0;` hoisted above fp        84 / gap 0 / aligned 34  worse
 *   `zero = 0;` above Func_8096fb0     138 / gap -2 / aligned 48 worse
 *   `zero = 0;` at the top of the arm  137 / gap -4 / aligned 53 worse
 *   a separate `mask = 1` local         64 / gap +1 / aligned 29 exactly inert
 *   a second `one2 = 1` for the test    64 / gap +1 / aligned 29 exactly inert
 *   `& one) == 1` instead of `!= 0`     64 / gap +1 / aligned 30 inert/worse
 *   `one` merged into `msg`             95 / gap -2 / aligned 46 (see above --
 *                                       the mechanism WORKS, the register does
 *                                       not follow)
 * FLAG AXIS, autosweep, 8 of 8 groups on the parked body: -fno-gcse,
 *   -fno-strict-aliasing, -fno-rerun-loop-opt all 64 (exactly inert);
 *   -fno-rerun-cse-after-loop 90; -fno-schedule-insns2 101; -fno-strength-reduce
 *   239; -ffixed-r7 248; -O1 259.  Nothing helps.
 * ANCHOR AXIS, autosweep: 70 boundaries, 64 measured, 6 did not compile so THAT
 *   PART IS UNTESTED NOT INERT.  Best is 63 at four placements, all with `first`
 *   unmoved at 85 -- phase, worth 1.
 *
 * The pool, the party loop and the two message arms are all exact and stay
 * exact under this change; everything that differs is the rotation above.
 */
extern unsigned char gState[];
extern unsigned char *iwram_3001ebc;

struct Ev {
    int f0;
    unsigned short f4;
    short f6;
    int f8;
};

extern int _GetPartySize(void);
extern unsigned char *_GetUnit(int id);
extern void _Func_801776c(int a, int b);
extern void _ClearFlag(int id);
extern void _SetFlag(int id);
extern int _GetFlag(int id);
extern void _Func_8019908(int a, int b);
extern struct Ev *Func_808e14c(int a);
extern int Func_808ddec(int a);
extern void CutsceneStart(void);
extern void CutsceneEnd(void);
extern void MessageID(int id);
extern void ActorMessage(int a, int b);
extern unsigned char *_GetItemInfo(int item);
extern int Func_8091d84(int a);
extern void _Func_8019a54(void);
extern void Func_8096fb0(int a, int b);
extern void FieldMove_NoTarget(void);
extern void Func_8097194(void);
extern void _Func_80788c4(int a, int b);

int Func_808e23c(int arg, int a1)
{
    struct Ev *e;
    unsigned char *iw;
    unsigned short *p;
    unsigned char *gp;
    unsigned char *gs;
    unsigned char *gs2;
    int i;
    int j;
    int n;
    int best;
    int item;
    int unit;
    int size;
    int ret;
    int msg;
    int m3e7;
    int one;
    int zero;
    unsigned char *fp;
    int v;
    int w2;

    ret = -1;
    item = arg & 0x3ff;
    unit = (arg >> 10) & 0xf;
    best = 0;
    size = _GetPartySize();
    if (unit == 0xf) {
        unit = 0;
        for (j = 0; j < size; j++) {
            p = (unsigned short *)(_GetUnit(gState[(0xfc << 1) + j]) + 0xd8);
            n = 0;
            for (i = 0xe; i >= 0; i--) {
                if ((*p++ & 0x1ff) == item)
                    n++;
            }
            if (best < n) {
                best = n;
                unit = gState[(0xfc << 1) + j];
            }
        }
    } else {
        p = (unsigned short *)(_GetUnit(unit) + 0xd8);
        for (i = 0xe; i >= 0; i--) {
            if ((*p++ & 0x1ff) == item)
                best++;
        }
    }
    if (best == 0) {
        _Func_801776c(0x927, 1);
        return -1;
    }
    e = Func_808e14c(item);
    if (e != 0 && e->f8 != 0) {
        _ClearFlag(0x143);
        _ClearFlag(0xa1 << 1);
        if ((e->f4 & (0x80 << 3)) == 0) {
            _Func_8019908(unit, 1);
            _Func_8019908(item, 2);
            _Func_801776c(0x91c, 1);
        }
        if (e->f8 < (0x80 << 9)) {
            gs = gState;
            gs += 0xfa << 1;
            msg = Func_808ddec(*(int *)gs);
            CutsceneStart();
            MessageID(e->f8);
            ActorMessage(msg, 0);
            CutsceneEnd();
        } else {
            ((void (*)(int, int, int))e->f8)(item, unit, a1);
        }
        ret = 0;
    } else {
        _ClearFlag(0x143);
        _SetFlag(0xa1 << 1);
        msg = *(unsigned short *)(_GetItemInfo(item) + 0x28);
        iw = iwram_3001ebc;
        if (msg != 0) {
            _SetFlag(0x145);
            _ClearFlag(0xa1 << 1);
            m3e7 = 0x3e7;
            if (msg == 0x95 && _GetFlag(0xa2 << 1) == 0) {
                _Func_8019908(item, 2);
                _Func_801776c(0x924, 0xd);
                j = Func_8091d84(1);
                _Func_8019a54();
                if (j != 0)
                    return 0;
                gs2 = gState;
                v = *(unsigned short *)(gs2 + 0x240);
                *(unsigned short *)(gs2 + 0x1c0) = v;
                w2 = *(unsigned short *)(gs2 + 0x242);
                *(unsigned short *)(gs2 + 0x1c2) = w2;
                *(unsigned short *)(iw + (0xb8 << 1)) = m3e7;
            }
            _Func_8019908(unit, 1);
            _Func_8019908(item, 2);
            fp = iw + 0xcc6;
            _Func_801776c(0x91c, 1);
            Func_8096fb0(msg, 0);
            one = 1;
            zero = 0;
            *fp = one;
            FieldMove_NoTarget();
            *fp = zero;
            Func_8097194();
            if ((_GetItemInfo(item)[0xc] & 1) != 0)
                _SetFlag(0x143);
        }
    }
    if (_GetFlag(0xa1 << 1) != 0)
        _Func_801776c(0x927, 1);
    if (_GetFlag(0x143) != 0)
        _Func_80788c4(unit, a1);
    return ret;
}
