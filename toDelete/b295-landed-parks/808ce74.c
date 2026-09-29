/* Func_808ce74 (0x0808ce74) -- NON-MATCHING, 2 encodings of 121 differ
 * (objcmp: "ENCODINGS differ in 2 place(s) (ref 121, ours 121)", size equal).
 * asm/rom_8a000/rom_8ba38_a_c_a.s holds 4 functions (FieldMain, this,
 * InitPlayerPos, Debug_PaletteEditor), so landing needs a split.
 *
 * Verify with:
 *   python3 tools/objcmp.py src/rom_8a000/rom_8ba38_a_c_a_a_b.c (landed in batch 295) \
 *       asm/rom_8a000/rom_8ba38_a_c_a_a.s --func Func_808ce74
 *
 * Built from the landed sibling Func_808bd24 (src/rom_8a000/rom_8ba38_a_a_a_c_a_b.c):
 * derived `&iwram_3001ebc - 0x4c`, signed `/ 0x200000` and `/ 0x100000`,
 * `t = base; t += ...`.  Two levers of its own took it from 97 to 2:
 *   - single `result` variable returned once (ROM keeps it in r10, set to 0
 *     before the GetFieldActor call);
 *   - the _Func_8011f54 call reads `v[0], v[2]` while the tile math reads
 *     `bp[0], bp[2]`: the different address RTL keeps GCSE from reusing the
 *     division inputs, so the ROM's reloads from [r5] appear.
 *
 * BLOCKER: one reload register.  In the else-arm the first `+0xfffff`
 * bias is `ldr r0,=0xfffff / add r3,r0` in the ROM, `ldr r2 / add r3,r2`
 * here.  It is a reload (the .greg shows insn 152 spilled), and 2.96's
 * allocate_reload_reg rotates through spill_regs from last_spill_reg: the
 * previous reload is the then-arm's second 0x1fffff in r0, r1 holds t, so r2
 * comes next.  The ROM picking r0 again means r2 was taken or the rotation
 * pointer sat at r2 -- consistent with the then-arm's `ldr r2,=ewram_2020000`
 * being a RELOAD in the ROM (it is a local-alloc pseudo, 74 in r2, here).
 * Not confirmed.
 *
 * Inert (all stay at 2): shared `x` for the first quotient in either arm;
 * explicit `if (x < 0) x += 0xfffff; x >>= 20` (8); `* 128`, `<< 7` swapped
 * (11); ewram spelled as `&[]`, `+ (int)`, `<< 2`, named offset, int-array
 * index; `unsigned char code`; `code >= 0xf2 && code <= 0xf7`; v/bp and t
 * declaration order.  `t = ewram_2020000; t += ...` in the then-arm: 26.
 */
extern unsigned char gState[];
extern unsigned char *iwram_3001ebc;
extern unsigned char ewram_2020000[];
extern void vec3_translate(int a, int b, int *v);
extern struct A *GetFieldActor(int id);
extern int _Func_8011f54(int a, int x, int z);
extern int FindMapActorEvent(int a, int b);

struct A {
    unsigned char pad00[6];
    unsigned short f06;
    int f08;
    int f0c;
    int f10;
    unsigned char pad14[0x22 - 0x14];
    unsigned char f22;
};

int Func_808ce74(void)
{
    int v[3];
    int *bp;
    unsigned char *g;
    unsigned char *p;
    unsigned char *q;
    struct A *e;
    unsigned char *t;
    int code;
    int result;
    int h;

    g = gState;
    result = 0;
    e = GetFieldActor(*(int *)(g + (0xfa << 1)));
    p = iwram_3001ebc;
    q = *(unsigned char **)((char *)&iwram_3001ebc - 0x4c);
    if (e != 0) {
        bp = v;
        bp[0] = e->f08;
        bp[1] = e->f0c;
        bp[2] = e->f10;
        vec3_translate(0x80 << 13, e->f06, bp);
        if (*(short *)(p + (0xcf << 1)) == 3) {
            t = ewram_2020000 + (((bp[0] / 0x200000) & 0x1f) + (((bp[2] / 0x200000) & 0x1f) << 5)) * 4;
        } else {
            t = *(unsigned char **)(q + (0x98 << 1));
            t += ((bp[0] / 0x100000) + ((bp[2] / 0x100000) << 7)) * 4;
        }
        code = t[2];
        if ((unsigned int)(code - 0xf2) <= 5) {
            h = _Func_8011f54(e->f22, v[0], v[2]);
            if (h >= e->f0c && h <= e->f0c + 0x400000)
                result = code;
        } else if (FindMapActorEvent(3, code) != 0) {
            result = code;
        }
    }
    return result;
}
