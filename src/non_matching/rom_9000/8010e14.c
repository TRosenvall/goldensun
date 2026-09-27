/* Func_8010e14 -- NON-MATCHING, 136 encodings of 201.  NO SIZE LINE -- the object is
 * byte-for-byte the same LENGTH -- and the instruction count is EXACT (201 = 201).  Same 23
 * relocations, same order, same symbols; offsets 0x96..0x102 shifted by 2 and re-aligned at
 * 0x16a.
 *
 * Verify with:
 *   python3 tools/objcmp.py src/non_matching/rom_9000/8010e14.c \
 *     asm/rom_9000/rom_108e4_c.s --func Func_8010e14
 *
 * EVERY BLOCK MATCHES LINE FOR LINE: the two signed /0x200000 biases, the five
 * GetFile/DecompressLZ/DMA3_COPY groups, DMA3_FILL, the 20x15 ramp loop, prologue and
 * epilogue.  What is left is REGISTER NAMING for one triple -- ROM desc->r6, pal->r5
 * (shared with the four ewram destinations), c0->r8; ours desc->r5, pal->r8, c0->r6.
 *
 * LANDING NEEDS A TEXT/DATA SPLIT AND THE SHAPE IS KNOWN AND ALREADY USED IN THIS BANK.
 * datacheck.py reports one `.rodata` with one exported label, and it sits AFTER every
 * function -- `.L132cc: .incrom 0x132cc, 0x132fc` (48 bytes) at line 1493, referenced from
 * exactly ONE place, line 660 inside this function.  asm/rom_9000/rom_b798_c_c_c_c.s is
 * already a pure-rodata object with zero functions consumed via
 * `extern unsigned char L12f20[] __asm__(".L12f20")`, and three more such objects exist in
 * the bank.  stage1.ld:285 already carries `asm/rom_9000/rom_108e4.o(.rodata)`, so the
 * repoint is a one-line edit plus a `.global` on the new blob-only .s.  This candidate
 * already declares the blob `extern`, so objcmp's size line is not inflated.
 *
 * THE FINDING MOST WORTH CARRYING: A HImode CONSTANT STORE CANNOT BE CHAINED BY
 * `reload_cse_move2add`, BECAUSE RELOAD REWRITES IT AS
 * `(set (subreg:SI (reg:HI)) (const_int))` AND move2add SKIPS A SUBREG DESTINATION -- so
 * gcc falls back to the literal pool (`ldr r3,=0x10a`, and odd values like 0x10b/0x11b are
 * pool-only).  ROUTING THE VALUE THROUGH AN `int` makes it a plain SImode
 * `(set (reg) (const_int))`, move2add fires, and the ROM's whole chain
 * `mov #0xa6 / lsl #1 / sub #0x42 / add #0x44 / sub #0x43 ...` falls out.
 * 191 insns -> 185, 149 differing -> 139, IN ONE EDIT.
 * Rejected variants: `volatile int tv` (188/153); the store via a `u16*` (191/149, inert);
 * `short` for `unsigned short` (191/149, inert).
 *
 * Also load-bearing, each by its own drop: `(w << 1) >> 25` LITERALLY rather than
 * `(w >> 24) & 0x7f` (which costs a `mov #0x7f` plus `and`); and a BLOCK-SCOPED
 * `unsigned char *dst = ewram_20xx000;` IMMEDIATELY BEFORE each
 * `DecompressLZ(GetFile(desc[n]), dst)` -- the argument-precompute lever, four places,
 * 4 instructions.  Left as a bare symbol argument gcc materialises the pool load AFTER the
 * call; named, it lands before it, as the ROM has it.
 *
 * BLOCKER: REG_ALLOC_ORDER / colouring choice for the desc-pal-c0 triple.
 * PASS, and it is a strong measured negative: 60 PERMUTATIONS OF THE 11-LOCAL DECLARATION
 * BLOCK ARE ALL IDENTICAL at 149 differing / 191 insns -- COMPLETELY INERT.  Pinning
 * desc->r6 and pal->r5 is WORSE (185/142); pinning all three worse still (184/154), the
 * "two competing pins is worse than one" rule holding.  Also inert: three spellings of the
 * ewram_2020000 cell load, and removing the `cx` local.
 */
#include "dma.h"

extern unsigned char *iwram_3001e70;
extern unsigned char L132cc[] __asm__(".L132cc");
extern unsigned char ewram_2020000[];
extern unsigned char ewram_2028000[];
extern unsigned char ewram_2038000[];
extern unsigned char ewram_203a000[];
extern unsigned char ewram_203c000[];
extern unsigned char ewram_203e000[];
extern void *Func_8004938(unsigned int size);
extern void *GetFile(int index);
extern void DecompressLZ(void *src, void *dst);
extern void DecompressLZ1(void *src, void *dst);
extern void free(void *p);
extern void Func_80113e4(void);

void Func_8010e14(int x, int z)
{
    unsigned char *state;
    unsigned char *buf;
    unsigned int *desc;
    short *pal;
    unsigned int *d;
    unsigned int t;
    int group;
    short c0;
    int cx;
    int i;
    int j;

    group = 0;
    buf = Func_8004938(0x80 << 2);
    state = iwram_3001e70;
    cx = (x / 0x200000) & 0x1f;
    if (((((unsigned int *)ewram_2020000)[cx + (((z / 0x200000) & 0x1f) << 5)] << 1) >> 25) == 0x15)
        group = 1;
    desc = (unsigned int *)(L132cc + group * 0x18);
    pal = (short *)0x5000000;
    *(unsigned int **)(state + 0x11c) = desc;
    c0 = *pal;
    DecompressLZ1(GetFile(desc[0]), buf);
    *(short *)buf = c0;
    DMA3_COPY(buf, pal, 0x1c0);

    {
        unsigned char *dst = ewram_2038000;
        DecompressLZ(GetFile(desc[1]), dst);
        DMA3_COPY(dst, (void *)0x6008000, 0x2000);
    }
    {
        unsigned char *dst = ewram_203a000;
        DecompressLZ(GetFile(desc[2]), dst);
        DMA3_COPY(dst, (void *)0x600a000, 0x2000);
    }
    {
        unsigned char *dst = ewram_203c000;
        DecompressLZ(GetFile(desc[3]), dst);
        DMA3_COPY(dst, (void *)0x600c000, 0x2000);
    }
    {
        unsigned char *dst = ewram_203e000;
        DecompressLZ(GetFile(desc[4]), dst);
        DMA3_COPY(dst, (void *)0x600e000, 0x2000);
    }
    DecompressLZ(GetFile(desc[5]), ewram_2028000);

    DMA3_FILL((void *)0x6002800, 0xf07ff07f, 0x600);

    d = (unsigned int *)0x6003000;
    t = 0x1a901a8;
    for (i = 0; i <= 0x13; i++) {
        for (j = 0xe; j >= 0; j--) {
            *d++ = t;
            t += 0x20002;
        }
        d++;
    }

    if (group == 1) {
        int tv;
        tv = 0x10a; *(unsigned short *)(state + 0x14c) = tv;
        tv = 0x10b; *(unsigned short *)(state + 0x14e) = tv;
        tv = 0x10c; *(unsigned short *)(state + 0x150) = tv;
        tv = 0x11a; *(unsigned short *)(state + 0x16c) = tv;
        tv = 0x11b; *(unsigned short *)(state + 0x16e) = tv;
        tv = 0x11c; *(unsigned short *)(state + 0x170) = tv;
        Func_80113e4();
    }
    free(buf);
}
