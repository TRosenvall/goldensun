/* OvlFunc_924_200d244 (0x0200d244) -- NON-MATCHING, 40 encodings of 153 differ, SAME LENGTH.
 *
 * asm/overlays/rom_7ac2d8/ovl_35b8_a_c_a_a.s (1 function, no data -- no split needed).
 *
 * Verify with:
 *   python3 tools/objcmp.py src/non_matching/ovl_7ac2d8/200d244.c asm/overlays/rom_7ac2d8/ovl_35b8_a_c_a_a.s --func OvlFunc_924_200d244
 *
 * TWIN: the parked OvlFunc_923_2009cb4 (src/non_matching/ovl_7aa430/2009cb4.c) is
 * byte-for-byte the same function; this body is that park's with one change.
 *
 * THE CHANGE -- IT RETURNS int. The ROM epilogue is `pop {r1} / bx r1` (the batch-286
 * "pop {r1} = returns int" tell); the twin park declares it void (`pop {r0}`).
 * Declaring it `int` with NO return statement: 43 -> 40 (same on the twin: 43 -> 40).
 * `return 1;` / `return b;` / a named one `k` shared by the mask, the f25 store and the
 * return are all worse (128-131, the size flips) -- the ROM's `mov r0,#1` shared by
 * `and` and `strb` is not reached that way.
 *
 * WHAT IS LEFT: tx/ty in r9/r11 where the ROM has r11/r9, and the reload/scratch regs
 * that follow. Read from .17.lreg: tx and ty are "used 4 times across 49/51 insns" --
 * whichever is loaded SECOND has the shorter life and wins global_alloc. Loading tx
 * first ties them EXACTLY (4/50 each); the tie then breaks on pseudo number, so
 * loading tx first AND declaring ty before tx gives the ROM's tx=r11 / ty=r9.
 * But that spelling is 4 BYTES SHORT (320 vs 324): the tx load's output reload
 * lands in r1 (allocate_reload_reg's round robin -- find_reg chose r3, see
 * "Using reg 3 for reload 0" at that insn in .18.greg), so the later `tx - a->x`
 * INHERITS r1, where the ROM's reload took r3, the 0x80<<24 constant then clobbered r3,
 * and the ROM re-copies with `mov r2, r11` -- one extra instruction, which also moves
 * the `.call_via` alignment padding. So the correct allocation costs the length until
 * the reload round robin is also right: the ROM also has p in r2 (ours r3).
 *
 * INERT / WORSE: a typed struct (fields x/y/z, f30.., f50, f68) in place of the byte
 * offsets -- same allocation, 131 at the short length; all 60 legal orders of
 * {f30 store, f34 store, p load, tx load, ty load, f38 store} with tx before ty and ty
 * declared first: 58 are 320 bytes, (f30,p,tx,f38,f34,ty) and (f30,p,f38,f34,tx,ty)
 * are the right length at 47; the twin park's own inert list (read swap, decl swap,
 * division swap, clobber lists) still applies.
 *
 * NEXT: get the first reload of the function (the tx load) into r3 -- per batch 282 the
 * round robin is function-scoped and inherited reloads advance it silently, so look
 * for a spelling that puts a live pseudo in r1 (and p in r2) across the tx load.
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

int OvlFunc_924_200d244(unsigned char *a)
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
