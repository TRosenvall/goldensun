/* InitRenderTilemapBG1 -- 0x080cdd58.  *** EXACT *** (batch 316).
 *
 *   OK InitRenderTilemapBG1 -- 312 bytes, 129 encodings and 12 relocations identical
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/rom_c9000/rom_cd508_c_a_b.c \
 *     asm/rom_c9000/rom_cd508_c_a_b.s --func InitRenderTilemapBG1
 * (before the split, against the tracked multi-function reference:
 *     ... tools/objcmp.py <this file> asm/rom_c9000/rom_cd508_c_a.s \
 *       --func InitRenderTilemapBG1 )
 *
 * SPLIT: asm/rom_c9000/rom_cd508_c_a.s holds FOUR functions and this is the
 * FIRST, so `python3 tools/split_s.py asm/rom_c9000/rom_cd508_c_a.s
 * InitRenderTilemapBG1` writes _a_b.s (this function) + _a_c.s (DrawLine,
 * Anim_PlanetDiver, Anim_Haunt) and rewrites stage1.ld.  tools/datacheck.py
 * prints NOTHING for this file -- no data section, so the split is TEXT-ONLY
 * and no `.global` has to be added anywhere.
 *
 * SHIMS: THREE register pins (tools/shimcount.py says 3).  A fakematch.txt row
 * is required.  All three are load-bearing; single drops from this file:
 *     register int c   __asm__("r12")  ->  101 of 129 and -4 bytes
 *     register int n   __asm__("r1")   ->   11 of 129
 *     register int row __asm__("r4")   ->    6 of 129
 *
 * ================ HOW THE LAST 2 FELL: THE PARK'S DIAGNOSIS WAS WRONG =======
 *
 * The park stood at 2 of 129 for three batches and closed itself with
 * "THIS PARK IS AT ITS FLOOR AT 2 absent a way to add a dependent without an
 * instruction."  The residue was one adjacent transposition in the loop
 * preheader (objcmp indices 39/40):
 *
 *     ROM   mov ip, r2   (0x4694)   then   movs r4, #0  (0x2000)
 *     ours  movs r4, #0            then   mov ip, r2
 *
 * The SCHEDULING ANALYSIS WAS CORRECT and survives: both insns carry
 * INSN_PRIORITY 1 and an empty INSN_DEPEND list, they are both independent of
 * `movs r6,#0` (the insn scheduled just before, so the CLASS rung ties at 3),
 * and rank_for_schedule therefore falls all the way through to INSN_LUID.
 *
 * WHAT WAS WRONG WAS THE CONCLUSION DRAWN FROM IT.  The park reasoned that the
 * only way to break a full LUID fall-through is to add a DEPENDENT, i.e. an
 * instruction the ROM does not have.  There is a second way, and it is the
 * obvious one: CHANGE THE LUID.  LUID is chain position, so the fix is to get
 * `mov ip, r2` emitted earlier in the chain -- between `movs r6,#0` (i = 0) and
 * `movs r4,#0` (row = 0) -- rather than at the end of the preheader.
 *
 * `mov ip, r2` is reload's copy for the 0x100 loop invariant (Thumb cannot
 * shift into a hi register, so the value is built in r2 and copied).  Written
 * as a bare `0x100` inside the loop, the invariant is HOISTED BY loop.c's
 * move_movables, which uses emit_insn_before(loop_start) -- so it lands AFTER
 * everything expand already put in the preheader, and LUID(copy) > LUID(row=0)
 * BY CONSTRUCTION.  That is why every reordering of the source statements was
 * inert: none of them touch where loop.c puts its own insn.
 *
 * THE FIX IS TO TAKE THE INVARIANT AWAY FROM loop.c -- name it at source
 * position, in the hard register it ends up in:
 *
 *     register int c __asm__("r12");   (equivalently __asm__("ip"))
 *     ...
 *     o = 0;
 *     i = 0;
 *     c = 0x100;          <-- expand emits the build + copy HERE
 *     row = 0;
 *     for (; i != 0x10; i++) { ... t = row + c; ... }
 *
 * THE HARD REGISTER IS WHAT MAKES IT WORK, AND THAT IS THE TRANSFERABLE PART.
 * A PLAIN local `int c` measures 101 of 129 and -4 BYTES: cse/cprop propagates
 * the constant into the loop body, loop.c then declines to hoist it, and the
 * `mov ip,r2` / `mov r3,ip` pair disappears entirely (two instructions short).
 * A hard-register variable is not a cprop candidate, so the constant stays in
 * its register and the build+copy stay where the source put them.  So:
 *
 *   *** A SOURCE-NAMED LOOP INVARIANT IS A LUID LEVER ONLY IF IT IS PINNED. ***
 *   Unpinned it is a REGRESSION, because cprop undoes it.  This is the inverse
 *   of the usual reading of a pin (an allocation lever): here the pin's job is
 *   to keep a pseudo alive through cprop so that EXPAND ORDER, and therefore
 *   LUID, is what the source says.
 *
 * AND THE POSITION IS THE WHOLE POINT, which is the control that proves the
 * mechanism: the same pinned `c` assigned BEFORE `o = 0` instead of between
 * `i = 0` and `row = 0` gives 3 of 129 (first diff at index 37) -- the copy now
 * schedules one slot too EARLY.  r8/r9/r10/r11 instead of r12 are 56-57.
 *
 * WHAT WAS ALSO WRONG, SMALLER: the park's figure table reads its encodings as
 * decimal ("ref 4694", "ours 2400").  objcmp prints them as bare HEX digits;
 * 0x4694 is `mov ip,r2` and 0x2000 is `movs r4,#0`.
 *
 * REFUTED HERE, AND WORTH NOT RE-DERIVING: the park's "strength-reduction
 * hypothesis" was retried in its SPLIT form (`row = i * 0x10;` as its own
 * statement, then `t = row + 0x100;`), which the park had never tried -- it had
 * only tried the single expression `t = (i << 4) + 0x100`.  Seven spellings,
 * all 98 of 129 and -4 bytes: loop.c does not strength-reduce `i * 0x10` here
 * at all, it leaves `lsl r3, r4, #4` inside the loop and hoists 0x100 into a
 * LOW register, so the copy vanishes.  The split form is not a way in.
 *
 * ================ THE OTHER TWO PINS, UNCHANGED FROM THE PARK ================
 *  (1) `register int n __asm__("r1")` for the size argument of the last two
 *      calls -- worth 11 from this file.  The ROM re-materialises
 *      `mov r1,#0x80 / lsl r1,#7` at BOTH calls; cse1 unifies the two pseudos
 *      (a CONST_INT costs 0, so a REG at cost 0 ties and wins), local_alloc
 *      then gives the survivor a CALLEE-SAVED register because it crosses the
 *      call, which steals r5 from the function pointer.  A hard call-clobbered
 *      register is invalidated by invalidate_for_call, so the pin breaks the
 *      unification.  Eight source spellings of the same value are all inert.
 *  (2) `register int row __asm__("r4")` -- worth 6, pass is global_alloc.
 *      .18.greg's priority order is `43 41 42 60 45 40 62 58 37`; the ROM needs
 *      row before base.  Both carry 7 loop-depth-weighted refs, so
 *      floor_log2(n_refs)*n_refs is 14 for each and live_length decides: base
 *      dies at the last inner store, row at the outer `row += 0x10`, so base's
 *      range is shorter and it wins.  Declaration order is inert in four
 *      permutations.
 *
 * NOTE: pin (1)'s mechanism is the SAME ONE that makes two individually-inert
 * pins jointly load-bearing in src/non_matching/ovl_787e04/2008578.c.
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
    register int c __asm__("r12");
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
    i = 0;
    c = 0x100;
    row = 0;
    for (; i != 0x10; i++) {
        j = 0;
        t = row + c;
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
