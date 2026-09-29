/* Func_808e680 (0x0808e680) -- NON-MATCHING, 5 encodings of 311.
 * SIZE IS EXACT (748 bytes) and the pool is a single end pool with 14 words in
 * the reference's exact order.
 *
 * Blocker class: sched2 -- two r0-argument-fill scheduling slots.  ONE of the
 * five is the _MSG_920 pool word (see below), so the instruction-level residue
 * is FOUR.
 *
 * Verify with:
 *   python3 tools/objcmp.py src/non_matching/rom_8a000/808e680.c \
 *     asm/rom_8a000/rom_8d9a4_a_c_a_a_a_c_c.s --func Func_808e680
 * The reference .s holds this function alone, so no split is needed to land it.
 *
 * THE FINDING OF THIS FUNCTION IS A CORRECTION TO docs/elevation.md, and it
 * matters for every function in the 250-400 instruction band:
 *
 *   POOL PLACEMENT BECOMES THE DOMINANT BLOCKER BEFORE ANY INSTRUCTION-LEVEL
 *   DEFECT, AND IT MASKS EVERYTHING BEHIND IT.  Below ~250 instructions the
 *   literal pool fits after the epilogue automatically.  At 748 bytes it does
 *   not: arm_reorg dumps an interior pool, every subsequent PC-relative offset
 *   shifts, and objcmp reports 200+ differing encodings on an object whose
 *   INSTRUCTION STREAM is five differences away.  Measured here: 216 differing
 *   with an interior pool against 5 with a single end pool -- same C except one
 *   constant.
 *
 * THE LEVER, AND IT RUNS THE OPPOSITE WAY TO THE DOCUMENTED ONE.
 * `*(unsigned short *)(iw + (0xb8 << 1)) = 0x3e7;` makes 0x3e7 a HImode pool
 * entry with a 32-60 byte range, so it becomes the pool's first entry and
 * arm_reorg dumps early.  Routing it through an `int` local makes the load
 * SImode (1020 bytes) and the whole pool goes to the end: 2 pools / 752 bytes
 * -> 1 pool / 748, 14 words, order exact.
 *
 * docs/elevation.md records only the HARMFUL direction -- "A wide first entry
 * pushes the whole pool to the end of the function.  This is what makes an
 * `int` local for a constant actively harmful when the ROM's pool is
 * mid-body."  The converse is now measured: WHEN THE ROM'S POOL IS AT THE END,
 * THE `int` LOCAL IS REQUIRED, and at this size it is the only thing that
 * makes the size match at all.  Same lever, two placements, opposite signs.
 *
 * AND IT NEEDED PAIRING.  The `int` local alone was WORSE (5 -> 10 diffregion).
 * It only paid once HOISTED ABOVE THE DOMINATING BRANCH (10 -> 5).
 *
 * THE REMAINING FOUR are two r0-argument-fill scheduling slots at the two
 * Func_8096b28(a_or_b, unit, x) sites: the ROM emits `ldr r2,[sp,#8]` before
 * `ldr r0,[sp,#4]` / before `mov r0,r8`; ours emits r0 first.  LUID order
 * (r0, r1, r2, from load_register_parameters) is ours; the ROM's r2-first
 * implies x's MEM was precomputed into a pseudo ahead of the register fills.
 * That needs a memory dependence or a block boundary and NEITHER IS AVAILABLE
 * HERE.  Four spellings tie at 5 -- prototype dropped entirely, prototype
 * taking `struct Ev *`, a temp for x read before the call, and the plain form
 * -- so it is not a spelling.
 *
 * TWO LEVERS THAT GOT IT THERE:
 *   A SEPARATE POINTER LOCAL PER gState SITE -- four sites here: 39 -> 9.
 *   Reusing one variable across sites lengthens its live range; the
 *   strength-reduced pointer then lands in a long-lived scratch register and
 *   the loop preheader mis-schedules.  This EXTENDS the neighbour's
 *   `gs = gState; gs += 0xfa << 1;` idiom: THE IDIOM IS PER-SITE, NOT
 *   PER-FUNCTION.
 *
 *   A 16-BIT MEMORY-TO-MEMORY COPY NEEDS TWO `int` TEMPS, ONE PER COPY.  The
 *   gState 0x240->0x1c0 / 0x242->0x1c2 block: the direct form gives gcc the
 *   DEST offset as the base of its constant chain (0x1c0 -> +0x80 -> +2); a
 *   temp gives the SOURCE (0x240 -> -0x80 -> +2, the ROM).  `unsigned short v`
 *   produces `mov r0,#0 / ldrsh rX,[r3,r0]`; `int v` gives the ROM's
 *   `ldrh rX,[r3,#0]`.  One SHARED temp still coalesces the value into the
 *   address register -- two temps make the five-instruction block exact.
 *   11 -> 6.
 *
 * ANTI-TELL WORTH RECORDING: `ldrsh rX, [rBase, rZero]` with a materialised
 * zero is the NORMAL gcc output for a signed-short read at a computed address.
 * Three sites here (*(short *)(iw + (0xcf << 1))) reproduce it unaided.  It
 * looks like a defect and is not.
 *
 * SYMBOL NEEDED AND DELIBERATELY NOT ADDED -- AN OPEN DECISION:
 *
 *     _MSG_920 = 0x920;
 *
 * From `_Func_801776c(0x920, 0xd)`.  0x920 = 0x92 << 4 is SHIFTABLE, so
 * gcc-2.96 emits `mov r0,#146 / lsl r0,#4`; five spellings probed (plain int,
 * (int)(void*), unsigned short local, (unsigned short) cast, pointer
 * parameter) and all synthesise.  THE INTERNAL CONTROL THE DOCS REQUIRE IS
 * PRESENT AND STRONG: the same function passes 0x91e, 0x91f and 0x921 to the
 * SAME callee; all three are unshiftable, gcc pools them, and all three
 * reproduce as plain literals.  Measured: literal 216 differing encodings,
 * symbol 5 with SIZE exact.
 *
 * Not added because it does not COMPLETE the function -- the four sched2
 * differences survive it.  Reported, per this tree's practice.  The C below is
 * written WITH the symbol and needs that message.sym line (or a top-level
 * `__asm__(".equ _MSG_920, 0x920");` shim) to reproduce the 5.
 *
 * NEXT: the two r0 fills, and they want sched2 read against .23.sched2 rather
 * than more spellings.  Same class as ovl_77a7c8/200a8e8.c, landed in the same
 * round -- if either ever closes, try the other's handle first.
 */
