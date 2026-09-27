/* OvlFunc_968_200c520 -- 0x0200c520, asm/overlays/rom_7f2f14/ovl_30_c_c_a_c_c.s
 *
 * objcmp: SIZE ref 208 bytes, ours 204; ENCODINGS differ in 87 place(s)
 * (ref 93, ours 91) -- 87 encodings of 93, but it is ONE global-alloc swap:
 * instruction for instruction the stream is the ROM's with two pseudos'
 * hard registers exchanged, plus the two reload moves that swap removes.
 *
 * Verify with:
 *   python3 tools/objcmp.py src/non_matching/ovl_7f2f14/200c520.c asm/overlays/rom_7f2f14/ovl_30_c_c_a_c_c.s --func OvlFunc_968_200c520
 *
 * Spawns one particle through OvlFunc_968_2008118 on frames where
 * iwram_3001e40 & 3 == 0, in one of two shapes chosen by a coin flip. Uses the
 * struct P spelling of the landed sibling src/overlays/rom_7f2f14/ovl_30_c_c_c_c_a.c.
 *
 * BLOCKER: register assignment of x vs tp (&t).
 *     rom   tp -> r8 (hence `add r2,sp,#16 ... mov r8,r2` and `mov r2,r8` /
 *           `mov r3,r8` reloads), x -> r7
 *     ours  tp -> r7, x -> r8
 * Read from -da .17.lreg: x (reg 32) "used 4 times across 47 insns", tp
 * (reg 38) "used 6 times across 58 insns". global.c priority is
 * floor_log2(refs)*refs/live_length: tp 12/58 = .207 beats x 8/47 = .170, so
 * tp is coloured first and takes r7. For the ROM order x needs a 5th ref
 * (10/47 = .213) or tp must drop to 4 refs. tp's six refs (set, three field
 * stores, two call args) are all real; writing the stores through `t.` instead
 * of tp changes nothing because CSE re-routes them through the same pseudo.
 *
 * INERT: t.f22 vs tp->f22; tp assigned before/after the f8/fc stores; unsigned
 * params; `(x << 19) - 0x40000`; literal 0 vs n/k for the zero stack args (CSE
 * substitutes the known-zero register either way). `short` params (79, adds
 * extensions). A `register ... __asm__("r7")` pin on x is not honoured across
 * the calls and wrecks the function (92).
 */
struct P {
    int f0;
    int f4;
    int f8;
    int fc;
    unsigned char pad10[0x18 - 0x10];
    unsigned short f18;
    unsigned char pad1a[0x1c - 0x1a];
    void *f1c;
    unsigned char pad20[0x22 - 0x20];
    unsigned short f22;
    unsigned char pad24[0x28 - 0x24];
};

extern volatile unsigned int iwram_3001e40;

extern unsigned int __Random(void);
extern int _divsi3_RAM(int a, int b);
extern void OvlFunc_968_2008118(int a, int b, int c, int d,
                                int e, int f, int g, struct P *p);

void OvlFunc_968_200c520(int x, int y)
{
    struct P t;
    unsigned int n;
    unsigned int k;
    unsigned int r;
    int e;
    struct P *tp;

    tp = &t;
    tp->f8 = 0xb333;
    tp->fc = 0xb333;
    tp->f22 = (__Random() * 0x1000 >> 16) + (0xf8 << 8);
    n = iwram_3001e40 & 3;
    if (n == 0) {
        k = __Random() * 2 >> 16;
        if (k != 0) {
            r = __Random();
            e = _divsi3_RAM(((__Random() * 5 >> 16) << 16) + (0xe0 << 11), 10);
            OvlFunc_968_2008118((x + ((r * 2 >> 16) << 4)) << 16, 0, y << 19, 0,
                                n, e, 0x88 << 16, tp);
        } else {
            OvlFunc_968_2008118((x + (__Random() * 17 >> 16)) << 16, 0,
                                (x << 19) + 0xfffc0000, 0, k, k, 0x88 << 16, tp);
        }
    }
}
