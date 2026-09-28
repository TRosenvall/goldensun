/* Func_80a24d0 (0x080a24d0) -- 163 instructions.  Returns Func_80a2680's answer.
 * NON-MATCHING: 166 encodings of 174 differ (objcmp).
 * ours 181 encodings / 448 bytes against ref 174 / 432, so 166 is NOT a true
 * distance.  tryc.py --align says 75 instructions in disagreeing regions of
 * 165, and that is the figure that ranks variants.
 *
 * Reference: asm/rom_a1000/rom_a1814_c_a_c_c_a_c_c_c_c_a.s -- TWO functions
 * (this one and the 1392-line state machine Func_80a2680 at 0x080a2680), so
 * landing needs a split.  datacheck clean; the only mentions of Func_80a2680's
 * labels from outside its body are in the PROSE COMMENT above its
 * .thumb_func_start, so there is no real cross-reference and the split needs
 * no export.  Its `.word .La26f0 ...` run is its own jump table, inside itself.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/rom_a1000/80a24d0.c \
 *     asm/rom_a1000/rom_a1814_c_a_c_c_a_c_c_c_c_a.s --func Func_80a24d0
 *
 * STRUCTURE: the screen-driver skeleton of the LANDED, byte-exact Func_80a7380
 * (src/rom_a1000/rom_a7380_a_b.c) -- galloc_iwram(0x37, 0xa70),
 * `*(short *)(e[0] + 4) = 1`, _Func_80170f8(0,0,0x1e,0x14), WaitFrames(1),
 * Func_80a1090(0), `*(u8 *)(p + 0x219) = _Func_80796c4(p + 0x208)`,
 * `*(int *)(p + 0x10c) = _CreateUIBox(0xd, 0, 0x11, 3, 2)`,
 * _Func_80164ac(*(int *)(p + 0x24)), Func_80a34c0(), gfree(0x37), and the
 * `iwram_3001e68` base NAMED once because cse cannot relate two distinct
 * SYMBOL_REFs (the note in src/non_matching/rom_a1000/80a5b94.c) -- the ROM
 * reaches +0, +0x24 and +0x54 through the single register r8.
 *
 * WHAT IS ALREADY RIGHT: every call, in order, with its arguments; the three
 * `(*g)[0xea6]` byte stores; the `if (ret == 1)` block; the two indirect calls
 * through function-pointer VARIABLES (a cast folds to a direct `bl`); and the
 * frame, 0x10 with the three out-parameters of Func_80a2680 at sp+4/8/0xc.
 *
 * BLOCKER -- local-alloc.c's REG_EQUIV penalty feeding global.c.  The ROM and
 * this candidate disagree about WHICH THREE constants get callee-saved
 * registers.  ROM: r9 = 0x2000 (four uses), r11 = the Func_8001af8 pointer
 * (two uses), r10 = the return value, and 0x6004000 is RE-MATERIALISED from the
 * pool at both its sites (`ldr r1,=0x6004000`, `ldr r0,=0x6004000`).  Ours:
 * r10 = 0x6004000, r11 = the return value, and BOTH 0x2000 and the function
 * pointer are re-materialised -- exactly inverted.
 *
 * The mechanism, read out of local-alloc.c:869: every pseudo that carries a
 * REG_EQUIV note for a function-invariant constant has
 *     REG_LIVE_LENGTH (regno) *= 2;
 * applied to it before global allocation, and allocno_compare ranks on
 * floor_log2(n_refs) * n_refs / live_length -- so naming a constant HALVES its
 * own priority.  The -da .18.greg dump confirms it: the `;; N regs to
 * allocate:` line IS the sorted priority order, and the 0x2000 pseudo (its
 * REG_EQUIV (const_int 8192) is in .17.lreg) does not appear among the winners.
 *
 * MEASURED INERT -- and this is the part worth recording, because it
 * contradicts the "name a constant at ALL sites" lever as stated:
 *   naming 0x6004000 as a local used at all three sites ..... 75  IDENTICAL
 *   removing the `len` local, 0x80<<6 written at all 4 sites  75  IDENTICAL
 * Both produce byte-for-byte the same assembly as this file.  The naming has NO
 * effect here at all, in either direction, because update_equiv_regs collapses
 * the named local back into its REG_EQUIV constant before allocation.  The
 * calls.c:855 argument-precompute lever is real but it does not reach a
 * constant this cheap to re-materialise.
 *
 * SMALLER RESIDUE, all downstream of the above: gcc chains its move2add
 * constants differently in the `ret == 1` block (the ROM derives 0x180 from the
 * live 0x1ff by `sub #0x7f` and 0x19a from 0x174 by `add #0x26`; we derive
 * 0x174 from 0x1ff and build 0x180 and 0x19a fresh), and the two small
 * constants 1 and 0 land on the opposite sides of the pool/immediate choice
 * from the ROM's -- see the note in the sibling park about that choice NOT
 * being evidence for a symbol.
 */
