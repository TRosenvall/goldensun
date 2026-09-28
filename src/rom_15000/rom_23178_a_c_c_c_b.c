/* Func_802938c -- 0x0802938c, last of the three functions that were in
 * asm/rom_15000/rom_23178_a_c_c_c.s.  Func_8029274 and Func_80292c4 stay in
 * assembly as the `_a` part, together with their `.L3742c` / `.L37428` references.
 * No data section, so this is a plain text split and nothing crosses the boundary
 * in either direction -- this function reads only gKeyRepeat, gKeyPress, _GetFlag,
 * _SetFlag and _ClearFlag, all of which are already global.
 *
 * 324 bytes, 157 encodings and 5 relocations identical.  Exact on the FIRST
 * candidate, with no iteration.
 *
 * `int *r = q + 1;` MUST BE A NAMED LOCAL, not `q[1]` written into each arm.  The
 * ROM's `add r4,r5,#4` sits in the ENTRY block, above the first branch, and expand
 * emits linearly -- so a subscript inside the arms puts the address computation
 * inside an arm instead.
 *
 * THE TWO WRAP SPELLINGS ARE READ OFF THE ROM AND ARE NOT INTERCHANGEABLE.
 * `if (x < 0) x = 0xf;` gives `mov r3,#0xf`, while `if (x > 0xf) x = 0;` REUSES the
 * zero that the failed `and` already left in r0 -- which is why several arms store
 * r0 rather than materialising a constant.
 *
 * No pins, no barriers, no .equ, no per-file flags, no fakematch row.  `volatile` on
 * gKeyRepeat and gKeyPress is a real qualifier: the interrupt handler writes the key
 * state and this menu re-reads it every frame.
 */
/* Func_802938c -- 0x0802938c, third of the three functions in
 * asm/rom_15000/rom_23178_a_c_c_c.s (Func_8029274, Func_80292c4, Func_802938c).
 *
 * Shape: a gKeyRepeat dispatch chain over two int cursors.  Args (a, p, q) with
 * a unused; q is an int pair, and `r = q + 1` is computed in the ENTRY block,
 * above the first branch, so it is a named local rather than a `q[1]` subscript
 * inside each arm (expand emits linearly, so a subscript would land in the arm).
 *
 * Wrap spellings read off the ROM:
 *   `if (x < 0) x = 0xf;`  -> mov r3,#0xf / str
 *   `if (x > 0xf) x = 0;`  -> str r0 reusing the zero the failed `and` left in r0
 * The second is why several arms store r0 instead of a fresh zero.
 *
 * Returns: the arms that move *p return 1, the arms that move q[0]/q[1] return 0.
 */
extern volatile unsigned int gKeyRepeat;
extern volatile unsigned int gKeyPress;

extern int _GetFlag(int id);
extern void _SetFlag(int id);
extern void _ClearFlag(int id);

int Func_802938c(int a, int *p, int *q)
{
    int *r = q + 1;
    int n;

    if (gKeyRepeat & 1) {
        n = ((((*p << 4) + *r) << 4) + *q);
        if (_GetFlag(n))
            _ClearFlag(n);
        else
            _SetFlag(n);
        return 1;
    }
    if ((gKeyPress & 2) || (gKeyRepeat & 4))
        return -1;
    if (gKeyRepeat & 0x40) {
        *r = *r - 1;
        if (*r < 0)
            *r = 0xf;
        return 0;
    }
    if (gKeyRepeat & 0x80) {
        *r = *r + 1;
        if (*r > 0xf)
            *r = 0;
        return 0;
    }
    if (gKeyRepeat & 0x20) {
        *q = *q - 1;
        if (*q < 0)
            *q = 0xf;
        return 0;
    }
    if (gKeyRepeat & 0x10) {
        *q = *q + 1;
        if (*q > 0xf)
            *q = 0;
        return 0;
    }
    if ((gKeyRepeat & 0x200) && (gKeyRepeat & 8)) {
        *p = *p - 0xa;
        if (*p < 0)
            *p = 0xf;
        return 1;
    }
    if ((gKeyRepeat & 0x100) && (gKeyRepeat & 8)) {
        *p = *p + 0xa;
        if (*p > 0xf)
            *p = 0;
        return 1;
    }
    if (gKeyRepeat & 0x200) {
        *p = *p - 1;
        if (*p < 0)
            *p = 0xf;
        return 1;
    }
    if (gKeyRepeat & 0x100) {
        *p = *p + 1;
        if (*p > 0xf)
            *p = 0;
        return 1;
    }
    return 0;
}
