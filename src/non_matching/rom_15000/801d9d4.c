/* Func_801d9d4 (DrawSubScreen) -- 0x0801d9d4, asm/rom_15000/rom_1ca1c_c_c_c.s
 *
 * NON-MATCHING: 2 encodings of 184 differ (objcmp).  PIN-FREE, SHIM-FREE, NO asm.
 * Was 41 when batch 324 opened it.  41 -> 36 -> 30 -> 4 -> 2.
 *
 * SIZE EXACT (412 bytes both).  INSTRUCTION COUNT EXACT (176 = 176).  objcmp
 * reports NO relocation difference -- all 14 `bl` offsets and all 4 ABS32 pool
 * words are the ROM's, at the ROM's byte offsets.  Only two ENCODINGS differ,
 * and they are ONE ADJACENT TRANSPOSITION of two independent instructions.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_15000/801d9d4.c \
 *     asm/rom_15000/rom_1ca1c_c_c_c.s --func Func_801d9d4
 *
 * The only non-obvious declaration is
 * `extern unsigned char L367dc[] __asm__(".L367dc");` -- the tree's established
 * spelling for a `.L` local label (src/rom_c9000/rom_dd2ac_c_c_b.c:123).
 *
 * SPLIT SHAPE.  tools/datacheck.py, verbatim:
 *     data sections : .rodata
 *     functions     : Func_801d9d4, StartMenu_Main, Func_801dd28
 *     EXPORTS       : .L367c9, .L367cc, .L367ce, .L367d0, .L367d6, .L36750
 *                     (already global -- NOT the set a split needs)
 *     Func_801d9d4                 reads .L367dc
 *                                  *** SPLIT MUST EXPORT: .global .L367dc
 *     StartMenu_Main               reads .L367dc
 *                                  *** SPLIT MUST EXPORT: .global .L367dc
 * tools/split_s.py refuses until that export exists, verbatim:
 *     REFUSING to split: 1 local label(s) would cross files.
 *     asm/rom_15000/rom_1ca1c_c_c_c_b.s references .L367dc, defined in
 *     asm/rom_15000/rom_1ca1c_c_c_c_c.s
 * So the split needs SEVEN exports, not six -- the previous header said this and
 * it is CONFIRMED by both tools.  Data is a clean tail cut.
 *
 * ================= WHAT BATCH 324 CHANGED, AND WHAT IT MEASURED =============
 *
 *   step  edit                                                        figure
 *   ----  ----------------------------------------------------------  ------
 *   park  as shipped                                                      41
 *   v1    `s->obj = Func_801eadc(...)` direct store; `&s->obj` at the
 *         call; the `pp` LOCAL DELETED                                    36
 *   v2    + `rows = 1;` before `sel = 2;`  AND  `h` before `y`            30
 *   v3    + `y` before `h` (reverting half of v2) AND `tx = bx->x * 8`
 *         named before `ty`                                                4
 *   v4    + final loop as three statements in the order q, n, p            2
 *
 * THE PARK'S DIAGNOSIS IS REFUTED.  It said: *"Every one of the 41 differing
 * encodings is the same instruction with a different register number ... This is
 * the corpus's REG_ALLOC_ORDER class ... and nothing tried here moves it."*
 * Two of its nineteen runs were not that at all -- indices 44-46 were three
 * IDENTICAL instructions in a different ORDER, and indices 125-135 were a
 * different instruction SEQUENCE -- and those two runs were 25 of the 41.
 *
 * ITS TWO "MEASURED" NEGATIVES WERE NEGATIVES FOR WANT OF A PREREQUISITE.
 * The park records `rows = 1;` before `sel = 2;` as **+2, worse**.  On top of v1
 * the same edit is **-2**, and crossed with `h before y` (itself -4) the pair is
 * **-6**: crossfire depth 2 from v1 reads base 36, `rows=1 first` 34,
 * `h before y` 32, BOTH 30.
 *
 * AND `h before y` WAS ITSELF ONLY HALF A FIX.  It is load-bearing while
 * `bx->x * 8` is written inline at the call, and a 4-encoding REGRESSION once
 * `tx` is named: from v2, `y before h` alone reads 34 (worse than 30),
 * `tx` named alone reads 10, and the two together read **4**.  One half in the
 * applied list, the other in the untried list, never crossed.
 *
 * ================= THE MECHANISM: RELOAD'S ROUND-ROBIN CURSOR ===============
 *
 * The r0/r2/r3 "rotation" the park saw is NOT REG_ALLOC_ORDER and NOT either
 * register allocator.  Every instruction in it is a RELOAD -- of a high-register
 * allocno (`mov sl,rX`, `mov rX,sl`, `mov rX,r8`) or of a spilled stack slot
 * (`movs rX,#2 / str rX,[sp,#8]`) -- so the register comes from
 * `allocate_reload_reg`, reload1.c:4962.  At reload1.c:5003:
 *
 *     i = last_spill_reg;
 *     for (count = 0; count < n_spills; count++)
 *       { i++; if (i >= n_spills) i -= n_spills; regnum = spill_regs[i]; ... }
 *
 * with the comment "We advance it round-robin between insns to use all spill
 * regs equally."  `last_spill_reg` is set on every successful allocation
 * (reload1.c:4937) and reset to -1 EXACTLY ONCE PER FUNCTION (reload1.c:823);
 * `spill_regs` is built in ASCENDING hard-register order in `finish_spills`
 * (reload1.c:3531).
 *
 * So a reload register is a function of HOW MANY RELOADS PRECEDE IT IN THE
 * FUNCTION, modulo n_spills.  It is a CURSOR, not a preference order.  The whole
 * "coherent rotation" is ONE PHASE STEP of that cursor, and adding or removing a
 * single earlier reload shifts every later reload register by one -- which is
 * reachable from ordinary C, and is why deleting `pp` (one edit, late in the
 * function) moved the registers at indices 16, 18, 48, 50, 65 and 67.
 *
 * Deleting `pp` is the lever with the mechanism attached: for `s->obj = f(...)`
 * gcc expands the CALL first and the LHS ADDRESS AFTER it, so the address pseudo
 * is born after the call and never crosses it -- it gets the ROM's
 * call-clobbered r2 instead of a callee-saved r5.  The park's `pp = &s->obj;`
 * placed the address BEFORE the call, forcing a callee-saved register and
 * pushing `slot` off r5 onto r6.
 *
 * `tx` works the same way and explains the 11-encoding run at 125-135: the ROM's
 * box copy is `mov r0, r8` -- into r0, the register STILL HOLDING the return
 * value of Func_801eadc.  That WAR dependence on r0 is what forces the box chain
 * below `str r0,[r2,#0]`, which is why the ROM stores first.  Ours put the copy
 * in r1, no WAR, so sched2 hoisted the whole box chain above the store.  Naming
 * `tx` re-phases the cursor, `bx` lands in r0, and the run collapses to zero AS
 * A CONSEQUENCE OF THE REGISTER -- no scheduling edit was involved.
 *
 * ================= WHAT THE LAST 2 ARE, AND WHY THEY DO NOT CLOSE ==========
 *
 *   index 143  ROM  ldr r3, =.L367dc      ours  movs r4, #194
 *   index 144  ROM  movs r4, #194         ours  ldr r3, =.L367dc
 *
 * `.23.sched2` from `-da -fsched-verbose=6`, basic block 16, verbatim:
 *
 *     ;;      insn  code    bb   dep  prio  cost   blockage units
 *     ;;      508   173     0     0     3     1    1 - 32   core : 509
 *     ;;      346   173     0     0     3     2    1 - 32   core : 348
 *     ;;      507   173     0     0     3     2    1 - 32   core : 348
 *     ;;      343   173     0     0     1     1    1 - 32   core :
 *     ;;  Ready list (t =  0):    343  507  346  508
 *     ;;      --> scheduling insn <<<508>>> on unit core
 *
 * 508 is `r4 = 194` (the `q` address: 0x610 = 194 << 3), 346 the `.L367dc` pool
 * load.  PRIORITY TIES AT 3 -- both chains are three cost-weighted insns to the
 * block end -- and `rank_for_schedule` (haifa-sched.c:4029) then falls through
 * every remaining rung: `INSN_REG_WEIGHT` is gated `!reload_completed` and so is
 * DEAD in sched2; the three interblock rungs are skipped because `INSN_BB` is
 * equal; the CLASS rung gives both class 3 (both independent of
 * `last_scheduled_insn`); DEPENDENT COUNT is 1 each.  The pick therefore lands on
 * the last rung, `return INSN_LUID (tmp) - INSN_LUID (tmp2);` -- pure RTL order,
 * lower LUID scheduled first.
 *
 * `.20.ce2` gives the pre-sched2 chain for bb 16:
 *     508 (r4=194) -> 509 (lsl) -> 340 (add fp) -> 343 (r5=sl)
 *       -> 346 (pool) -> 507 (sel reload) -> 348 (adds r6)
 *
 * REMAINING CAUSE, NAMED: matching needs LUID(346) < LUID(508) WHILE 348 stays
 * last.  No statement order reaches it, because the two insns are welded to
 * opposite ends of ONE statement -- 346 is emitted at the first reference to
 * `.L367dc`, and 507/348 are emitted together at the end of the same statement
 * (507 is reload's load of the spilled `sel`, inserted immediately before the
 * insn that needs it).  Moving the statement moves both ends.
 *
 * MEASURED AND SIZE-WRONG -- 408 bytes against 412, a DIFFERENT PROGRAM, do not
 * repeat: hoisting only the pool load.  `p = L367dc;` ... `p += sel;`,
 * `p = &p[sel];`, and both with `n = rows;` moved -- all four lose an
 * instruction to cse folding the split back.
 *
 * MEASURED AND INERT AT 2 (candidate prerequisites, not dead ends):
 *   declaration order `p` before `q` inside the block                     0
 *   `q = s->items` for `q = &s->items[0]`                                 0
 *   `p = L367dc + sel` for `p = &L367dc[sel]`                             0
 *   `q` as a declaration-initialiser rather than a statement              0
 *   `bx = (struct Box *)box;` hoisted above the store                     0
 *   `sel` declared last in the local list                                 0
 *   a named `w = 0x14` argument for CreateUIBox                           0
 *
 * MEASURED AND WORSE from v4:
 *   final loop as p, n, q                                                +2
 *   final loop as p, q, n                                                +3
 *   final loop as n, p, q                                                +3
 *   `p` before `q` in declaration-initialiser form                       +3
 *   `s = iwram_3001ea0;` moved below `f = _GetFlag(...)`         +33 and RELOC
 *
 * THE PARK'S OWN INERT LIST IS WRONG ON TWO ROWS, measured here: it records
 * "`p` before `q` in the final loop  0" (it is +3) and "`n = rows` assigned
 * before q/p  +3" (it is +2).
 *
 * KEPT FROM THE PREVIOUS HEADER, all four re-measured and still load-bearing:
 * the `int` carrier that keeps the sign extension a separate `ldrb / lsl / asr`
 * instead of `ldrsb`; the single `ty` local spanning the text row, the sprite y
 * and the final loop's -4; `sel = 0` AFTER the _GetFlag call; and `t = 3` before
 * `n = rows - 1`.  Its "one shared scratch variable" refutation also still holds
 * -- five distinct locals beat every merge.
 *
 * NEXT MOVE: nothing source-level is known.  The 2 is a sched2 LUID tie with the
 * two insns pinned to opposite ends of one statement.  If a lever is ever found
 * that gives a pool load an independent RTL position WITHOUT cse folding the
 * split back, this closes.
 */