extern unsigned char gState[];
extern unsigned char *iwram_3001ebc;
extern unsigned char _MSG_920[];

struct Ev {
    int f0;
    unsigned short f4;
    short f6;
    int f8;
};

extern unsigned char *_GetMoveInfo(int id);
extern unsigned char *_GetUnit(int id);
extern int GetFieldActor(int a);
extern void Func_8091660(void);
extern void _ClearFlag(int id);
extern void _SetFlag(int id);
extern int _GetFlag(int id);
extern void _Func_8019908(int a, int b);
extern void _Func_801776c(int a, int b);
extern int Func_8091d84(int a);
extern void _Func_8019a54(void);
extern int Func_808e5d8(unsigned int a);
extern void _ModifyPP(int a, int b);
extern struct Ev *Func_808e4b4(int a, int b, int *c);
extern int Func_808df1c(int a, int b);
extern void Func_808b8e8(void);
extern void Func_8096fb0(int a, int b);
extern void Func_80970f8(int a, int b);
extern void Func_809728c(void);
extern void Func_8096b28(int a, int b, int c);
extern void FieldMove_Target(void);
extern void FieldMove_NoTarget(void);
extern void Func_8097174(void);
extern void Func_8096ab0(void);
extern void Func_8097194(void);
extern void Func_808b98c(void);

