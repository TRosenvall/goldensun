/* OvlFunc_957_2008cf8  --  0x02008cf8
 *
 * Cut out of goldensun/asm/overlays/rom_7e3e08/ovl_30_c_c_a_c_c_c_c_c_c_a_c.s;
 * the rest of that file stays as assembly beside it.
 *
 * A tile-position guard: if actor 0xc is standing exactly on tile (0x1e, 0x14),
 * mark it, repaint that one tile, and record the visit with save bit 0x212.
 * Both coordinates are 12.20 fixed point, hence the `asr #20`.
 *
 * THE THREE 0x14s ARE NOT A SOURCE SIGNAL -- MEASURED, and recorded because
 * the opposite is the tempting reading. The ROM builds arguments 1 and 2 with
 * `mov r0, #0x1e` / `mov r1, #0x14` but fills the last stack slot with
 * `str r4, [sp, #4]`, r4 still holding the shifted y from the test that just
 * succeeded. That looks like it says which of the three is a live variable and
 * which are rematerialised constants, so the obvious move is to pass `y` for
 * the stack slot and literals for the register arguments.
 *
 * It says nothing of the kind. All three spellings -- `y` in the stack slot
 * only, `y` in both slot and r1, and 0x14 everywhere -- compile to the SAME
 * thirty-five instructions. gcc knows y == 0x14 inside the branch and chooses
 * per operand whether to re-derive it or reuse the register, and the choice is
 * not reachable from the C. The form below is written because it reads as what
 * the original meant, not because it was forced.
 *
 * The general lesson: a value that is provably constant inside its branch is
 * NOT evidence about the source, however suggestive the register reuse looks.
 * Contrast the stack-arg lever of batch 84, where the shared value could not be
 * constant-folded and naming it therefore did decide the output.
 *
 * Matched on the first screen.
 */
struct A {
    unsigned char pad00[8];
    int f8;
    unsigned char pad0c[4];
    int f10;
    int f14;
    unsigned char pad18[0x23 - 0x18];
    unsigned char f23;
    unsigned char pad24[0x55 - 0x24];
    unsigned char f55;
};

extern struct A *__MapActor_GetActor(int slot);
extern void __SetFlag(int id);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

void OvlFunc_957_2008cf8(void)
{
    struct A *a;
    int y;

    a = __MapActor_GetActor(0xc);
    if ((a->f8 >> 20) == 0x1e) {
        y = a->f10 >> 20;
        if (y == 0x14) {
            a->f55 = 2;
            a->f14 = 0;
            a->f23 = 2;
            __Func_8010704(0x1e, 0x14, 1, 1, 0x20, y);
            __SetFlag(0x212);
        }
    }
}
