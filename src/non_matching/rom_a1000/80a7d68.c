/* Func_80a7d68 -- NON-MATCHING, 167 encodings of 211.  ref 211 instructions / ours 209,
 * ref 476 bytes / ours 472.
 *
 * (The agent reported 160 for its best candidate and the body shipped here measures 167 --
 * parkcheck.py caught the discrepancy before the commit.  The 160 candidate was not the one
 * handed over, so 167 is what this file can be held to; the lever below is what produced
 * the improvement either way.)
 *
 * Verify with:
 *   python3 tools/objcmp.py src/non_matching/rom_a1000/80a7d68.c \
 *     asm/rom_a1000/rom_a7380_a_c_a.s --func Func_80a7d68
 *
 * NOTE THE REFERENCE PATH: moved into the `_a` part by batch 285's split of
 * rom_a7380_a_c.s to land Func_80a7f44.
 *
 * ONE LEVER LANDED, 187 -> 160, AND IT IS REUSABLE: THE TWO MESSAGE IDS ARE ONE
 * INCREMENTED VARIABLE.  `id = 0xc05; ... id++;` reproduces the ROM's single
 * `ldr r5, =0xc05` plus `add r5, #1`, where two literals give TWO pool loads and TWO pool
 * words.  Same family as the _MSG_* base-plus-arithmetic shape, but with no symbol
 * involved -- the arithmetic is on a plain pooled literal.
 *
 * BLOCKER: a three-way callee-saved rotation -- `base` r7 vs the ROM's r10, `sel` r8 vs
 * r7, `off` r10 vs r8 -- in global_alloc.  The ROM's order needs `base` ABOVE `sel`, and
 * this C references `sel` roughly TWICE as often as the original (35 weighted refs against
 * about 16 readable in the ROM).  So the search is for a shape that computes `sel * 2`
 * ONCE rather than in both arms of the dirty test -- a REFERENCE-COUNT change, not a
 * naming or ordering one.
 *
 * THE PRIORITY FORMULA IS A READOUT, NOT A GUESS: the -da .17.lreg dump prints both terms
 * of floor_log2(n_refs) * n_refs / live_length, and on three functions in this file it
 * reproduced the printed `;; N regs to allocate:` order EXACTLY, ties included.  Check the
 * arithmetic before writing a spelling; 30 + 72 + 14 + 8 spellings were burned confirming
 * declaration order is INERT against this class, because declaration order is a
 * local-alloc/slot lever and this is global_alloc.
 */
/* Func_80a7d68 -- 0x080a7d68, asm/rom_a1000/rom_a7380_a_c.s.  PARKED.
 *
 *   objcmp: XX SIZE ref 476 bytes, ours 472
 *           XX ENCODINGS differ in 167 place(s) (ref 211, ours 209)
 *           first at index 7: ref 4b3a ours 4b3b
 *   tryc:   rom 213 lines, ours 212, first diff at 8, 160 differ
 *
 * The djinn/party picker: reads the selection from base+0x1c and the count from
 * base+0x1e as SIGNED bytes (`ldrsb`, so `signed char` casts are load-bearing),
 * then loops -- redraw when dirty, poll the keys, wrap the selection with
 * `(sel + n) % n` through __modsi3 -- and writes the selection back on the way
 * out. Both exits (`return 1` on A, `return -1` on B) share one cross-jumped
 * tail at .La7f0e, which is why the stores after the loop are written once with a
 * `ret` variable rather than duplicated at each return.
 *
 * ONE LEVER LANDED, 187 differing to 160: THE TWO MESSAGE IDS ARE ONE VARIABLE.
 * The ROM has `ldr r5, =0xc05` once and `add r5, #1` between the two
 * _Func_801e7c0 calls; writing the literals 0xc05 and 0xc06 gives two pool loads
 * and two pool words. `id = 0xc05; ... ; id++; ...` is the fix -- the same
 * one-pseudo shape rom_a7380_c_b.c records, arriving here without needing its
 * cse-associative escape because the base is a plain literal used twice rather
 * than an offset of an offset.
 *
 * THE RESIDUE IS A THREE-WAY REGISTER ROTATION AND IT IS global_alloc AGAIN.
 *
 *     ROM:   base r7   sel r8   n r9   off r10  list r11   id r5   w r6
 *     ours:  sel  r7   off r8   n r9   base r10 list r11
 *
 * With `base` in r10 every `[r7, rN]` register-offset access in the ROM becomes a
 * `mov r3, r10` plus an absolute pointer add in ours. The -da .17.lreg dump gives
 * the priorities and the printed allocation order
 * ";; 14 regs to allocate: 174 39 139 116 80 34 38 172 32 35 33 37 36 40" is
 * reproduced exactly by global.c's
 * pri = floor_log2(n_refs) * n_refs / live_length:
 *
 *     34 = sel   n_refs 35  live 136   1.287
 *     38 = off   n_refs 19  live  80   0.950
 *     32 = base  n_refs 27  live 143   0.755
 *     35 = n     n_refs 16  live 127   0.504
 *     33 = list  n_refs 13  live  99   0.394
 *
 * The ROM's order requires base ABOVE sel and off BELOW n, so `sel` in this C is
 * referenced far more than in the original -- 35 weighted refs against roughly 16
 * readable in the ROM -- and `off` far more. That is a counting statement, not a
 * spelling one, so the search is for a shape in which `sel * 2` is computed once
 * rather than in both arms of the dirty test. All four declaration orders and an
 * `off`-inline variant are INERT (212/160, 211/169), as expected: declaration
 * order is a local-alloc and slot-assignment lever, and this is global_alloc,
 * which ties on allocno number only after the priority compare.
 *
 * Settled along the way: 0x1e and 0x1a pool because they are HImode literal
 * stores (rom_a7380_a_b.c's movhi note), the `sub r3,#2` clear loop's base is
 * top - 2*(count-1) = base+0x144 for 8 halfwords ending at base+0x152, gcc builds
 * `sel * 0x18` as `(sel*2 + sel) << 3` by reusing the index scale, and the two
 * `ldrh` of the same halfword in the tail are a genuine reload across the
 * intervening `str r3, [r7, #8]`, so nothing is needed to keep them.
 */
