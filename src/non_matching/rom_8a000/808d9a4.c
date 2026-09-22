/* Func_808d9a4 (0x0808d9a4) -- NON-MATCHING, 372 encodings of 424, size 996
 * against the ROM's 1044 (-48).  269 diff regions.  The least-advanced park of
 * batch 280 and honestly so: structurally traced and largely aligned, but with
 * three independent blockers.
 *
 * Blocker class: BASIC-BLOCK ORDER (primary), plus a register permutation and
 * two symbol tells.
 *
 * Verify with:
 *   python3 tools/objcmp.py src/non_matching/rom_8a000/808d9a4.c \
 *     asm/rom_8a000/rom_8d9a4_a_a_a.s --func Func_808d9a4
 * The reference .s holds this function alone, so no split is needed.
 *
 * WHAT IS ALREADY RIGHT: the whole cutscene chain, the four 0x?00000 cases,
 * and the item / coins / encounter arms all line up.  Three corrections found
 * while transcribing, each worth keeping:
 *   - the range test is `(unsigned)(arg - 0xf2) <= 5` (UNSIGNED, giving `bhi`)
 *   - the table value is a SEPARATE variable from the later `idx`
 *   - Func_808ece0 guards on `(e->f0 & 0x1ff) == 0x13` BEFORE the 0x300000
 *     chain, not after
 *
 * BLOCKER (a) -- BASIC-BLOCK ORDER, and no structured spelling reaches it.
 * The ROM lays out test1, test2, body1, body2, cutscene, with test2 ending in
 * `bne CS / b B2`.  Measured:
 *     if / else if / else       -> test1, body1, test2, body2, CS   286
 *     the inverted form         -> test1, test2, ..., body1         269
 *     an explicit goto layout   -> 292
 * The ROM's order interleaves two tests ahead of both bodies, which C's
 * structured forms do not express and which gcc-2.96's jump optimiser does not
 * produce from any of the three.
 *
 * BLOCKER (b) -- a two-swap register permutation (`arg` r8<->r10, `flagid`
 * r10<->r8, `e` r7<->r6, `actor` r6<->r7), unmoved by declaration order.
 * Almost certainly DOWNSTREAM of (a): block order changes live ranges, and
 * every permutation in this round's other large functions turned out to be a
 * symptom.  DO NOT CHASE THIS BEFORE (a).
 *
 * BLOCKER (c) -- TWO SYMBOLS AND ONE FAKEMATCH ROW.
 *
 * REPORTED, NOT ADDED -- AN OPEN DECISION:
 *
 *     _MSG_970 = 0x970;
 *
 * 0x97 << 4, shiftable.  Controls in the SAME function: 0x968, 0x969, 0x96a,
 * 0x96b, 0x96f, 0x973, 0x976, 0x928, 0x948, 0x92d, 0x94d and 0x3e7 -- all
 * unshiftable, all pooled by gcc, all reproducing as literals.  Extra
 * evidence: the ROM then derives 0x971 and 0x972 as `add r0, r5, #1` /
 * `add r5, #2` from that register, and probing confirms gcc pools 0x971 and
 * 0x972 SEPARATELY from a literal 0x970 -- so the register-base form is
 * unreachable without the symbol.  Not added because 269 differences survive
 * it; it cannot be said to complete anything.
 *
 * REPORTED AS WEAKER -- DO NOT ADD ON THIS EVIDENCE ALONE: 0x1000 in
 * `flagid | 0x1000` (= 0x80 << 5) is a const.sym candidate, but it is the only
 * constant of its kind at that site, so it has NO IN-FUNCTION CONTROL.  Noting
 * the absence of a control explicitly, because that absence is the reason.
 *
 * A DOT-PREFIXED GLOBAL, AND THE CLAIM THIS PARK ORIGINALLY MADE ABOUT IT WAS
 * WRONG.  Its 6-entry byte table is a global symbol whose name starts with a dot:
 * `nm goldensun.elf` gives `0809e680 T .L9e680`, defined in
 * asm/rom_8a000/rom_8d9a4_c_c_c_c_c.s.  This park used to say "No C identifier can
 * name it" and reports/batch-280.md published it as "one permanent fakematch row
 * nobody can remove".  BOTH HALVES ARE FALSE, and the contradiction was sitting in
 * this very file -- the C below has always used
 * `extern const unsigned char tbl[] __asm__(".L9e680");`, which names it exactly.
 *
 * Batch 281 verified the mechanism independently on `.Lb4146` and `.Lb4ab2` in
 * rom_b0000, where objcmp confirms the relocation is byte-for-byte the reference's
 * R_ARM_ABS32.  And the construct is already in LANDED files -- six of them under
 * src/overlays/rom_77a7c8/ -- where it is not consistently booked in
 * fakematch.txt, so it is not established as fakematch debt either.
 *
 * So the asm-name attribute is an ordinary spelling for a symbol whose name is not
 * a valid C identifier, and this function carries no special naming burden.  The
 * real blockers are (a), (b) and the two symbols above.
 *
 * NEXT: (a) only.  It wants gcc-2.96's jump.c / cfg layout read against the
 * .19.flow2 dump to find whether ANY input produces test1/test2/body1/body2,
 * and if none does this is UNREACHABLE and should be said so.  (b) and (c) are
 * not worth touching until (a) is settled.
 */