extern unsigned char *iwram_3001e68[];
extern void *Func_8004970(int size);
extern unsigned char *galloc_iwram(int tag, int size);
extern void _Func_80170f8(int a, int b, int c, int d);
extern void WaitFrames(int n);
extern void Func_80a1090(int a);
extern int _Func_80796c4(unsigned char *buf);
extern void Func_80a3354(int a, int b, int c, int d);
extern void Func_80a5534(void);
extern void Func_80a2144(int a);
extern void _Func_80219c8(int addr);
extern int _CreateUIBox(int a, int b, int c, int d, int e);
extern void Func_80a1070(void);
extern void Func_8001af8(void *dst, void *src, int len);
extern void Func_80008d8(void *dst, int len, int val);
extern void _Func_801e3c8(int a);
extern void Func_80a2474(void);
extern int Func_80a2680(int *a, int *b, int *c);
extern void Func_80a2490(void);
extern void _Func_80164ac(int a);
extern void Func_80a34c0(void);
extern void Func_80ae8dc(void);
extern void gfree(int tag);
extern void _Func_801e318(void);
extern void free(void *p);
extern void Func_80a1050(void);
extern void _ClearUIRegion(int a, int b, int c, int d);
extern void _Func_8091858(void);

int Func_80a24d0(void)
{
    void (*copy)(void *, void *, int);
    void (*fill)(void *, int, int);
    void *buf;
    unsigned char *p;
    unsigned char **e;
    unsigned char **g;
    unsigned char *q;
    int len;
    int ret;
    int ofs;
    int a;
    int b;
    int c;

    len = 0x80 << 6;
    buf = Func_8004970(len);
    p = galloc_iwram(0x37, 0xa7 << 4);
    e = iwram_3001e68;
    *(short *)(e[0] + 4) = 1;
    _Func_80170f8(0, 0, 0x1e, 0x14);
    WaitFrames(1);
    Func_80a1090(0);
    *(unsigned char *)(p + 0x219) = _Func_80796c4(p + (0x82 << 2));
    Func_80a3354(0, 3, 0, 7);
    Func_80a5534();
    Func_80a2144(0xe);
    _Func_80219c8(0x6002500);
    *(int *)(p + (0x86 << 1)) = _CreateUIBox(0xd, 0, 0x11, 3, 2);
    Func_80a1070();
    copy = Func_8001af8;
    copy(buf, (void *)0x6004000, len);
    fill = Func_80008d8;
    fill((void *)0x6004000, len, 0x33333333);
    _Func_801e3c8(1);
    Func_80a2474();
    ret = Func_80a2680(&a, &b, &c);
    Func_80a2490();
    if (ret == 1) {
        q = e[21];
        *(short *)(q + 0x180) = (a << 10) | (c & 0x1ff);
        *(short *)(q + 0x19a) = *(short *)(p + (0xba << 1));
    }
    _Func_80164ac(*(int *)(p + 0x24));
    g = &e[9];
    ofs = 0xea6;
    (*g)[ofs] = 1;
    Func_80a34c0();
    _Func_80170f8(0, 0, 0x1e, 0x14);
    Func_80ae8dc();
    gfree(0x37);
    *(short *)(e[0] + 4) = 0;
    _Func_801e318();
    _Func_801e3c8(0);
    copy((void *)0x6004000, buf, len);
    (*g)[ofs] = 0;
    free(buf);
    WaitFrames(1);
    Func_80a1050();
    WaitFrames(1);
    _ClearUIRegion(0, 0, 0x1e, 0x14);
    (*g)[ofs] = 0;
    _Func_8091858();
    return ret;
}