extern unsigned char *iwram_3001f2c;
extern volatile unsigned int gKeyPress;
extern volatile unsigned int gKeyRepeat;
extern int _GetUnit(int id);
extern void _Func_8016498(unsigned int win);
extern void _Func_801e7c0(int id, unsigned int win, int x, int y);
extern void Func_80a8088(int id, int mode);
extern void Func_80a1804(unsigned char *p, int id);
extern int Func_80a68ec(int unit, unsigned char *list, int c);
extern void Func_80a68a8(unsigned char *list);
extern void Func_80a9b94(int a, int b, int c);
extern void Func_80a3d24(unsigned char *list);
extern void Func_80a1a40(int a, int b);
extern void _PlaySound(int id);
extern void WaitFrames(int n);

int Func_80a7d68(void)
{
    unsigned char *p;
    unsigned char *list;
    int sel;
    int n;
    int dirty;
    int mode;
    int off;
    int i;
    int ret;
    int id;

    p = iwram_3001f2c;
    sel = *(signed char *)(p + 0x1c);
    n = *(signed char *)(p + 0x1e);
    dirty = 1;
    mode = *(unsigned short *)(p + (0x88 << 2));
    off = sel * 2;
    _GetUnit(*(unsigned short *)(p + (0x82 << 2) + off));
    _Func_8016498(*(unsigned int *)(p + (0x86 << 1)));
    id = 0xc05;
    _Func_801e7c0(id, *(unsigned int *)(p + (0x86 << 1)), 0, 0);
    id++;
    _Func_801e7c0(id, *(unsigned int *)(p + (0x86 << 1)), 0, 0x10);
    list = p + (0xe4 << 1);
    for (;;) {
        if (dirty != 0) {
            dirty = 0;
            sel = (sel + n) % n;
            off = sel * 2;
            _GetUnit(*(unsigned short *)(p + (0x82 << 2) + off));
            mode = (mode + 3) % 3;
            Func_80a8088(*(unsigned short *)(p + (0x82 << 2) + off), mode);
            Func_80a1804(p, *(unsigned short *)(p + (0x82 << 2) + off));
            for (i = 7; i >= 0; i--)
                *(unsigned short *)(p + (0xa2 << 1) + i * 2) = 0x1e;
            *(unsigned short *)(p + (0xa2 << 1) + off) = 0x1a;
            p[0x86 << 2] = Func_80a68ec(_GetUnit(*(unsigned short *)(p + (0x82 << 2) + off)), list, 0);
            Func_80a68a8(list);
            Func_80a9b94(0x60, 0x60, 8);
            Func_80a3d24(list);
        } else {
            off = sel * 2;
        }
        Func_80a1a40(off * 8 + sel * 8 - 0xa, 0x10);
        WaitFrames(1);
        if ((gKeyPress & 1) != 0) {
            _PlaySound(0x70);
            ret = 1;
            break;
        }
        if ((gKeyPress & 2) != 0) {
            _PlaySound(0x71);
            ret = -1;
            break;
        }
        if ((gKeyRepeat & 0x20) != 0) {
            _PlaySound(0x6f);
            if (n > 1) {
                sel--;
                dirty = 1;
            }
        }
        if ((gKeyRepeat & 0x10) != 0) {
            _PlaySound(0x6f);
            if (n > 1) {
                sel++;
                dirty = 1;
            }
        }
    }
    p[0x1c] = sel;
    *(int *)(p + 8) = *(unsigned short *)(p + (0x82 << 2) + off);
    p[0x21a] = *(unsigned short *)(p + (0x82 << 2) + off);
    return ret;
}
