/* OvlFunc_924_200a030  --  0x0200a030
 * [asm/overlays/rom_7ac2d8/ovl_1db4_a.s, SECOND of two functions;
 *  the file also carries a trailing `.section .data` that belongs to
 *  ovl_1db4_b.c's function, so this is a THREE-WAY split, not a two-way one.]
 *
 * 183 instructions -- 412 bytes, 188 encodings and 14 relocations identical,
 * under the DEFAULT flag set.  NO flag group is involved: -fno-gcse,
 * -fno-schedule-insns2, -ffixed-r7, -fno-strength-reduce,
 * -fno-rerun-cse-after-loop and -fno-strict-aliasing were all swept against
 * the sibling in this same .s and none of them is needed here.
 *
 * Three passes of a seven-tick sparkle over a map rectangle: paint the tiles,
 * play 0x121, then for each of three passes step an odd counter 1..7 spawning
 * one particle per odd tick from a mode-dependent line, and repaint one row at
 * the end of each pass.  `mode` picks between three different lines.
 *
 * THE INNER LOOP IS A `goto` LOOP AND THE OUTER ONE IS NOT, and that split is
 * the whole function.  Written with both as ordinary loops the screen is 199
 * lines against 193 with 152 differing: gcc STRENGTH-REDUCES `((k*4+i) << 17)`
 * into a giv (`add r5, r5, #0x20000` per tick), spills the commoned `k*4` to a
 * fourth stack word and the frame goes `sub sp, #0x3c` against the ROM's
 * `sub sp, #0x38`.  The ROM rebuilds `lsl r2, r7, #2` inside every arm, which
 * is docs/elevation.md's recorded signature (§"`goto` loops disable loop
 * optimisation ENTIRELY") -- and this is the TRANSFORMATION case the §"what
 * actually separates it from its counter-example" table says pays: strength
 * reduction, not one hoisted value.  Rewriting only the INNER loop with `goto`
 * took it to 188 lines and 21 instructions in disagreeing regions.
 *
 * LEAVING THE OUTER LOOP A REAL LOOP IS LOAD-BEARING, and that is the part
 * worth keeping.  The lever's own corollary -- "with hoisting off, anything set
 * up in a register before the loop had to be written there" -- would have meant
 * naming all four of &p, 0x90000, 0 and 1 by hand.  Because the OUTER loop is
 * still a `do/while`, its LICM still hoists what it should: `1` and `0x90000`
 * arrived in r9/r11 unaided.  Only what LICM will not lift had to be named.
 * Applying the lever to the innermost loop that carries the transformation, and
 * no further out, is the cheap form of it.
 *
 * `int zero = 0;` IS A REAL DECLARATION AND IT IS WORTH FIVE INSTRUCTIONS.
 * gcc will not hoist a bare `0` -- rematerialising `mov r3, #0` is cheaper than
 * a callee-saved register -- so without it we came out 188 against 193 and used
 * only r8/r9/r10 where the ROM pushes r8..r11.  Naming it, and using it ONLY at
 * the two sites where the ROM stores a register (`mov r3, r10` in the mode 1
 * and mode >= 2 arms, against `mov r3, #0x0` in mode 0, where CSE already has
 * the parameter's proven zero in r6), is exactly the +5: `mov r2, #0x0`,
 * `mov r10, r2`, and the three-instruction cost of the fourth push.  That is
 * §"gcc will ADD a callee-saved register to share a constant" read backwards.
 *
 * `int d = 0x90 << 12;` IS ALSO A DECLARATION, and the ONLY thing it buys is
 * the ORDER of the two `mov rHIGH` copies.  With `zero` named and `0x90 << 12`
 * left inline the screen is 193 against 193 with 8 differing, all of them in
 * one setup block: ours builds the constant in r2 and the zero in r3 where the
 * ROM has them the other way round, and emits `mov r10` before `mov r11`.
 * Naming `d` FIRST puts the two values in source order and the block collapses
 * to 3 differing.  Naming `zero` first instead is 8 -- the same count as not
 * naming `d` at all, so this is an ORDER lever, not a naming one.
 *
 * THE COUNTER INITIALISER IS THE LAST THREE INSTRUCTIONS, and it wants to come
 * BEFORE the struct stores.  With `k = 0` left in the `for` header the residue
 * is `mov r7, #0x0` sitting three slots too late.  The recorded rule
 * (§"Counter initialisation wants to come first", and the three later
 * confirmations) says hoist it; here "first" means above `p.f4 = 5;`, not
 * merely above the two named constants -- `d; zero; k = 0;` is 3 differing and
 * `d; k = 0; zero;` is 2.  Exact is `k = 0;` then the three field stores, then
 * `d`, then `zero`.
 *
 * The frame is arithmetic, as always: sp = 0x38 is 0x10 of outgoing stack
 * arguments (the spawner takes eight) plus a 0x28-byte `struct P` at sp+0x10.
 * `struct P` is the sibling ovl_35b8_a_a_c_a_c_c_a_b.c's struct unchanged, and
 * f0 is again never written.
 *
 * The mode >= 2 arm's x is the ROM's own grouping, not the other two arms'.
 * Modes 0 and 1 compute `((k * 4 + i) << 17) + C` -- one `lsl #2`, one `add`,
 * one `lsl #17`.  Mode 2 computes `lsl r0, r5, #0x11 / neg r0 / lsl r3, r7,
 * #0x13 / sub r0, r3 / add r0, #const`, which is `C - (i << 17) - (k << 19)`
 * with the two shifts kept apart.  Spelling all three arms the same way costs
 * the `neg` and moves the pool.  This is the sibling's "adjacency does not carry
 * a spelling over" rule inside ONE function: three arms of one `if` chain, and
 * only the ROM's instruction order says which grouping each one had.
 *
 * r4 is absent from the push set because `-fcall-used-r4` is in GCC296_CFLAGS.
 */
