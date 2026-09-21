/* Cluster Func_80a1d08..Func_80a1d08 extracted from goldensun/asm/rom_a1000/rom_a1814_c_a_a_c_a_c_a_a_c.s.
 *
 * Total .text for this TU = 304 bytes (= 0x130). Never attempted before batch 277.
 * No pins, no new symbols. 127 instructions, three candidates.
 *
 * `volatile` HERE IS NOT SCAFFOLDING AND COSTS NO fakematch ROW.
 * `extern volatile unsigned int gKeyPress;` is this tree's established declaration for the
 * hardware-updated input register -- six landed files use it and none of them is booked in
 * fakematch.txt (verified at landing). Without it gcc CSEs the three gKeyPress reads in the
 * A/B/Start spin loop into one, which costs an extra global register and a wider push list:
 * 137 differing to 16.
 *
 * THE STATE BASE MUST BE A STRUCT POINTER, NOT AN INTEGER. `*(u16 *)(base + 0x222) = 1`
 * ALWAYS emits a pooled `ldrh`; `s->f222 = 1` through a real struct pointer with a far
 * field emits the ROM's `mov r3, #1 / strh`. Probed in isolation to be sure it was the
 * typing and not the offset: `*(u16 *)p = 1` pools, `struct S *s; s->far_h = 1` does not.
 * That closed the last 14.
 *
 * AND THAT SETTLES A READING OF THE ROM'S POOL WORTH CARRYING -- see const.sym, which this
 * batch annotates. The pooled HImode word here is SHARED WITH A FOLLOWING `strb` of the
 * same value, which is why the ROM shows `mov r3,#1 / strh` beside `ldr r1,=1 / strb`. THAT
 * IS NOT THE SYMBOL TELL: const.sym's _CONST_1 reproduces the byte store but leaves a
 * fourth pool word, while the plain literal 1 plus a struct halfword field matched exactly.
 * A pooled small constant feeding a `strb` can be a shared HImode pool entry.
 *
 * HOW TO READ A ROM POOL, measured twice here: POOL ENTRIES ARE NOT IN REFERENCE ORDER --
 * HImode entries sort BEFORE SImode ones. A `.word 1` from an `ldrh` landed at pool offset 0
 * ahead of an 0x151 referenced earlier in the insn stream. So this function's pool
 * [1, 0x151, 0x222] at 0x080a1e1c says the 1 is a HALFWORD entry, and that alone is what
 * revealed the byte store was sharing it.
 */
extern volatile unsigned int gKeyPress;

struct Menu {
    unsigned char pad00[0x14];
    unsigned char *f14;
    unsigned char pad18[0x2c - 0x18];
    unsigned int f2c;
    unsigned char pad30[0x3c - 0x30];
    unsigned int f3c;
    unsigned char pad40[0x222 - 0x40];
    unsigned short f222;
};

extern struct Menu *iwram_3001f2c;

extern void _TextBox(int id, int *a, int *b, int *c, int *d);
extern int Func_80a10d0(unsigned int *slot, int a, int b, int c, int d, int e);
extern void Func_80a23f4(unsigned int win, int a, int b, int c, int d);
extern void _Func_8016498(unsigned int win);
extern void _Func_80164ac(unsigned int win);
extern void _Func_801e7c0(int id, unsigned int win, int x, int y);
extern void _DrawSmallText(int id, unsigned int win, int x, int y);
extern void WaitFrames(int n);
extern void _SetFlag(int id);
extern void Func_80a1114(unsigned int *slot, int a);

void Func_80a1d08(int id, int wait, int reuse)
{
    struct Menu *s;
    unsigned int *slot;
    unsigned int win;
    int v0, v1, v2, v3;

    s = iwram_3001f2c;
    s->f14[5] = 0xd;
    if (reuse != -1) {
        _TextBox(id, &v3, &v2, &v1, &v0);
        slot = &s->f3c;
        if (Func_80a10d0(slot, wait, reuse, v1, v0, 0x81 << 1) == 0)
            Func_80a23f4(*slot, wait, reuse, v1, v0);
        win = *slot;
    } else {
        win = s->f2c;
    }
    _Func_8016498(win);
    _Func_80164ac(win);
    if (reuse == -1)
        _Func_801e7c0(id, win, 0, 0);
    else
        _DrawSmallText(id, win, 0, 0);
    if (wait != -1) {
        WaitFrames(1);
        while (1) {
            WaitFrames(1);
            if (gKeyPress & 1)
                break;
            if (gKeyPress & 2)
                break;
            if (gKeyPress & 8)
                break;
        }
        if (reuse == -1)
            _Func_8016498(win);
        _Func_80164ac(win);
    } else {
        _SetFlag(0x151);
    }
    s->f222 = 1;
    s->f14[5] = 1;
    if (reuse != -1)
        Func_80a1114(&s->f3c, 1);
}