struct SubScr {
    unsigned char pad0[0x5a4];
    void *obj;                          /* 0x5a4 */
    unsigned char pad1[0x610 - 0x5a8];
    void *items[1];                     /* 0x610 */
};
struct Box { unsigned char pad[0xc]; unsigned short x; unsigned short y; };

extern struct SubScr *iwram_3001ea0;
extern unsigned char gDebugMode;
extern unsigned char L367dc[] __asm__(".L367dc");
extern unsigned char Data_310a4[];
extern int _GetFlag(int flag);
extern unsigned char *CreateUIBox(int x, int y, int w, int h, int mode);
extern void Func_801e41c(void *box, int a, int b, int c, int d);
extern void DrawSmallText(int id, void *box, int x, int y);
extern int AllocSpriteSlot(void);
extern int UploadSpriteGFX(int slot, int n, void *src);
extern void *Func_801eadc(int slot, unsigned int m, void *box, int d, int e);
extern void _Func_80b0a20(void *p, int x, int y);
extern void *Func_8021750(int a, int b, void *box, int c, int d);

unsigned char *Func_801d9d4(void)
{
    struct SubScr *s;
    int rows;
    int sel;
    int f;
    int y, h, ty, slot, n, t, m, c, tx;
    struct Box *bx;
    unsigned char *box;

    s = iwram_3001ea0;
    rows = 3;
    f = _GetFlag(0x17e);
    sel = 0;
    if (f != 0) {
        rows = 1;
        sel = 2;
    }
    if (gDebugMode != 0)
        rows += 3;
    y = 8 - rows;
    h = rows * 3 + 1;
    if (y + h > 0x13) {
        y = 1;
        h = 0x13;
    }
    box = CreateUIBox(5, y, 0x14, h, 2);
    if (rows > 1) {
        t = 3;
        n = rows - 1;
        do {
            Func_801e41c(box, 0, t, 0x13, t);
            t += 3;
        } while (--n != 0);
    }
    ty = 4;
    if (f == 0) {
        m = 0xc23;
        DrawSmallText(m, box, 0x30, 4);
        m++;
        DrawSmallText(m, box, 0x30, 0x1c);
        ty = 0x34;
    }
    DrawSmallText(0xc25, box, 0x30, ty);
    ty += 0x18;
    if (gDebugMode != 0) {
        m = 0xc27;
        DrawSmallText(m, box, 0x30, ty);
        ty += 0x18;
        DrawSmallText(m + 1, box, 0x30, ty);
        ty += 0x18;
        m += 2;
        DrawSmallText(m, box, 0x30, ty);
    }
    slot = AllocSpriteSlot();
    if (slot <= 0x5f) {
        UploadSpriteGFX(slot, 0x80, Data_310a4);
        s->obj = Func_801eadc(slot, 0x40000000, box, 0, 0);
        bx = (struct Box *)box;
        tx = bx->x * 8;
        ty = bx->y * 8 + 0x10;
        _Func_80b0a20(&s->obj, tx, ty);
    }
    ty = -4;
    if (rows > 0) {
        void **q;
        unsigned char *p;
        q = &s->items[0];
        n = rows;
        p = &L367dc[sel];
        do {
            c = *p;
            p++;
            *q++ = Func_8021750((signed char)c, 0, box, 0xc, ty);
            ty += 0x18;
        } while (--n != 0);
    }
    return box;
}