int Func_808e680(unsigned int arg)
{
    unsigned char *iw;
    unsigned char *gs1;
    unsigned char *gs2;
    unsigned char *gs3;
    unsigned char *gs4;
    unsigned short *sp1;
    struct Ev *a;
    struct Ev *b;
    struct Ev *c;
    int x;
    int item;
    int unit;
    int kind;
    int flagS;
    int ok;
    int cost;
    int w;
    int m3e7;
    int v;
    int w2;

    item = arg & 0x3ff;
    iw = iwram_3001ebc;
    kind = _GetMoveInfo(item)[0xc];
    unit = (arg >> 10) & 0xf;
    gs1 = gState;
    gs1 += 0xfa << 1;
    GetFieldActor(*(int *)gs1);
    flagS = 0;
    Func_8091660();
    _ClearFlag(0x145);
    if (unit == 0xf)
        unit = 0;
    if (_GetFlag(0xbf << 1) != 0) {
        _Func_8019908(unit, 1);
        _Func_8019908(item, 4);
        _Func_801776c(0x91f, 1);
        return 0;
    }
    if (*(short *)(iw + (0xcf << 1)) == 3 && item == 0x90) {
        _Func_8019908(unit, 1);
        _Func_8019908(0x90, 4);
        _Func_801776c(0x91f, 1);
        return 0;
    }
    m3e7 = 0x3e7;
    if (item == 0x95) {
        if (_GetFlag(0xa2 << 1) != 0) {
            _Func_8019908(unit, 1);
            _Func_8019908(0x95, 4);
            _Func_801776c(0x921, 1);
            return 0;
        }
        _Func_8019908(0x95, 4);
        _Func_801776c((int)_MSG_920, 0xd);
        ok = Func_8091d84(1);
        _Func_8019a54();
        if (ok != 0)
            return 0;
        gs2 = gState;
        v = *(unsigned short *)(gs2 + 0x240);
        *(unsigned short *)(gs2 + 0x1c0) = v;
        w2 = *(unsigned short *)(gs2 + 0x242);
        *(unsigned short *)(gs2 + 0x1c2) = w2;
        *(unsigned short *)(iw + (0xb8 << 1)) = m3e7;
        flagS = 1;
    }
    w = arg & (0x80 << 6);
    if (w != 0)
        return Func_808e5d8(arg);
    if (unit <= 7) {
        cost = _GetMoveInfo(item)[9];
        if (*(short *)(_GetUnit(unit) + 0x3a) < cost) {
            _Func_8019908(unit, 1);
            _Func_8019908(item, 4);
            _Func_801776c(0x91e, 1);
            if (flagS != 0)
                *(unsigned short *)(iw + (0xb8 << 1)) = w;
            return 0;
        }
        _ModifyPP(unit, -cost);
    }
    a = Func_808e4b4(0x10000005, kind, &x);
    b = Func_808e4b4(5, kind, &x);
    c = Func_808e4b4(0x50000005, kind, &x);
    x = -1;
    _SetFlag(0xa0 << 1);
    _SetFlag(0x141);
    if (a != 0 || b != 0 || c != 0) {
        gs3 = gState;
        gs3 += 0xfa << 1;
        x = Func_808df1c(*(int *)gs3, kind);
        if (b != 0 && (b->f4 & (0x80 << 3)) != 0) {
            _ClearFlag(0xa0 << 1);
            _ClearFlag(0x141);
        }
    } else {
        _ClearFlag(0x141);
    }
    if (*(short *)(iw + (0xcf << 1)) == 3)
        Func_808b8e8();
    Func_8096fb0(item, 0);
    iw[0xcc6] = 1;
    gs4 = gState;
    gs4 += 0xfa << 1;
    Func_80970f8(*(int *)gs4, x);
    Func_809728c();
    Func_8096b28((int)a, unit, x);
    if (_GetFlag(0xa0 << 1) != 0) {
        if (_GetFlag(0x141) != 0)
            FieldMove_Target();
        else
            FieldMove_NoTarget();
    }
    Func_8097174();
    Func_8096b28((int)b, unit, x);
    if (_GetFlag(0xa0 << 1) != 0)
        Func_8096ab0();
    _ClearFlag(0xa0 << 1);
    _ClearFlag(0x141);
    iw[0xcc6] = 0;
    Func_8097194();
    if (*(short *)(iw + (0xcf << 1)) == 3)
        Func_808b98c();
    return 0;
}
