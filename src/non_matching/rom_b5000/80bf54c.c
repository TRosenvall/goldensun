/*
 * Func_80bf54c (TickStatusCounterB) -- asm/rom_b5000/rom_bbb0c_a_c_c_a_b.s
 * SPLIT OUT of an eight-small-function file this round; byte-neutral, verified.
 *
 * BLOCKER: register roles. 19 lines against 19, 8 differing. The ROM computes
 * the field address into r1 and loads into r2, then copies to r3; we compute
 * into r2 and load into r3.
 *
 * SETTLED, and it is what got the LENGTH right: the counter must be an
 * `unsigned char` local, not an int. With `int v` the function is one
 * instruction SHORT (18 against 19, 16 differing) because gcc drops the
 * truncation; `unsigned char v` restores it. Reading the byte twice -- the
 * lever that works on the +0x27 family -- does NOT add the copy here (still 18
 * lines), and an explicit second local for the test scores 16 differing.
 *
 * The decrement is `v + 0xff`, not `v - 1`: gcc emits `sub r3, #1` for the
 * latter and the ROM has `add r3, #0xff`.
 */
extern unsigned char *_GetUnit(int id);

int Func_80bf54c(int id)
{
    unsigned char *p;
    int off;
    unsigned char v;

    off = 0x13f;
    p = _GetUnit(id) + off;
    v = *p;
    if (v != 0) {
        v = v + 0xff;
        *p = v;
        if ((unsigned char)v == 0)
            return 1;
    }
    return 0;
}
