/* InitRenderTilemapBG1 -- NON-MATCHING, 2 ENCODINGS OF 129.
 *
 * SIZE EXACT (312 = 312), INSTRUCTION COUNT EXACT (129 = 129), RELOCATIONS EXACT
 * (objcmp prints no RELOCATIONS line).  So 2 IS A TRUE DISTANCE.
 * tools/datacheck.py reports no data section for rom_cd508_c.s; the split is
 * text-only, and Anim_PlanetDiver / Anim_Haunt stay in assembly beside DrawLine
 * and Anim_Confuse.
 *
 * Verify with:
 *   python3 tools/objcmp.py src/non_matching/rom_c9000/80cdd58.c \
 *     asm/rom_c9000/rom_cd508_c_a.s --func InitRenderTilemapBG1
 *   XX ENCODINGS differ in 2 place(s) (ref 129, ours 129)
 *      first at index 39: ref 4694  ours 2400
 *
 * THE RESIDUE IS ONE ADJACENT TRANSPOSITION IN THE LOOP PREHEADER:
 *
 *     [39] ref 4694 mov  ip, r2   | ours 2400 movs r4, #0
 *     [40] ref 2400 movs r4, #0   | ours 4694 mov  ip, r2
 *
 * `mov ip, r2` is reload's copy for the 0x100 loop invariant (thumb cannot shift
 * a hi register, so the value is built in r2 and copied to ip).
 *
 * ============ PASS: sched2, AND BATCH 314 PROVED IT FROM THE DUMP ============
 * THIS IS THE ONE PARK OF ITS BATCH WHOSE NAMED BLOCKER WAS CONFIRMED RATHER THAN
 * CORRECTED, and it now rests on the dependence table instead of inference.
 * `-fsched-verbose=6`, block 0's four tail insns, as
 * `;; insn code bb dep prio cost ... : <INSN_DEPEND>`:
 *
 *     ;;  88  173  0  1  1  1  core :        <- movs r0, #0   (o   = 0)
 *     ;;  91  173  0  2  1  1  core :        <- movs r6, #0   (i   = 0)
 *     ;;  93  173  0  4  1  1  core :        <- movs r4, #0   (row = 0)
 *     ;; 330  173  0  5  1  1  core :        <- mov  ip, r2   (reload's copy)
 *
 * ALL FOUR CARRY INSN_PRIORITY 1 AND AN EMPTY INSN_DEPEND LIST -- dependent count
 * 0 for every one.  So rank_for_schedule genuinely does fall through priority AND
 * dependent count to INSN_LUID, and the trace confirms it: the ready list prints
 * as `330 93 91 88` yet the picks are 88, 91, 93, 330 -- strict ascending LUID.
 * The LUID order is the pre-sched chain order, which is [expand's o/i/row inits]
 * then [loop.c's hoisted invariant + reload's copy], because loop.c inserts the
 * invariant with emit_insn_before(loop_start), i.e. AFTER everything expand
 * already put in the preheader.  The ROM needs the opposite order.
 *
 * TWO CORRECTIONS TO THE PREVIOUS REVISION OF THIS PARK: the priorities tie at
 * 1, NOT 0; and it asserted the LUID fall-through WITHOUT EVER CHECKING THE
 * DEPENDENT COUNT -- the exact step that cost five other parks their attribution
 * in the same batch.  Checked here, it ties at 0, so the conclusion holds.
 *
 * THE BRIEF'S ALIAS-SET DEPENDENT-COUNT LEVER CANNOT REACH THIS TIE.  It needs a
 * MEM as one of the two COMPETING insns, so that retyping the access widens its
 * alias set and picks up anti-dependences on the block's stores.  Here BOTH
 * competitors are register sets (`mov ip,r2`, `movs r4,#0`) and the block holds
 * no store either could take an anti-dependence on.  Breaking this tie needs an
 * extra DEPENDENT, i.e. an instruction the ROM does not have.
 *
 * ============ BATCH 314: 12 MORE SPELLINGS, FLOOR STILL 2 ============
 *   `o` folded into the for-init                        2  INERT
 *   `t = 0x100 + row`                                   2  INERT
 *   `extern int Func_80008d4` / `extern int WaitFrames` 2  INERT
 *     (Func_80008d4 is the ONLY callee here declared in a tracked header --
 *      libcamelot.h:6 `void Func_80008d4(void*, u32);` -- and it AGREES with this
 *      file, so the return-type lever has no evidence base and measures inert.)
 *   `for (row = 0, i = 0; ...)` / loop counters unsigned 3
 *   `extern int _Func_80c0774`                          4
 *   the r4 pin dropped                                  7  (so it is worth 5)
 *   `o` pinned to r0                                   14
 *   THE STRENGTH-REDUCTION IDEA, three spellings        98 and -4 BYTES
 *
 * THE STRENGTH-REDUCTION HYPOTHESIS, AND WHY IT FAILED -- RECORDED SO IT IS NOT
 * RE-DERIVED.  `row` is a SOURCE biv whose `row = 0` expand emits into the
 * preheader BEFORE loop.c appends the hoisted 0x100, which is precisely why
 * LUID(row=0) < LUID(copy).  If `row` were instead a loop.c STRENGTH-REDUCTION
 * product (`t = (i << 4) + 0x100`, with `row` and `row += 0x10` deleted), then
 * strength_reduce -- which runs AFTER move_movables -- would emit the giv's
 * initialisation with its own emit_insn_before(loop_start), landing it CLOSER to
 * loop_start and therefore LATER in the chain than the hoisted invariant: the
 * ROM's order.  Measured at 98 of 129 and -4 bytes in all three spellings
 * (`i << 4`, `i * 0x10`, with and without the r4 pin).  loop.c does not reduce it
 * to the ROM's shape; it removes the add entirely.
 *
 * ============ TWO HARD-REGISTER PINS ARE LOAD-BEARING ============
 * Without them this file is 18 of 129 (same size, same count).  Reported, not
 * smuggled; tools/shimcount.py counts 2.
 *  (1) `register int n __asm__("r1")` for the size argument of the last two calls
 *      -- worth 8.  The ROM re-materialises `mov r1,#0x80 / lsl r1,#7` at BOTH
 *      calls; cse1 unifies the two pseudos (a CONST_INT costs 0, so a REG at cost
 *      0 ties and wins), local_alloc then gives the survivor a CALLEE-SAVED
 *      register because it crosses the call, which steals r5 from the function
 *      pointer.  A hard call-clobbered register is invalidated by
 *      invalidate_for_call, so the pin breaks the unification.  CONTROL: giving
 *      the second call a different constant reproduces the whole tail with no pin.
 *      Eight source spellings of the same value are ALL INERT.
 *  (2) `register int row __asm__("r4")` -- worth 5, and the pass is global_alloc.
 *      .18.greg prints the priority order as `43 41 42 60 45 40 62 58 37`; the ROM
 *      needs row before base.  Both carry 7 loop-depth-weighted refs, so
 *      floor_log2(n_refs)*n_refs is 14 for each and live_length decides: base dies
 *      at the last inner store, row at the outer `row += 0x10`, so base's range is
 *      shorter and it wins.  Closing it needs one MORE weighted reference on row
 *      (7 -> 8 flips floor_log2 and the numerator to 24), i.e. an instruction the
 *      ROM does not have.  Declaration order is inert in four permutations.
 *
 * NOTE: pin (1)'s mechanism is the SAME ONE that makes two individually-inert
 * pins jointly load-bearing in src/non_matching/ovl_787e04/2008578.c -- there it
 * takes TWO pins to break the unification, because unification needs two unpinned
 * peers.  See that park's header for the grouping rule any depinning pass needs.
 *
 * A pinned landing needs a fakematch.txt row.
 *
 * THIS PARK IS AT ITS FLOOR AT 2 absent a way to add a dependent without an
 * instruction.
 */