extern unsigned char gState[];
extern unsigned char *iwram_3001ebc;
extern int ewram_2000434;
extern const unsigned char tbl[] __asm__(".L9e680");

struct Ev {
    int f0;
    unsigned short f4;
    short f6;
    int f8;
};

extern void Func_8091660(void);
extern void _Func_801776c(int a, int b);
extern void _SetFlag(int id);
extern void _ClearFlag(int id);
extern int _GetFlag(int id);
extern void _Func_8019908(int a, int b);
extern struct Ev *FindMapActorEvent(int a, unsigned int b);
extern int Func_808d428(int a);
extern void CutsceneStart(void);
extern void CutsceneEnd(void);
extern void CutsceneWait(int n);
extern void WaitFrames(int n);
extern void Func_808ece0(unsigned int a);
extern void Func_808ed1c(unsigned int a);
extern int Func_808ed4c(unsigned int a);
extern void Func_808ed78(unsigned int a);
extern void Func_808ec8c(unsigned int a);
extern void Func_808ec50(unsigned int a);
extern void Func_808f0d8(int a);
extern void _PlaySound(int id);
extern void Func_808c2dc(int a, int b);
extern void _Func_801ef08(int a);
extern void _Func_801f5d4(void);
extern void _Actor_SetAnim(int a, int b);
extern int GetEncounterGroup(int a, int b);
extern void Func_808b320(int a, int b);
extern int Func_808ef70(int a, int b);
extern void _AddCoins(int n);
extern void _DeleteActor(int a);
extern int _GiveItem(int item);
extern void DeleteMapActorPtr(int a);
extern void Func_809202c(void);
extern void Func_808ece0x(unsigned int a);

