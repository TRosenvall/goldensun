/* Func_801b9ec -- 0x0801b9ec, the TWIN of Func_801b9a8: identical instruction
 * for instruction plus one trailing `bl Func_801c188`.
 *
 * AFTER the split that p3_candidate.c needs, this is the ONLY function left in
 * asm/rom_15000/rom_1aeec_a_a_c_a_c_c_c_c_c.s, so it converts WHOLE-FILE with
 * no further split.
 *
 * MATCHES to the same single `_CONST_1f` pool word: 33 of 33 instructions,
 * size exact, only `ref 0000001f / ours 00000000` plus the extra
 * `R_ARM_ABS32 _CONST_1f` relocation the link resolves.
 * NO PINS, no shim, no fakematch row, no per-file flag.
 *
 * The park for Func_801b9a8 said "whatever lands this lands both" and it is
 * right. Everything in p3_candidate.c's header applies verbatim; the only
 * addition is the trailing call.
 *
 * Verify with (AFTER the split):
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/rom_15000/rom_1aeec_a_a_c_a_c_c_c_c_c.c \
 *     asm/rom_15000/rom_1aeec_a_a_c_a_c_c_c_c_c.s --whole
 * Before the split, against the three-function reference:
 *   ... python3 tools/objcmp.py <this file> \
 *     asm/rom_15000/rom_1aeec_a_a_c_a_c_c_c_c.s --func Func_801b9ec
 */
extern void LoadOldUIIcon(int id, int b, int *x, int *y, int flag);
extern void Func_801c188(void);
extern int _CONST_1f;

void Func_801b9ec(char *p, int n)
{
    char *node;
    int a;
    int b;
    int t;
    int id;

    node = *(char **)(p + 0xd2 * 4);
    while (n != 0) {
        n--;
        node = *(char **)(node + 4);
    }
    t = *(unsigned short *)(node + 0xa);
    if (t == 1 || t == 6) {
        id = *(unsigned short *)(node + 0x20) - (int)&_CONST_1f;
        a = *(unsigned short *)(node + 0xc);
        LoadOldUIIcon(id, 0, &a, &b, 1);
        Func_801c188();
    }
}
