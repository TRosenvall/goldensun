/* Func_808c2dc -- 0x0808c2dc, split out of asm/rom_8a000/rom_8ba38_a_a_a_c_a_c_a.s.
 *
 * Applies one PP delta to every party member.
 *
 * EXACT ON THE FIRST CANDIDATE, and the whole result is the file-mate habit
 * paying again. Func_808c30c, already elevated in ..._c_a_c_b.c beside this,
 * walks the SAME table over the same party-size call; copying its idiom
 * verbatim -- `gState[(0xfc << 1) + i]` under an ascending `for` -- matched
 * without a single iteration. `(0xfc << 1)` is also what produces the ROM's
 * `mov r2, #0xfc / lsl r2, #1` rather than a pool word.
 *
 * THE NEGATIVE IS THE INTERESTING HALF, AND IT IS A TRAP WORTH RECORDING. The
 * ROM's loop reads `ldrb r0, [r6] / add r6, #1` against a counter that goes
 * DOWN -- a textbook walking pointer, and the recorded walking-index/walking-
 * pointer lever says a pointer is what emits that form. Transcribing it that
 * way is WORSE: the honest do-while over `unsigned char *p` with `--n` measures
 * 18 differing of 21 and is four bytes short.
 *
 * The pointer in the ROM is strength_reduce's, not the source's. The source
 * indexes; loop optimisation turns the index into the pointer and the trip
 * count into a countdown, and writing the OUTPUT of that pass back into C
 * hands the optimiser something it cannot re-derive. Same shape as the batch-264
 * reversed-loop result: an induction variable's final form in the ROM is
 * evidence about the PASS, not about the statement.
 *
 * EXACT: 48 bytes, 21 encodings, 3 relocations, measured three times.
 */
extern unsigned char gState[];
extern int _GetPartySize(void);
extern void _ModifyPP(int id, int b);

void Func_808c2dc(int a)
{
    int n;
    int i;

    n = _GetPartySize();
    for (i = 0; i < n; i++)
        _ModifyPP(gState[(0xfc << 1) + i], a);
}
