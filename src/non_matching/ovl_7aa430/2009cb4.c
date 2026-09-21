/* OvlFunc_923_2009cb4 (0x02009cb4) -- NON-MATCHING, 43 of 153.
 * Blocker class: global_alloc -- two high registers transposed.
 *
 * asm/overlays/rom_7aa430/ovl_1a3c_a_c_a_a.s (2 functions after batch 278's split, so landing
 * needs another split). TWIN: OvlFunc_924_200d244 -- 149 lines, BYTE-FOR-BYTE IDENTICAL
 * including every symbol and constant, the only fully-identical pair in this family.
 *
 * The algorithm is fully recovered: the control flow, the pool constants, the two /0x10000
 * divisions, the /8 and clamp, and the tail (`(iwram_3001e40 >> 1) & 1`, `q[5] = b * 7`,
 * `spr[0x25] = 1`) all line up.
 *
 * ===== TWO CALL IDIOMS COEXIST IN THIS ONE FUNCTION AND ONLY ONE IS PLAIN C =====
 *
 * `bl _call_via_r3` / `bl _call_via_r9` is what gcc emits for an ordinary function-pointer
 * local. `.call_via r4` / `.call_via r3` is a DIFFERENT THING: per include/macros.inc it
 * expands to `mov r12, pc / bx rN`, which gcc-2.96 never emits. It needs the
 * `static inline int call_via(...)` inline-asm helper already used by about ten accepted files
 * (src/overlays/rom_791794/ovl_30_c_c_a_a.c, src/rom_c9000/rom_e3958_c_c_c_a.c and others).
 * NONE of those files has a row in fakematch.txt, so it is established practice here rather
 * than scaffolding.
 *
 * In this function every Func_8000888 site uses that helper form and every other indirect call
 * is a plain pointer. Do not assume one idiom per function.
 *
 * AND THE HELPER'S CLOBBER LIST IS A REAL LEVER, which is not recorded anywhere else:
 *     "memory","r12"              73
 *     + "r2"                      45   <- best
 *     "memory","lr","r12"         73
 *     + "r4"                      48
 *     + "r2","r3"                 65
 *     + "r2","r4"                 58
 * Without "r2" in the list gcc treats the veneer as not clobbering r2 and parks the function
 * pointer there; the ROM parks it in r4 and keeps r3 LIVE across, so r3 must NOT be clobbered.
 *
 * REMAINING 43: `tx`/`ty` land in r9/r11 where the ROM has r11/r9, and the scratch r1/r2 choices
 * follow from that. Swapping the two reads 43; swapping the declarations 44; swapping the two
 * divisions 74; swapping the dx/dy declarations 45. Reusing dx/dy for both the divided and
 * undivided deltas (ONE variable, not two pairs) was worth 141 -> 138 and is kept. Naming the
 * Func_8000888 pointer in a local is WORSE (117) than passing the symbol to the helper (73/45).
 *
 * NEXT: price the tx/ty contest with the batch-277 formula from `.17.lreg` before spelling
 * anything else.
 */
extern int Func_8000948(int a);
extern int Func_8000888(int a, int b);
extern int Func_80008ac(int a, int b);
extern int __FastIntSqrtFP1616_RAM(int a);
extern unsigned int iwram_3001e40;

static inline int call_via(int (*f)(int, int), int a, int b)
{
    register int _a __asm__("r0") = a;
    register int _b __asm__("r1") = b;
    __asm__ volatile (
        "\t.align\t2, 0\n"
        "\tmov\tr12, pc\n"
        "\tbx\t%1"
        : "=r" (_a)
        : "r" (f), "0" (_a), "r" (_b)
        : "memory", "r12", "r2"
    );
    return _a;
}

void OvlFunc_923_2009cb4(unsigned char *a)
{
    unsigned char *p;
    unsigned char *spr;
    unsigned char *q;
    int (*f1)(int);
    int (*h)(int, int);
    int tx;
    int ty;
    int dx;
    int dy;
    int mag;
    int step;
    int b;

    *(int *)(a + 0x30) = 0x80 << 10;
    *(int *)(a + 0x34) = 0x80 << 9;
    p = *(unsigned char **)(a + 0x68);
    ty = *(int *)(p + 0x10);
    tx = *(int *)(p + 8);
    *(int *)(a + 0x38) = 0x80 << 24;
    *(int *)(a + 0x3c) = 0x80 << 24;
    *(int *)(a + 0x40) = 0x80 << 24;
    dx = (tx - *(int *)(a + 8)) / 0x10000;
    dy = (ty - *(int *)(a + 0x10)) / 0x10000;
    f1 = Func_8000948;
    mag = f1(dx * dx + dy * dy) << 16;
    dx = tx - *(int *)(a + 8);
    dy = ty - *(int *)(a + 0x10);
    if (mag < (0x80 << 15))
        mag = __FastIntSqrtFP1616_RAM(call_via(Func_8000888, dx, dx)
                                      + call_via(Func_8000888, dy, dy));
    step = mag / 8;
    if (step > *(int *)(a + 0x30))
        step = *(int *)(a + 0x30);
    if (mag < (0x80 << 7)) {
        *(int *)(a + 8) = tx;
        *(int *)(a + 0x10) = ty;
    } else {
        if (mag > step) {
            h = Func_80008ac;
            dx = call_via(Func_8000888, h(mag, dx), step);
            dy = call_via(Func_8000888, h(mag, dy), step);
        }
        *(int *)(a + 8) += dx;
        *(int *)(a + 0x10) += dy;
    }
    b = (iwram_3001e40 >> 1) & 1;
    spr = *(unsigned char **)(a + 0x50);
    q = *(unsigned char **)(spr + 0x28);
    q[5] = b * 7;
    spr[0x25] = 1;
}
