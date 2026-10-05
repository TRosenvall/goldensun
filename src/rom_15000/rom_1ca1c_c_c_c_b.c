/* Func_801d9d4 (DrawSubScreen) -- 0x0801d9d4, asm/rom_15000/rom_1ca1c_c_c_c.s
 *
 * MATCHING.  objcmp: 412 bytes, 184 encodings and 18 relocations identical.
 * PIN-FREE, SHIM-FREE, NO asm, NO flags, NO symbol-table entry.
 * Park history: 41 (batch 324 open) -> 36 -> 30 -> 4 -> 2 -> 0 (batch 325).
 *
 * Verify with: docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work goldensun-build python3 tools/objcmp.py src/rom_15000/rom_1ca1c_c_c_c_b.c asm/rom_15000/rom_1ca1c_c_c_c_b.s --whole
 *
 * Verified before the split, against the unsplit .s, with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     scratch_elev/b325/A/p1_candidate.c \
 *     asm/rom_15000/rom_1ca1c_c_c_c.s --func Func_801d9d4
 *
 * SPLIT.  asm/rom_15000/rom_1ca1c_c_c_c.s holds Func_801d9d4, StartMenu_Main and
 * Func_801dd28 plus a .rodata tail, so this needs a TEXT/DATA SPLIT and ONE new
 * export.  tools/split_s.py, verbatim:
 *     REFUSING to split asm/rom_15000/rom_1ca1c_c_c_c.s: 1 local label(s) would
 *     cross files.
 *     asm/rom_15000/rom_1ca1c_c_c_c_b.s references .L367dc, defined in
 *     asm/rom_15000/rom_1ca1c_c_c_c_c.s
 * so `.global .L367dc` goes in FIRST (a .global emits no bytes; gate make+compare
 * on it before the split).  tools/datacheck.py confirms .L367c9/.L367cc/.L367ce/
 * .L367d0/.L367d6/.L36750 are ALREADY global and .L367dc is the only addition.
 * Target lands in rom_1ca1c_c_c_c_b.s -> src/rom_15000/rom_1ca1c_c_c_c_b.c.
 *
 * The only non-obvious declaration is
 * `extern unsigned char L367dc[] __asm__(".L367dc");` -- the tree's established
 * spelling for a `.L` local label (src/rom_c9000/rom_dd2ac_c_c_b.c:123).
 *
 * ==================== WHAT CLOSED THE LAST 2 ===============================
 *
 * The park's residue was ONE adjacent transposition at ref indices 143-144:
 *     index 143  ROM  ldr r3, =.L367dc      ours  movs r4, #194
 *     index 144  ROM  movs r4, #194         ours  ldr r3, =.L367dc
 * a sched2 pick at t = 0 in basic block 16 where `rank_for_schedule`
 * (haifa-sched.c:4029) runs out of rungs: priority ties at 3 for all of 508,
 * 346 and 507; INSN_REG_WEIGHT is gated `!reload_completed` and dead in sched2;
 * the interblock rungs are skipped on equal INSN_BB; the CLASS rung is skipped
 * at t = 0 because `last_scheduled_insn` is 0; dependent count is 1 each.  So
 * line 4115's `INSN_LUID` decides, lower first.
 *
 * The park STATED the requirement correctly -- "matching needs LUID(346) <
 * LUID(508) WHILE 348 stays last" -- and then drew the wrong conclusion from it
 * ("No statement order reaches it ... the two insns are welded to opposite ends
 * of ONE statement").  Both halves of that sentence are true.  What it misses is
 * that the fix is not a REORDER of the statement, it is a SPLIT of it:
 *
 *   346 = `ldr r3,=.L367dc`, emitted at the first reference to the table
 *   508 = `movs r4,#194`, a RELOAD, so reload1.c plants it immediately before
 *         509 (`lsl r4,r4,#3`) -- its LUID is pinned INSIDE the `q` statement
 *   348 = `add r6,r0,r3`, the `+ sel`
 * and the ROM's block order is  346, 508, 507, 509, 340, 343, 348.  The pool
 * load has to sit ABOVE the whole `q = &s->items[0]` statement and the add
 * BELOW it.  One statement cannot straddle another; two can.
 *
 * So `tbl = L367dc;` goes above `q`, and `p = tbl + sel;` stays below `n`.
 *
 * ==================== WHY THE PARK'S HOIST LOST AN INSTRUCTION =============
 *
 * The park recorded this as MEASURED AND SIZE-WRONG, 408 bytes against 412, "a
 * DIFFERENT PROGRAM, do not repeat", over four spellings: `p = L367dc;` then
 * `p += sel;`, `p = &p[sel];`, and both with `n = rows;` moved.  Every one of
 * those REUSES `p` as the carrier, so `p` becomes multi-set and the split is
 * folded back.
 *
 * > A SPLIT IS ONLY FOLDED BACK IF ITS TWO HALVES SHARE A PSEUDO.  Given its own
 * > single-assignment local the hoist survives: the pool load is
 * > `(set tbl (mem (symbol_ref *.LCn)))` and folding would have to produce
 * > `(set p (plus (mem ...) reg))`, which no thumb pattern recognises, so
 * > combine's recog fails and the two insns stay.
 *
 * Generalises: a park that rejects a hoist on a SIZE change should be retried
 * with a FRESH CARRIER before the hoist is written off.
 *
 * Four spellings land, all 412 bytes / 184 encodings / 18 relocations exact:
 *     tbl = L367dc;              + p = tbl + sel;        0
 *     tbl = L367dc;              + p = &tbl[sel];        0
 *     unsigned char *tbl = L367dc;  + p = tbl + sel;     0
 *     tbl = &L367dc[0];          + p = tbl + sel;        0
 * and the order of the three statements after the hoist still matters exactly as
 * the park's table said:
 *     tbl, n, q, p   -> 2 differing encodings of 184 (first at index 147)
 *     tbl, q, p, n   -> 2 differing encodings of 184 (first at index 148)
 *
 * Everything the park listed as load-bearing is kept and still is: the `int`
 * carrier that keeps the sign extension `ldrb / lsl / asr` instead of `ldrsb`;
 * the single `ty` spanning the text row, the sprite y and the final loop's -4;
 * `sel = 0` AFTER the _GetFlag call; `t = 3` before `n = rows - 1`; the direct
 * `s->obj = Func_801eadc(...)` store with no `pp` local; `rows = 1` before
 * `sel = 2`; `y` before `h`; the named `tx`; and the final loop's q, n, p order.
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
        unsigned char *tbl;
        tbl = L367dc;
        q = &s->items[0];
        n = rows;
        p = tbl + sel;
        do {
            c = *p;
            p++;
            *q++ = Func_8021750((signed char)c, 0, box, 0xc, ty);
            ty += 0x18;
        } while (--n != 0);
    }
    return box;
}