#include "gba/io.h"

extern unsigned char iwram_3001e74[];
extern unsigned char iwram_3001ad0[];
extern void Func_80cd508(void);
extern void _Func_80c0774(int a, unsigned short b, int c);
extern void Func_80008d4(void *dst, int n);
extern void Func_80008d8(void *dst, int n, int v);
extern void WaitFrames(unsigned int n);

void InitRenderTilemapBG1(void)
{
    int (*fp)(void *, int);
    int (*fq)(void *, int, int);
    void (*fr)(void *, int);
    unsigned char *st;
    unsigned char *ad0;
    unsigned char *dst;
    unsigned char *tbl;
    unsigned char *base;
    int i, j, o, t, z, zr;
    register int row __asm__("r4");

    st = iwram_3001e74;
    tbl = *(unsigned char **)st;
    dst = *(unsigned char **)(st + 0x7c);
    base = *(unsigned char **)(st + 0x8c);
    Func_80cd508();
    _Func_80c0774(2, *(unsigned short *)(tbl + (0xc9 << 3)), 0);
    ad0 = iwram_3001ad0;
    zr = 0;
    z = 0x20;
    *(unsigned short *)(ad0 + 6) = z;
    *(int *)(base + 0xc) = zr;
    fp = Func_80008d4;
    fp((void *)0x6003fc0, 0x40);
    fq = Func_80008d8;
    fq((void *)0x600f900, 0x80 << 2, -1);
    o = 0;
    for (i = 0, row = 0; i != 0x10; i++) {
        j = 0;
        t = row + 0x100;
        for (; j != 0x20; j++) {
            if (j > 0xf)
                *(volatile unsigned short *)(o + 0x600fb00) = 0xff;
            else
                *(volatile unsigned short *)(o + 0x600fb00) = t;
            t++;
            o += 2;
        }
        row += 0x10;
    }
    REG_DISPCNT = 0x7741;
    REG_BG1CNT = 0x1f81;
    REG_BLDCNT = 0x3f42;
    REG_WIN0H = 0xf0;
    REG_WIN0V = 0x1088;
    REG_WIN1H = 0xf0;
    REG_WIN1V = 0x1088;
    REG_WININ = 0x3537;
    REG_WINOUT = 0x3f21;
    REG_BLDALPHA = 0x100e;
    fr = Func_80008d4;
    {
        register int n __asm__("r1");
        n = 0x80 << 7;
        fr(dst, n);
        n = 0x80 << 7;
        fr((void *)0x6004000, n);
    }
    WaitFrames(1);
}
