/* Func_80a51d0 -- NOT MATCHING
 *
 * Source asm: goldensun/asm/rom_a1000/rom_a4f08.s
 * Best screen: 50 instructions against the ROM's 49, 24 differing.
 *
 * BLOCKER CLASS: how many times an indirect global is re-read, and then
 * scheduling on top.
 *
 * The ROM reads `*(p + 0x21c)` FIVE times -- once for the field it passes to
 * _Func_801bcd4 and once before each of the three stores and the call:
 *
 *     ldr r3, [r5] ... ldr r2, [r5] / strb ... ldr r2, [r5] / strh ...
 *     ldr r2, [r5] / strh ... ldr r0, [r5] / bl
 *
 * Plain `(*s)->f5 = 1;` and friends let gcc keep the pointer in one register:
 * 47 instructions against 49, 37 differing. Declaring the pointer `volatile`
 * forces all five reads and gets the shape right, but overshoots by one
 * instruction and leaves 24 differing, all of it the two offset computations
 * scheduled differently and an r2/r3 exchange.
 *
 * ALSO TRIED: -fno-strict-aliasing (identical to the volatile form, 50/24),
 * -fno-rerun-cse-after-loop and -fno-gcse (both identical to the plain form).
 *
 * So the re-read count is reachable and the residue is not. The volatile
 * spelling is NOT kept below -- it is a lie about the object and it does not
 * match either; the plain form is kept as the honest starting point, and the
 * volatile result is recorded here so it is not re-derived.
 */
struct S {
    unsigned char pad00[5];
    unsigned char f5;
    unsigned short f6;
    unsigned short f8;
    unsigned char pad0a[4];
    unsigned char fe;
};

extern char *iwram_3001f2c;
extern void _Func_801bcd4(int a, int b, int c, int d);
extern void Func_80a17c4(struct S *s);
extern int _GetUnit(int id);
extern void _Func_801e8b0(int a, void *b, int c, int d);
extern void _Func_801e7c0(int a, void *b, int c, int d);

void Func_80a51d0(void)
{
    char *p;
    struct S **s;
    unsigned short *w;
    void **q;

    p = iwram_3001f2c;
    s = (struct S **)(p + (0x87 << 2));
    w = (unsigned short *)(p + (0xbc << 1));
    _Func_801bcd4(2, *w, (*s)->fe, 0);
    (*s)->f5 = 1;
    (*s)->f6 = 0x70;
    (*s)->f8 = 8;
    Func_80a17c4(*s);
    q = (void **)(p + (0x86 << 1));
    _Func_801e8b0(_GetUnit(*(unsigned char *)(p + 0x21a)), *q, 0x10, 0);
    _Func_801e7c0((*w & 0x1ff) + 0x182, *q, 0x10, 8);
}
