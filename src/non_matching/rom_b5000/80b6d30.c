/* Func_80b6d30 (AssignBattlePositions) -- NON-MATCHING, 4 encodings of 119.
 * 0x080b6d30, the only function in asm/rom_b5000/rom_b5a0c_c_c_c_a_c.s (no data),
 * so landing would be a plain whole-file conversion. Fresh in batch 287.
 *
 * objcmp: XX ENCODINGS differ in 4 place(s) (ref 119, ours 119). SIZE EXACT.
 * NO SHIM, NO PIN, NO FLAG.
 *
 * Verify with:
 *   python3 tools/objcmp.py src/non_matching/rom_b5000/80b6d30.c \
 *     asm/rom_b5000/rom_b5a0c_c_c_c_a_c.s --func Func_80b6d30
 *
 * The inner slot search is the landed Func_80b6cdc (rom_b5a0c_c_c_c_a_b.c)
 * verbatim, and the two slot stores reuse its `off`/`a` idiom -- in a NEW
 * block-scoped offset variable (a fresh `int o`; reusing the loop's `off` puts
 * it in r2 instead of the ROM's r0, 10 differing).
 *
 * THE RESIDUE, TWO PLACES:
 *
 *  1. `mov r4, r10` (ROM) against `mov r4, #0` (ours): the outer counter j is
 *     initialised by COPYING the already-zero `ret` out of r10, after the
 *     Func_80c2384 call. cse1 replaces any `j = ret` with the constant (the
 *     .03.cse dump shows `(set (reg j) (const_int 0))` with REG_EQUAL);
 *     -fno-gcse / -fno-cse-follow-jumps / -fno-rerun-cse-after-loop /
 *     -fno-strength-reduce all leave it at 4. Tried: `for (j = ret; ...)`,
 *     `j = ret` before and after the call, `ret = j = 0`, `j = ret = 0`,
 *     `j = 0; ...; ret = j` (all 4 or far worse, 77-104). The live-zero lever
 *     in rom_b8228_c_a_c_c_a_c_a_c_b.c does not transfer: there the zero is in
 *     another EBB. Something the ROM's author wrote keeps ret's zero out of
 *     cse1's table; not found.
 *  2. `lsl r3,r5,#12 / orr r3,r7 / mov r10,r3` against r2: the reload
 *     register for `ret = (i << 12) | v` (ret lives in r10). .18.greg says
 *     "Using reg 3 for reload 0" for both insns yet the output is r2. Inert:
 *     `v | (i << 12)`, `ret = i << 12; ret |= v`, a block-local temp, `!j`.
 *     `+` instead of `|` is far worse (56). Probably tied to (1): the ROM has
 *     one more register-held value at that point.
 */
extern unsigned char *_GetUnit(int id);
extern int Func_80c23c0(int a);
extern int Func_80c2384(int a);
extern int Func_80c23a0(int a);
extern int _PreloadSpriteGFX(int a, int b, int c, int d);
extern char *iwram_3001e74;
extern unsigned char ewram_2018000[];

int Func_80b6d30(int slot)
{
    char *s;
    unsigned char *u;
    int flag;
    int v;
    int ret;
    int j;
    int i;
    int off;
    int a;

    s = iwram_3001e74;
    u = _GetUnit(slot);
    flag = Func_80c23c0(u[0x128]);
    ret = 0;
    v = Func_80c2384(u[0x128]);
    for (j = ret; j <= 1; j++) {
        if (u[0x129] != 0)
            continue;
        for (i = 0; i <= 5; i++) {
            off = i * 2;
            a = off + 4;
            if (*(short *)(s + a) != 0)
                continue;
            if (flag != 0)
                break;
            if (i > 4)
                continue;
            a = off + 6;
            if (*(short *)(s + a) == 0)
                break;
        }
        if (i == 6)
            break;
        if (_PreloadSpriteGFX(i, (int)(ewram_2018000 + (i << 14)), v + j,
                              Func_80c23a0(u[0x128])) == 0)
            return 0;
        if (j == 0)
            ret = (i << 12) | v;
        {
        int o = i * 2;
        a = o + 4;
        *(short *)(s + a) = slot;
        if (flag == 0) {
            a = o + 6;
            *(short *)(s + a) = slot;
        }
        }
        if (v == 0x1dc || v == 0x1e3)
            continue;
        break;
    }
    return ret;
}
