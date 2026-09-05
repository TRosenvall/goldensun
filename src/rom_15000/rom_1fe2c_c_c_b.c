/* Func_80200cc -- SubmitSecondCursor.  asm/rom_15000/rom_1fe2c_c_c.s, 64
 * encodings / 132 bytes.  EXACT, bare, no pins, no flags, no barriers.
 *
 *   OK Func_80200cc -- 132 bytes, 64 encodings and 2 relocations identical
 *
 * Class: "the zero interleaved into a shifted build" at one STRAIGHT-LINE site
 * -- the argument block
 *
 *     mov r3, #0x80 / mov r2, r4 / mov r1, r5 / lsl r3, #7 / str r4, [sp]
 *
 * with the two register moves sitting inside the mov+lsl build of 0x4000.
 * Per the re-derivation ("the wall hid 230 functions"), the general cure is the
 * UNIFORM WHOLE-VALUE ASCENDING FILL, not the hand-placed zero: one statement
 * per argument, in order, every shifted constant written WHOLE.  `0x80 << 7`
 * passed bare in the call is exactly that, and the interleave falls out on the
 * first screen.  Nothing else was needed.  rom_15000 is not an overlay and no
 * overlay idiom was transplanted; the shape was re-derived from a SOLVED TWIN
 * in this tree (below).
 *
 * SOLVED TWIN -- found before writing anything, per "Twins are worth checking
 * for before writing anything".  src/rom_a1000/rom_a1814_a_b.c (Func_80a19a0)
 * is the same loop over iwram_3001f2c: actor pointer, null test, a 2-int scale
 * vector and a 4-int pos vector on the stack, `_UpdateSprite(p, pos, scale,
 * 0x80 << 7)`, and the SAME r4 spill/reload around the call that -fcall-used-r4
 * forces.  It supplied the declaration order (scale before pos -> sp+4 / sp+0xc)
 * and the bare `0x80 << 7`.  This function differs in three ways, all of which
 * the source expresses directly:
 *
 *   - the second-cursor block sits at 0x224/0x234/0x23c, not 0x114/0x134/0x144,
 *     and has FOUR slots, not eight.  `add r6, r3, r2 / sub r2, #0x10 /
 *     add r7, r3, r2` is gcc's OWN arithmetic off 0x8d << 2 = 0x234; do not try
 *     to spell 0x224 separately.
 *   - the scale is the constant 0x10000, not a loaded field, and the pos[1]
 *     base is the constant 0x1F40000 rather than a subtraction.  Both are used
 *     twice per iteration, so gcc hoists them out of the loop into r9/r10 --
 *     which is why the prologue copies THREE high registers (r5<-r8, r6<-r9,
 *     r7<-r10) where the twin copies two.  Read the prologue by CONTENT: count
 *     the `mov rN, r5..r11` copies by destination, and the third copy is the
 *     carried 0x10000.
 *   - one read of actors[i] per iteration instead of the twin's two reads of
 *     the same base, so gcc folds the bump into the load: `ldmia r7!, {r0}`
 *     rather than a trailing `add r5, #4`.  That is a consequence of reading
 *     the array ONCE, not something to reach for.
 *
 * CONSTANT SPELLING.  Write the values whole and let gcc build them.  Its thumb
 * expansion takes the SMALLEST i with (val & (0xff << i)) == val, so 0x10000
 * comes out `mov #0x80 / lsl #9` -- NOT the `mov #1 / lsl #16` that "shift off
 * the trailing zeros" would predict.  Likewise 0x1F40000 -> `mov #0xfa /
 * lsl #17` and 0x4000 -> `mov #0x80 / lsl #7`.  All three match the ROM, so no
 * constant here needs a source-level split.
 *
 * PINS: none ship.  Sweeping front to back:
 *
 *   0x10000 and 0x1F40000 named at the top   140 bytes, 68 enc, 50 differing
 *   0x80 << 7 named as `int m`               EXACT -- measures ZERO
 *   `int *sc = scale; int *ps = pos;`        124 bytes, 60 enc, 61 differing
 *
 * The first is "naming a value gcc already CARRIES destroys the carry" at its
 * cleanest: pinned, the two constants stop being loop-invariants gcc hoists and
 * are rebuilt in the loop, costing 8 bytes.  The mode pin is INERT SCAFFOLDING
 * and is therefore omitted.  The named-pointer lever is actively wrong here --
 * the ROM computes sp+4 / sp+0xc once and keeps them, which the bare arrays
 * already give.
 *
 * SIBLING: Func_801ff58 (asm/rom_15000/rom_1fe2c_c_a.s, SubmitCursorSprite) is
 * the SAME shape one struct over -- 0x114/0x134/0x154, scale loaded from
 * b->scales[i], hence only two carried high registers.  It is the twin
 * Func_80a19a0 with a constant bound; it is being screened separately.
 *
 * LANDING: rom_1fe2c_c_c.s holds TWO functions plus a trailing `.section
 * .rodata` (.global .L73854 / .incrom 0x73854, 0x73864), so a whole-file .c is
 * impossible -- the split is required by the DATA, not by the neighbour.
 * `tools/split_s.py asm/rom_15000/rom_1fe2c_c_c.s Func_80200cc` cuts it in two
 * (the preamble holds only .include, so no _a part is written): the target to
 * _b, Func_8020150 + the rodata to _c.  Trailing data only, no `.L` crossing
 * the boundary.  Flag group: the generic `asm/%.o: src/%.c` rule, plain
 * GCC296_CFLAGS at -O2; tryc.makefile_flags returns the empty set for both
 * rom_1fe2c_c_c_b.c and rom_1fe2c_c_c_c.c, so no pattern rule bites.
 */

struct Sprite;

struct Blk {
    unsigned char pad000[0x224];
    struct Sprite *actors[4];
    short xs[4];
    short ys[4];
};

extern struct Blk *iwram_3001f2c;
extern void _UpdateSprite(struct Sprite *s, int *pos, int *scale, int mode);

void Func_80200cc(void)
{
    struct Blk *b;
    struct Sprite *p;
    int scale[2];
    int pos[4];
    int i;

    b = iwram_3001f2c;
    for (i = 0; i < 4; i++) {
        p = b->actors[i];
        if (p != 0) {
            scale[0] = 0x80 << 9;
            scale[1] = 0x80 << 9;
            pos[0] = b->xs[i] << 16;
            pos[1] = 0xfa << 17;
            pos[2] = (b->ys[i] << 16) + (0xfa << 17);
            pos[3] = 0;
            _UpdateSprite(p, pos, scale, 0x80 << 7);
        }
    }
}