struct P {
    int f0;
    int f4;
    int f8;
    int fc;
    unsigned char pad10[0x28 - 0x10];
};

extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern void __PlaySound(int id);
extern unsigned int __Random(void);
extern void __CutsceneWait(int n);
extern void OvlFunc_common0_10c(int x, int y, int z, int a,
                                int b, int c, int d, struct P *p);

void OvlFunc_924_200a030(int mode)
{
    struct P p;
    int d, zero;
    unsigned int k, i;

    __CopyMapTiles(0x70, 0x39, 0x71, 0x2a, 1, 1);
    __CopyMapTiles(0x75, 0x3a, 0x70, 0x2e, 1, 1);
    __CopyMapTiles(0x75, 0x39, 0x74, 0x2c, 1, 1);
    __PlaySound(0x121);
    k = 0;
    p.f4 = 5;
    p.f8 = 0x80 << 8;
    p.fc = 0x80 << 8;
    d = 0x90 << 12;
    zero = 0;
    do {
        i = 1;
    again:
        if (i & 1) {
            if (mode == 0)
                OvlFunc_common0_10c((0x319 - (__Random() * 5 >> 16)) << 16, 0,
                                    ((k * 4 + i) << 17) + 0x2b70000,
                                    0, 0, 0x80 << 7, d, &p);
            else if (mode == 1)
                OvlFunc_common0_10c(((k * 4 + i) << 17) + 0x3120000, 0,
                                    ((__Random() * 5 >> 16) << 16) + 0xba * 0x40000,
                                    0x80 << 7, zero, zero, d, &p);
            else
                OvlFunc_common0_10c(0xce * 0x40000 - (i << 17) - (k << 19), 0,
                                    ((__Random() * 5 >> 16) << 16) + 0xb2 * 0x40000,
                                    0x80 << 7, zero, zero, d, &p);
            __CutsceneWait(1);
        }
        i++;
        if (i <= 7)
            goto again;
        if (mode == 0)
            __CopyMapTiles(0x70, 0x3a, 0x71, k + 0x2b, 1, 1);
        else if (mode == 1)
            __CopyMapTiles(0x70, 0x3a, k + 0x71, 0x2e, 1, 1);
        else
            __CopyMapTiles(0x70, 0x3a, 0x73 - k, 0x2c, 1, 1);
        k++;
    } while (k <= 2);
}
