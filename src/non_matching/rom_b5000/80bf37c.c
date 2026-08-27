/* Func_80bf37c -- NOT MATCHING, and this park covers THREE functions.
 *
 * Source asm: goldensun/asm/rom_b5000/rom_bbb0c_a_c_c_a.s
 * Best screen: 31 instructions against the ROM's 32 -- one SHORT.
 *
 * Siblings at 30 instructions each, from tools/prologue_families.py:
 * Func_80bf37c, Func_80bf400, Func_80bf484. Three more at 46 instructions
 * (Func_80bf250, Func_80bf2b4, Func_80bf318) are the same routine with an
 * extra field, so six functions sit behind this shape.
 *
 * A status-effect tick: decrement a counter, and when it reaches zero try to
 * clear the effect.
 *
 * BLOCKER: a redundant register copy the ROM has and we do not.
 *
 *     rom    ldrb r2, [r5] / mov r3, r2 / cmp r3, #0
 *     ours   ldrb r3, [r5] / cmp r3, #0
 *
 * r2 is dead immediately after -- the later read of the same byte is a fresh
 * `ldrb r1, [r5]` -- so the copy does nothing. Splitting the value across two
 * named locals (`v = *p; n = v; n += 0xff;`) does not produce it; gcc coalesces
 * them.
 *
 * TWO THINGS WERE SOLVED and both generalise:
 *
 * `n += 0xff` AND `n--` ARE NOT THE SAME SPELLING HERE. The value is stored
 * with `strb` and then tested for zero, so +255 and -1 agree modulo 256 -- but
 * gcc emits the ROM's `add r3, #0xff` only for the first, and `n--` on an
 * `unsigned char` gives `lsl #24 / lsr #24` around the store instead. 24
 * differing to 11.
 *
 * THE THREE `return 0` PATHS SHARE ONE BLOCK, via `goto fail`. Written as three
 * separate `return 0` statements gcc hoists `mov r0, #0` to the top of the
 * function and emits the exit blocks in the opposite order -- 11 differing.
 * With the goto they collapse to the ROM's single tail. That is the
 * multiple-exits rule in docs/elevation.md.
 */
extern unsigned char *_GetUnit(void);
extern int Func_80bf208(int a, int b, int c);

int Func_80bf37c(int a)
{
    unsigned char *p;
    int n;

    p = _GetUnit() + (0x9c << 1);
    n = *p;
    if (n == 0)
        goto fail;
    n += 0xff;
    *p = n;
    if ((unsigned char)n == 0)
        return 1;
    if (Func_80bf208(a, *p, 0x1e) == 0)
        goto fail;
    *p = 0;
    return 1;
fail:
    return 0;
}
