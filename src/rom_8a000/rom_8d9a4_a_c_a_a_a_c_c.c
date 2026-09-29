/* Func_808e680 (0x0808e680) -- MATCHES.  748 bytes, 311 encodings and 51
 * relocations identical to asm/rom_8a000/rom_8d9a4_a_c_a_a_a_c_c.s.
 *
 * Needs ONE message.sym line -- `_MSG_920 = 0x920;` -- and nothing else.  With
 * it the object is byte-identical; without it the single difference is the pool
 * word (`ldr r0, =_MSG_920` against `=0x920`) and its relocation.  See the
 * symbol section below: the evidence was already recorded in the batch-282 park
 * and only the COMPLETION test was missing, which this file now passes -- the
 * same position batch 282 reached for _MSG_820.
 *
 * Verify with (the .equ stands in for the message.sym line, per _MSG_1299's note
 * -- objcmp assembles the candidate directly and cannot see a linker script):
 *   python3 tools/objcmp.py <cand.c> \
 *     asm/rom_8a000/rom_8d9a4_a_c_a_a_a_c_c.s --func Func_808e680
 * The reference holds this function ALONE and `tools/datacheck.py` reports no
 * data section, so no split and no export list.
 *
 * ================================================================
 * WHAT CLOSED IT, AND IT WAS A PARK BUG, NOT A LEVER
 * ================================================================
 *
 *     extern int Func_8096b28(int a, int b, int c);       -- NOT `void`
 *
 * One word, 4 encodings -> 0.  The park declared this callee `extern void`.
 * The callee is DEFINED in this tree as `int Func_8096b28(int *arg0, int arg1,
 * int arg2)` (src/rom_8a000/rom_944ec_a_c_c_a_b.c:16), and the file's own
 * NEIGHBOUR IN THE SAME asm STEM FAMILY already records the lever verbatim:
 * src/rom_8a000/rom_8d9a4_a_c_a_a_a_c_b.c says "Func_8096b28 declared to return
 * `int`, which emits r0 after the `ldr r2,[sp]` at both of its call sites.
 * Declared void: 8 of 64."  So the park's own file stem contained the answer.
 * src/rom_a1000/rom_a47b4_a_b.c records the same for Func_80a10d0 and batch 99
 * corrected it there to "the RETURN TYPE decides it".
 *
 * THE MECHANISM, which neither of those notes gives, read out of .23.sched2 and
 * haifa-sched.c -- it is the general reason the int-return lever works:
 *
 *   A VOID CALL IS `*call_insn`, WHICH NEVER **SETS** r0.  It only `(use)`s it.
 *   So after the call `reg_last_sets[r0]` still names the call's OWN r0 ARGUMENT
 *   FILL, and the next insn that defines r0 -- invariably the next call's r0
 *   argument -- takes a REG_DEP_OUTPUT on that fill.  The fill therefore carries
 *   ONE MORE INSN_DEPEND entry than every other argument fill of the same call.
 *   rank_for_schedule (haifa-sched.c) compares, in order: priority, then class
 *   relative to last_scheduled_insn, then DEPENDENT COUNT, then INSN_LUID.  Every
 *   argument fill of a call has priority `1 + priority(call)` -- arm_adjust_cost
 *   (arm.c, "Call insns don't incur a stall, even if they follow a load") returns
 *   1 for ANY true dependence into a CALL_INSN, whatever the producer's latency --
 *   so priority always TIES and the dependent count decides.  r0's fill wins and
 *   is emitted FIRST.  Declared `int`, the call is `*call_value_insn`, it SETS r0,
 *   the output dependence attaches to the CALL instead, the counts tie, and
 *   INSN_LUID -- i.e. the argument order gcc actually emitted -- decides.
 *
 * MEASURED HERE, both sites at once:
 *   site 1 (idx 242/243), insns 549 `ldr r2,[sp,#8]` / 551 `ldr r0,[sp,#4]`:
 *     void   prio 36/36, depend_count 2 / **3** (551 gains `movs r0,#160`)
 *     int    prio 36/36, depend_count 2 / **2**, and the call is code 240
 *   site 2 (idx 259/260), insns 591 `ldr r2,[sp,#8]` / 593 `mov r0,r8`:
 *     void   prio 35/35, depend_count 2 / **3** (593 gains `mov r0,r5`)
 *     int    prio 35/35, depend_count 2 / **2**
 *
 * A SECOND ROUTE TO THE SAME THING, worth recording because it generalises to
 * functions whose callee really is void: an empty `__asm__ volatile ("")`
 * PLACED AFTER THE CALL also equalises the counts, because a traditional asm
 * (ASM_INPUT, no operands) is analysed as setting every register
 * (haifa-sched.c, "Traditional and volatile asm instructions must be considered
 * to use and clobber all hard registers"), so the post-call r0 definition takes
 * its output dependence on the asm and every fill gains the asm equally.
 * Measured: barrier after site 1 alone, 4 -> 2 (site 1 closed).  It is NOT free
 * -- after site 2 it costs 3 (4 -> 7), because it also detaches `mov r0,r5` from
 * `lsls r5,r5,#1` and lets the r5 pair sink below the call.  The prototype is the
 * clean fix; the barrier is the fallback when the callee is genuinely void.
 *
 * ================================================================
 * THE OTHER LEVERS, all re-measured FROM EXACT on this file
 * ================================================================
 *
 * POOL PLACEMENT IS THE DOMINANT BLOCKER ABOVE ~250 INSTRUCTIONS AND IT MASKS
 * EVERYTHING BEHIND IT.  At 748 bytes the pool does not fit after the epilogue
 * by accident: `*(unsigned short *)(iw + (0xb8 << 1)) = 0x3e7;` makes 0x3e7 a
 * HImode entry with a 32-60 byte range, it becomes the pool's FIRST entry, and
 * arm_reorg dumps an interior pool -- every later PC-relative offset shifts.
 * Routing it through an `int` local makes the load SImode (1020 bytes) and the
 * whole pool goes to the end.  Ablations from exact:
 *   drop the `int m3e7` carrier entirely          187 differing, +4 bytes
 *   keep the carrier but assign it AT THE USE       5 differing
 * docs/elevation.md records only the harmful direction of this lever ("a wide
 * first entry pushes the whole pool to the end ... an `int` local for a constant
 * is actively harmful when the ROM's pool is mid-body").  WHEN THE ROM'S POOL IS
 * AT THE END THE `int` LOCAL IS REQUIRED, and it only pays once HOISTED ABOVE
 * THE DOMINATING BRANCH.  Same lever, two placements, opposite signs.
 *
 * A SEPARATE POINTER LOCAL PER gState SITE -- four sites here.  One shared
 * pointer: 27 differing.  Reusing one variable lengthens its live range, the
 * strength-reduced pointer lands in a long-lived scratch register and the loop
 * preheader mis-schedules.  This extends the neighbour's `gs = gState;
 * gs += 0xfa << 1;` idiom: THE IDIOM IS PER-SITE, NOT PER-FUNCTION.
 *
 * A 16-BIT MEMORY-TO-MEMORY COPY NEEDS TWO `int` TEMPS, ONE PER COPY.
 * Ablations from exact: one shared temp 3; `unsigned short` temps 218 and
 * +8 bytes (`mov r0,#0 / ldrsh rX,[r3,r0]` instead of `ldrh rX,[r3,#0]`);
 * direct memory-to-memory 206 and -4 bytes.  The direct form gives gcc the DEST
 * offset as the base of its constant chain (0x1c0 -> +0x80 -> +2); a temp gives
 * the SOURCE (0x240 -> -0x80 -> +2), which is the ROM.
 *
 * ANTI-TELL WORTH KEEPING: `ldrsh rX, [rBase, rZero]` with a materialised zero
 * is the NORMAL gcc output for a signed-short read at a computed address.  Three
 * sites here (`*(short *)(iw + (0xcf << 1))`) reproduce it unaided.  It looks
 * like a defect and is not.
 *
 * ================================================================
 * _MSG_920 = 0x920 -- THE SYMBOL, NOW ADMISSIBLE
 * ================================================================
 *
 * From `_Func_801776c(0x920, 0xd)`.  0x920 = 0x92 << 4 is SHIFTABLE, so
 * gcc-2.96 synthesises it as `mov r0,#146 / lsl r0,#4`; five spellings probed
 * (plain int, (int)(void*), unsigned short local, (unsigned short) cast, pointer
 * parameter) and all synthesise.  THE IN-FUNCTION CONTROL IS STRONG: the same
 * function passes 0x91e, 0x91f and 0x921 to the SAME callee, all three
 * unshiftable, gcc pools all three, and all three reproduce as plain literals.
 * Measured: literal 216 differing; symbol byte-exact.
 *
 * message.sym:527 records this candidate as withheld in batch 280 because it did
 * not COMPLETE its function -- "evidence quality and completion are separate
 * tests and this file requires both".  IT NOW COMPLETES THE FUNCTION.  That is
 * exactly the situation batch 282 resolved for _MSG_820, whose entry says the
 * batch-281 park "WAS WRONG ABOUT ITS OWN REASON FOR WITHHOLDING ... Batch 282
 * closed those 8, so it DOES complete the function, and on this tree's own
 * stated criterion -- evidence AND completion -- it qualifies."
 *
 * SHIMS: none of either class.  No `register ... __asm__` declaration and no
 * `__asm__(".equ ...")` in the landed file, so NO fakematch.txt row is needed.
 * (`extern unsigned char _MSG_920[];` is the symbol-address idiom, which
 * message.sym backs; the `.equ` is only for objcmp.)
 *
 * No per-file Makefile override applies to this stem; the figures above are on
 * the production flag group.
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
extern int Func_8096b28(int a, int b, int c);
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

