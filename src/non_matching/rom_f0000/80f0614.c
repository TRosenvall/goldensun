/* Func_80f0614 -- 0x080f0614 -- asm/rom_f0000/rom_f0254_c.s
 *
 * SCREENED ONCE, 40 of 44, one line short. Filed at the depth actually
 * reached; the structure is understood and the divergence is identified, but
 * only one spelling was tried.
 *
 * Returns early if a status halfword is non-zero; divides two signed halfwords
 * by 8 and returns early if they agree; otherwise copies one to the other,
 * indexes a table, and stores the result of Func_80f07f0.
 *
 * ONE THING READ CORRECTLY: ewram_2004c00 is loaded BOTH signed and unsigned
 * from the same address -- `ldrsh r2, [r3, r6]` for the division and
 * `ldrh r4, [r3]` for the copy -- which is the documented `short` rule: the
 * sign matters to the divide and cannot reach a halfword store, so one `short`
 * declaration gives both. That part reproduces.
 *
 * THE DIVERGENCE is inside gcc's signed-divide-by-8 expansion:
 *
 *     rom    mov r3, r2 / cmp r2, #0 / bge / add r3, r2, #7 / asr r1, r3, #3
 *     ours   cmp r2, #0 / bge / add r2, #7 / asr r0, r2, #3
 *
 * The ROM copies the loaded value into a second register before biasing it and
 * shifts the COPY; ours biases the loaded register in place. Both are correct
 * expansions of `v / 8` and the ROM's costs one more instruction, which is the
 * one-line shortfall -- and the extra live value is why it pushes r6 where we
 * push only r5.
 *
 * That is a copy of a value that is DEAD afterwards, so by the boundary in
 * docs/elevation.md it is not reachable by naming a second local -- gcc would
 * coalesce it. It is also not the source's choice: the copy is emitted by the
 * division expansion itself, below the level any C spelling addresses.
 *
 * NOT TRIED, and the honest next step: whether writing the division as an
 * explicit `(v < 0 ? v + 7 : v) >> 3` changes the expansion. That is a
 * different rtx from `v / 8` and might be expanded differently -- worth one
 * screen, but it was not run here.
 */
extern short ewram_2004c00;
extern short ewram_2004c04;
extern short ewram_2004c08;
extern int Lf1220[] __asm__(".Lf1220");
extern short Func_80f07f0(int a, int b, int c);

void Func_80f0614(void)
{
    int a;
    int b;
    int t;

    if (ewram_2004c04 != 0)
        return;
    a = ewram_2004c00 / 8;
    b = ewram_2004c08 / 8;
    if (a == b)
        return;
    ewram_2004c08 = ewram_2004c00;
    t = (a + 0x10) & 0x1f;
    ewram_2004c04 = Func_80f07f0(Lf1220[a], t * 24, 1);
}