int Func_808d9a4(unsigned int arg)
{
    struct Ev *e;
    unsigned char *iw;
    unsigned char *gs;
    int actor;
    int actor2;
    int a2;
    unsigned int t;
    int ti;
    int idx;
    int flagid;
    int msg;
    int got;
    int m16;
    int ok;

    gs = gState;
    gs += 0xfa << 1;
    t = arg - 0xf2;
    actor = *(int *)gs;
    if (t <= 5) {
        Func_8091660();
        ti = tbl[t];
        _Func_801776c(0x928 + ti, 1);
        _Func_801776c(0x948 + ti, 1);
        return 0;
    }
    e = FindMapActorEvent(3, arg);
    if (e != 0) {
        idx = (e->f0 >> 4) & 0x1f;
        flagid = e->f6;
        if ((e->f4 & (0x80 << 3)) == 0 && idx != 0) {
            Func_8091660();
            _Func_801776c(idx + 0x928, 1);
            _SetFlag(0xa1 << 1);
        } else {
            _ClearFlag(0xa1 << 1);
        }
        if ((e->f8 & (0xf0 << 20)) == 0) {
            if ((e->f8 & 0xfff00000) == (0x80 << 15)) {
                if (Func_808d428(flagid) != 0)
                    _Func_801776c(e->f8 & 0xffff, 1);
                else
                    _Func_801776c(0x976, 1);
            } else {
                CutsceneStart();
                if (Func_808d428(flagid) != 0) {
                    ok = 1;
                    if ((e->f8 & (0xf0 << 12)) == (0x80 << 9) && actor <= 7)
                        ok = 0;
                    if (ok != 0) {
                        if ((e->f0 & 0x1ff) == 0x13)
                            Func_808ece0(arg);
                        if ((e->f8 & 0xfff00000) == (0xc0 << 14)) {
                            if ((e->f0 & 0x1ff) == 0x13)
                                Func_808ed1c(arg);
                            actor2 = Func_808ed4c(arg);
                            Func_808f0d8(actor2);
                            _PlaySound(0x53);
                            _Func_8019908(e->f8 & 0xffff, 5);
                            msg = 0x970;
                            _Func_801776c(msg, 3);
                            Func_808c2dc(0x3e7, 0);
                            _Func_801ef08(1);
                            _PlaySound(0x7e);
                            _Func_801776c(msg + 1, 1);
                            _Func_801f5d4();
                            _Actor_SetAnim(actor2, 2);
                            _PlaySound(0xf6);
                            CutsceneWait(0x1e);
                            _Func_801776c(msg + 2, 1);
                            Func_808ed78(arg);
                            if (flagid != -1)
                                _SetFlag(flagid);
                        } else if ((e->f8 & 0xfff00000) == (0xa0 << 15)) {
                            iw = iwram_3001ebc;
                            if ((e->f0 & 0x1ff) == 0x13)
                                Func_808ec8c(arg);
                            if (flagid != -1) {
                                flagid |= 0x1000;
                                *(unsigned short *)(gState + (0x8d << 2)) = flagid;
                            }
                            *(unsigned short *)(iw + (0xbe << 1)) =
                                GetEncounterGroup(0x63, e->f8 & 0xffff);
                            gState[0x22b] = 2;
                            Func_808b320(0x63, e->f8 & 0xffff);
                            _PlaySound(*(short *)(gState + (0xf7 << 1)));
                            _Func_801776c(0x973, 1);
                        } else if ((e->f8 & 0xfff00000) == (0x80 << 14)) {
                            gs = gState;
                            gs += 0xfa << 1;
                            a2 = Func_808ef70(*(int *)gs, 0);
                            WaitFrames(0x1e);
                            if ((e->f0 & 0x1ff) == 0x13)
                                Func_808ed1c(arg);
                            Func_808f0d8(a2);
                            _PlaySound(0x53);
                            _Func_8019908(e->f8 & 0xffff, 5);
                            _Func_801776c(0x969, 3);
                            _AddCoins(e->f8 & 0xffff);
                            if (flagid != -1)
                                _SetFlag(flagid);
                            _DeleteActor(a2);
                        } else {
                            a2 = Func_808ef70(ewram_2000434, e->f8 & 0xfff);
                            WaitFrames(0x1e);
                            got = _GiveItem(e->f8 & 0xffff);
                            m16 = 0xffff;
                            if (got == -1) {
                                _Func_8019908(e->f8 & 0xfff, 2);
                                msg = 0x968;
                                _Func_801776c(msg, 1);
                                _Func_801776c(msg + 4, 1);
                                DeleteMapActorPtr(a2);
                                if ((e->f0 & 0x1ff) == 0x13)
                                    Func_808ec50(arg);
                            } else {
                                if ((e->f0 & 0x1ff) == 0x13)
                                    Func_808ed1c(arg);
                                Func_808f0d8(a2);
                                _PlaySound(0x53);
                                _Func_8019908(e->f8 & m16, 2);
                                if (got == ewram_2000434) {
                                    _Func_801776c(0x96a, 3);
                                } else {
                                    _Func_8019908(got, 1);
                                    _Func_801776c(0x96b, 3);
                                }
                                if (flagid != -1)
                                    _SetFlag(flagid);
                                _DeleteActor(a2);
                            }
                        }
                    } else {
                        _Func_801776c(0x96f, 1);
                    }
                } else {
                    _Func_801776c(idx + 0x948, 1);
                }
                CutsceneEnd();
                Func_809202c();
            }
        } else {
            if (Func_808d428(flagid) != 0) {
                gs = gState;
                gs += 0xfa << 1;
                ((void (*)(int))e->f8)(*(int *)gs);
            }
            if (_GetFlag(0xa1 << 1) != 0)
                _Func_801776c(idx + 0x948, 1);
        }
    } else {
        _Func_801776c(0x92d, 1);
        _Func_801776c(0x94d, 1);
    }
    _ClearFlag(0xa1 << 1);
    return 0;
}
