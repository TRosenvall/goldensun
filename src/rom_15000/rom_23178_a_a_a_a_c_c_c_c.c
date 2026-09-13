/* Cluster Func_8028808..Func_8028920 extracted from
 * goldensun/asm/rom_15000/rom_23178_a_a_a_a_c_c_c_c.s.
 *
 * LANDING: WHOLE, no split, no linker edit.  All three functions of the .s
 * convert, so the single stage1.ld line stays VERBATIM on the asm/ path:
 *
 *       asm/rom_15000/rom_23178_a_a_a_a_c_c_c_c.o(.text)
 *
 * .L37403 and .L373f7 are already `.global` in
 * asm/rom_15000/rom_23178_c_c_c.s -- grepped, no export to add.
 *
 * Struct stride 0x14 and the 0x8e count offset are confirmed against the
 * siblings src/rom_15000/rom_23178_a_a_a_a_c_c_c_b.c (AddMenuBarOption) and
 * src/rom_15000/rom_23178_a_a_a_a_c_c_a_b.c (Func_802851c).
 */
struct Ui {
    unsigned char pad0[0x78];
    void *box;
    unsigned char pad7c[0x12];
    short count;
    short w;
    short h;
    short y;
};

extern unsigned char *iwram_3001f38;

extern signed char L37403[] __asm__(".L37403");
extern signed char L373f7[] __asm__(".L373f7");

extern void *CreateUIBox(int a, int b, int c, int d, int e);
extern int _GetNumDjinn(int n);
extern void Func_80284dc(void);
extern void AddMenuBarOption(int n);
extern int Func_8028574(int n);
extern void Func_802851c(void);

/* OpenSubScreenWindow
 * r0.. = parameters. Opens the sub-screen's window with CreateUIBox, sizing it
 * with Func_af0.
 *
 * The row x is centred: 15 - (3 * count + 2 * width / 3) / 2, stepping 3 per
 * row, and the value left in x after the loop is what CreateUIBox is handed.
 */
void Func_8028808(int y, int w, int h)
{
    struct Ui *u;
    int i;
    int n;
    int x;

    u = (struct Ui *)iwram_3001f38;
    u->w = w + 2;
    u->h = h;
    u->y = y;
    n = u->count;
    x = 0xf - (n * 3 + u->w * 2 / 3) / 2;
    for (i = 0; i < u->count; i++) {
        *(short *)((unsigned char *)u + i * 0x14 + 0xc) = x * 8;
        *(short *)((unsigned char *)u + i * 0x14 + 0xe) = y * 8;
        x += 3;
    }
    u->box = CreateUIBox(x, y, u->w, 3, 2);
}

/* OpenSubScreenWindowAlt
 * r0.. = parameters. As Func_28808 with fixed geometry: the caller passes the
 * starting x itself instead of it being centred.
 *
 * The two field stores inside the loop come out in the OPPOSITE order to
 * Func_8028808's -- +0xe before +0xc -- but that is a scheduling artefact, not
 * evidence about the source: MEASURED, both source orders are byte-identical
 * here, so nothing hangs on which way round they are written.
 */
void Func_80288a8(int x, int y, int w, int h)
{
    struct Ui *u;
    int i;

    u = (struct Ui *)iwram_3001f38;
    u->w = w + 2;
    u->h = h;
    u->y = y;
    for (i = 0; i < u->count; i++) {
        *(short *)((unsigned char *)u + i * 0x14 + 0xe) = y * 8;
        *(short *)((unsigned char *)u + i * 0x14 + 0xc) = x * 8;
        x += 3;
    }
    u->box = CreateUIBox(x, y, u->w, 3, 2);
}

/* RunFieldMenu
 * r0 = the previously chosen entry. Draws the field menu and returns which
 * entry the player picked, or a negative value when they backed out.
 *
 * _GetNumDjinn(-1) decides how many entries there are: when the party summary
 * comes back empty the extra panel 0x0F is left out, so the menu is three rows
 * instead of four. The two byte tables .L37403 and .L373f7 translate between
 * the row on screen and the caller's entry number in each of those two shapes,
 * indexed by `previous + 6 * short`.
 *
 * The panels are appended in order -- 1, then 0x0F when the party is present,
 * then 2 and 7 -- with Func_28808(0x11, 7, 0) placing the box, Func_28574
 * running the cursor and Func_2851c tearing it down.
 *
 * Func_1c244 is the caller, and it dispatches the result 0..4 into
 * _Func_8ce74, _Func_a5b94, _Func_aa56c, _Func_a24d0 and _Func_a7478.
 *
 * THE FIRST TABLE READ NEEDS BOTH A NAMED BASE AND A NAMED INDEX.
 * The ROM issues `ldr r2, =.L37403` in the MIDDLE of the `k = m * 6` build,
 * between `add r3, r6` and `lsl r7, r3, #1`.  sched2 has the pool load ready
 * from the top of the block; the two shifts beat it on priority and then TIE
 * with it, and a tie goes to the smaller INSN_LUID -- so the load has to be
 * EMITTED earlier, not made cheaper.  Naming the base (`t = L37403;`) does
 * that, and it must sit AFTER the _GetNumDjinn call: named in the entry block
 * it stays live across the call and costs a callee-saved register.  Naming the
 * base alone still leaves `ldrsb r3, [r3, r2]` where the ROM has `[r2, r3]`,
 * because the pointer form expands the address into its own pseudo and combine
 * swaps the two registers when it folds that pseudo into the MEM.  Naming the
 * INDEX too (`j = idx + k;`) keeps the address a plain reg+reg that the
 * expander builds straight into the MEM, so combine never rewrites it.
 * Base-only is 1 of 51, index-only 2 of 51, both together exact.
 */
int Func_8028920(int idx)
{
    signed char *t;
    int m;
    int k;
    int j;
    int v;

    m = 0;
    if (_GetNumDjinn(-1) == 0)
        m = 1;
    t = L37403;
    k = m * 6;
    j = idx + k;
    v = t[j] - 1;
    if (v < 0)
        v = 0;
    Func_80284dc();
    AddMenuBarOption(1);
    if (m == 0)
        AddMenuBarOption(0xf);
    AddMenuBarOption(2);
    AddMenuBarOption(7);
    Func_8028808(0x11, 7, 0);
    v = Func_8028574(v);
    Func_802851c();
    if (v >= 0)
        v = L373f7[v + k + 1];
    return v;
}
