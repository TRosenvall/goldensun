/* Func_808e23c (0x0808e23c) -- NON-MATCHING, 64 encodings of 266 (30 diff
 * regions).  SIZE IS EXACT (632 bytes) and THE POOL IS EXACT -- 12 words in
 * the reference's order, gState at word 1 and iwram_3001ebc at word 6,
 * matching the reference's only two R_ARM_ABS32 at 0x24c and 0x260.
 *
 * Blocker class: global_alloc PRIORITY, and the formula says the ROM's order
 * is not reachable without a LIVE-RANGE change.
 *
 * Verify with:
 *   python3 tools/objcmp.py src/non_matching/rom_8a000/808e23c.c \
 *     asm/rom_8a000/rom_8d9a4_a_c_a_a_a_c_a_c.s --func Func_808e23c
 * NOTE: the reference .s holds TWO functions (Func_808e23c, Func_808e4b4), so
 * landing this one needs tools/split_s.py first.
 *
 * THE POOL WAS THE FIRST BLOCKER AND IT IS SOLVED.  3 pools / 652 bytes -> 1
 * pool / 632 by routing a HImode constant store through an `int` local and
 * hoisting it above the dominating branch.  See the fuller writeup in
 * src/non_matching/rom_8a000/808e680.c, its file-neighbour from the same
 * round: at this size an interior pool reports 200+ differences on an object
 * whose instruction stream is much closer than that, so FIX THE POOL BEFORE
 * READING ANY REGISTER.
 *
 * ONE VARIABLE SERVING TWO DISJOINT BRANCHES -- AND THE PRIORITY FORMULA
 * PREDICTED IT BEFORE IT WAS TRIED.  The outer-loop counter `j` and the
 * Func_8091d84 result are ONE variable: 50 -> 37, and the entire
 * 60-instruction search loop went exact.  Predicted from .17.lreg: merging
 * lifts REG_N_REFS 7->10 and REG_LIVE_LENGTH 54->60, so priority
 * floor_log2(refs)*refs/length goes 0.259 -> 0.5, passing `best` (0.326) and
 * `size` (0.267).  Predicted allocation ptr r5 > j r6 > best r7 > size r8 --
 * that is exactly the ROM.  Separately, the Func_808ddec result and `msg` are
 * also ONE variable: 50 -> 45.
 *
 * AND THE REVERSE DEFECT, IN THE SAME FUNCTION.  Reusing one variable for the
 * inner count AND the Func_8091d84 result made the inner count cross a call,
 * which excludes it from r4 in find_reg PASS 0 and produced a caller-save
 * str r4,[sp] / ldr r4,[sp] pair plus a third spill slot.  Separating them
 * removed the pair and the slot (92 -> 72).  MERGING AND SPLITTING ARE BOTH
 * LEVERS; which one applies is decided per value by whether it crosses a call.
 *
 * THE RESIDUE is entirely a register permutation in the item-message tail: a
 * 3-cycle (`e` r6<->r5, the Func_808ddec result r5<->r7, the 0x142 constant
 * r7<->r5, `msg` r5<->r7), plus the FieldMove_NoTarget block where the ROM
 * holds 1 in r5 and 0 in r8 across the call and ours has them swapped.
 *
 * WHY IT IS PARKED RATHER THAN CHASED.  Measured refs/lengths from .17.lreg:
 * msg 4/50 = 0.16, e 6/35 = 0.343, the 0x142 constant 3/28 = 0.107, iw 3/51 =
 * 0.059.  The ROM's order requires msg > e, i.e. msg's priority above 0.343,
 * WHICH ITS REF/LENGTH NUMBERS CANNOT REACH.  So this needs a live-range
 * change, not a spelling -- and the two available live-range merges are
 * already taken (above).
 *
 * MEASURED, spellings that tie or lose: one/zero assignment order 35,
 * declaration order 30 (no change), inlining iwram_3001ebc 64, a named fp
 * pointer without the constants 60.
 *
 * NEXT: a third live-range merge or split that lifts msg's REG_N_REFS, found
 * by reading .17.lreg for a value whose range could legitimately join msg's.
 * Belongs with the other global_alloc parks (Func_80a8578, Func_80cd52c,
 * Func_80919d8, Func_80a6794, Func_808b090, Func_80f6148).
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
            _Func_801776c(0x91c, 1);
            Func_8096fb0(msg, 0);
            fp = iw + 0xcc6;
            one = 1;
            zero = 0;
            *fp = one;
            FieldMove_NoTarget();
            *fp = zero;
            Func_8097194();
            if ((_GetItemInfo(item)[0xc] & one) != 0)
                _SetFlag(0x143);
        }
    }
    if (_GetFlag(0xa1 << 1) != 0)
        _Func_801776c(0x927, 1);
    if (_GetFlag(0x143) != 0)
        _Func_80788c4(unit, a1);
    return ret;
}
