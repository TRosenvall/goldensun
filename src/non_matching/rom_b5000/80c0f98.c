/* Func_80c0f98 -- asm/rom_b5000/rom_bffb8_c_c_a.s
 *
 * BLOCKER: REGISTER ROTATION inside both case bodies. 30 of 64, LENGTH EXACT.
 *
 * Sets a two-bit field (bits 2-3, mask -0xd) at offsets +5 and +0x11, either on
 * one object or across a null-terminated list of up to four, dispatched on the
 * low nibble of a byte at +0x54.
 *
 * ONE EDIT WAS WORTH 20 DIFFERENCES AND THE LENGTH: use a `switch`, not
 * `if / else if`.
 *
 *     rom    cmp r2,#1 / beq L1 / cmp r2,#2 / beq L2 / b L0
 *     ours   cmp r2,#1 / bne L1   (case 1 inlined as the fallthrough)
 *
 * The ROM evaluates BOTH tests up front and branches to separate blocks --
 * switch-dispatch shape. An if/else-if chain makes gcc fall through into the
 * first body instead, which costs two lines and shifts everything.
 * 50 of 62 -> 30 of 64. docs/elevation.md already notes that a wrong case
 * count can look like a register problem; this is the same warning one level
 * up -- the wrong CONTROL CONSTRUCT can too.
 *
 * WHAT REMAINS is a permutation inside both bodies:
 *
 *     rom    e->r4  mask->r2  v->r0    (and `p` in r0 is dead after the load)
 *     ours   e->r0  mask->r4  v->r1
 *
 * MEASURED AND FOLDED: the ROM does `and r5, r3` -- masking the PARAMETER IN
 * PLACE -- where ours computed into a temp. Writing `val &= 3; v = val << 2;`
 * to match is byte-identical to `v = (val & 3) << 2`, because `val` is dead
 * afterwards and gcc folds the two forms to one rtx.
 *
 * That is worth recording next to the copy-then-modify tell, which reads a
 * ROM `mov` before a modify as evidence of two names. The inverse does NOT
 * hold: an in-place modify in the ROM is not evidence of one name, because
 * both spellings compile identically when the value is dead. The tell is
 * one-directional.
 */
extern char *GetBattleActor(int a);

void Func_80c0f98(int actor, int val)
{
    char *p;
    char *e;
    char **list;
    int m;
    int v;
    int i;
    int t;

    p = GetBattleActor(actor);
    if (p == 0)
        return;
    p = *(char **)p;
    if (p == 0)
        return;
    t = *(unsigned char *)(p + 0x54) & 0xf;
    switch (t) {
    case 1:
        {
        e = *(char **)(p + 0x50);
        m = -0xd;
        val &= 3;
        v = val << 2;
        e[5] = (e[5] & m) | v;
        e[0x11] = (e[0x11] & m) | v;
        }
        break;
    case 2:
        {
        val &= 3;
        v = val << 2;
        list = *(char ***)(p + 0x50);
        m = -0xd;
        for (i = 0; i <= 3; i++) {
            e = *list++;
            if (e == 0)
                return;
            e[5] = (e[5] & m) | v;
            e[0x11] = (e[0x11] & m) | v;
        }
        }
        break;
    }
}
