/* LoadMapData -- 0x0800fb38, asm/rom_9000/rom_f9cc_a_c_c.s
 *
 * NON-MATCHING, 301 of 377 encodings differ.
 *
 * SIZE  ref 868 bytes, ours 876   (ours is 8 bytes / 4 instructions LONG)
 * COUNT ref 377 encodings, ours 380.  The reference's own instruction count is
 *       345; objcmp's 377 counts the pool words too.  The counts are NOT equal so
 *       objcmp's 301 is SATURATED -- use tools/aligncmp.py: this candidate reads
 *       aligned-equal 285 (75.6% of ref), 124 in 51 hunks.  That is the best of
 *       the three functions in this batch.
 * RELOCATIONS: THE SEQUENCE IS EXACT.  43 relocations, same symbols, same order,
 *       and the only name difference is `_call_via_sl` against the ROM's
 *       `_call_via_r10` at all four sites -- THE KNOWN ALIAS.  src/lib/call_via.s
 *       defines both at one label (batch 234 established this and objcmp carries
 *       the note at tools/objcmp.py:63); objcmp only resolves it from a linked
 *       ELF, so screening an unbuilt tree prints `XX RELOCATIONS differ` for a
 *       pair that IS one symbol.  Read the sequence, not the flag.
 *
 * SHIMS: 2 register pins, both inside one `static inline call_via` (see below).
 * That is the tree's ESTABLISHED spelling and it is a booked fakematch class --
 * src/rom_c9000/rom_e3958_c_c_c_a.c carries the same shim and has a
 * fakematch.txt row (line 543).  IF THIS LANDS IT NEEDS A ROW TOO.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_9000/LoadMapData.c \
 *     asm/rom_9000/rom_f9cc_a_c_c_c.s --func LoadMapData
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/aligncmp.py \
 *     src/non_matching/rom_9000/LoadMapData.c \
 *     asm/rom_9000/rom_f9cc_a_c_c.s LoadMapData -v
 *
 * SPLIT SHAPE.  tools/datacheck.py is SILENT on rom_f9cc_a_c_c.s -- no data
 * section.  The .s holds three functions and LoadMapData is the LAST, so this is
 * a clean tail cut:
 *     asm/rom_9000/rom_f9cc_a_c_c_a.s   Func_800fa8c, UnpackTilemap
 *     src/rom_9000/rom_f9cc_a_c_c_b.c   LoadMapData
 * stage1.ld line 217 becomes those two.  NO exports.s change and NO new
 * `.global`: `.thumb_func_start` already globalises UnpackTilemap, which this
 * function calls across the new boundary, and `.L13784` -- the map-archive table
 * this function indexes -- is ALREADY `.global` at
 * asm/rom_9000/rom_f9cc_c.s:697.  (`.export_func LoadMapData` at
 * src/rom_9000/exports.s:40 is a `_LoadMapData` stub and is indifferent.)
 *
 * THE REFERENCE'S PROSE IS GOOD.  Unusually so: the header names the archive as
 * `.L13784 + index * 0xC`, the `+0x128` resource base, every sub-resource offset
 * (+0x24 / +0x28 / +0x2C / +0x30 / +0x34 / +0x38) and its destination, the
 * `<< 19` bounds at +0xEC..+0xF8 and the three flag bytes at +0x100..+0x102, and
 * all of it checks out against the instructions.  Two nits, neither a lead
 * failure: the symbol is LoadMapData and the prose heading says "LoadMap", and it
 * abbreviates the EWRAM symbols (`ewram_10001` for ewram_2010001, `ewram_2c000`
 * for ewram_202c000).  It stops before the second half -- the three BGxCNT
 * composes, the 0x170 flag gate and the four 0x4000-byte tile uploads -- but it
 * does not claim otherwise.
 *
 * THE ONE CONSTRUCT THAT MATTERS: Func_8000888's PRIVATE CALLING CONVENTION.
 * The loop calls it with the hand-written `.call_via r9` macro, which is
 * `.align 2, 0 / mov r12, pc / bx r9` -- six bytes including an alignment
 * `.short 0x0000` INSIDE the function.  That is not the ordinary indirect call
 * (`bl _call_via_rN`, which this same function uses for Func_8001af8), and the
 * reason is in the callee.  Func_8000888 is ARM, at asm/rom_c0/rom_770.s:81:
 *     smull r2, r0, r1, r0 / lsl r0, #16 / orr r0, r2, lsr #16
 *     add r12, #1 / bx r12
 * It returns through r12 with the Thumb bit set by hand and NEVER TOUCHES lr.
 * Two consequences:
 *   * The `.align 2, 0` is load-bearing, not cosmetic: `mov r12, pc` must sit on
 *     a word boundary for pc+4 to be the instruction after the `bx`.
 *   * THE CALLER MAY KEEP A VALUE IN lr ACROSS THE CALL, and this one does --
 *     `mov r14, r3` before and `add r0, r14` after, holding the first sprite
 *     coordinate in lr while the multiply runs.  r14 is position 6 in
 *     REG_ALLOC_ORDER, so gcc will use it.
 * The spelling is the tree's established inline-asm shim: 22 generated .s files
 * already emit `mov r12, pc`, and src/rom_c9000/rom_e3958_c_c_c_a.c is a LANDED
 * file calling THE SAME callee.  ONE DIFFERENCE FROM THAT FILE AND IT IS
 * DELIBERATE: its clobber list names "lr", which is safe there and would forbid
 * exactly the lr use above, so the shim here drops it.  That is the whole reason
 * this function was not simply unreachable: at first reading `.call_via` looks
 * like a construct with no C spelling (0 generated .s files contain the MACRO),
 * and the resolution is that gcc reaches the same two instructions through
 * inline asm, which is what batch 119 recorded when it retracted ".call_via is a
 * hard wall".
 *
 * SIGNEDNESS: the tree's `s8` is `char`, which is UNSIGNED on ARM (see
 * include/gba/types.h:9-16 -- only the m4a TU gets -D M4A_SIGNED_CHAR).  The four
 * per-layer scroll bytes at entry +2..+5 are read `mov r3,#N / ldrsb r3,[r6,r3]`
 * in the ROM, so they must be spelled `signed char` explicitly.  Writing them
 * `s8` compiles to `ldrb` -- a real sign bug that ALSO happens to score better on
 * size (868 bytes exactly, 376 encodings, 71.1%), which is a reminder that a
 * closer size can be a wrong program.  The signed form is correct and costs 4
 * instructions.
 *
 * THE RESIDUE, ITEMISED (all four extra instructions, plus the rotations).
 *   (a) THE 0x194 STATE SIZE IS REMATERIALISED.  The ROM builds it once,
 *       `mov r6,#0xCA / lsl r6,#1`, and feeds r6 to both galloc_ewram and the
 *       Func_80008d4 zero-fill; we emit `movs r3,#148 / lsls r3,#1` twice.
 *       Two instructions.  Naming it in a local (0), writing it `0x194` (0) and
 *       writing it `0xca << 1` (0) are all BYTE-IDENTICAL -- gcc-2.96 propagates
 *       the constant and there is no spelling left.  THIS IS THE SAME CLASS AS
 *       InitWorldMap's 0x284 DMA count; see
 *       src/non_matching/rom_9000/InitWorldMap.c residue (b).  One class, two
 *       functions, and it is now the top item on both.
 *   (b) THE THREE BGxCNT MASKS ARE POOLED AND THE ROM BUILDS THEM.  The ROM has
 *       `movs r3,#0xA0 / lsls r3,#3` (and #0xC0, #0xE0) for 0x500 / 0x600 /
 *       0x700; we load each from the pool.  3 instructions fewer but 3 pool words
 *       more, so it costs 6 bytes.  gcc's own `thumb_shiftable_const` accepts all
 *       three (0x500 & ~(0xFF << 3) == 0), so the split SHOULD fire -- it does
 *       not, which points at the constant insn being created after the last split
 *       pass.  `0x500` written literally, `(0xa0 << 3)`, and reordering the OR
 *       chain are all BYTE-IDENTICAL (0).
 *   (c) ONE MISSING `.short 0x0000`.  The ROM's `.align 2, 0` inside the loop
 *       pads and ours does not, so our loop body sits two bytes off.  This is
 *       downstream of (a)/(b), not independent.
 *   (d) REGISTER ROTATIONS AT EQUAL COUNT, three of them: the +0x30 / +0x34
 *       archive offsets are r0 in the ROM and r3 in ours (and the reverse for
 *       +0x24 / +0x28 / +0x2C, which IS fixed here by naming them in a temp); the
 *       loop's held function pointer and its counter swap r9 and r11
 *       (`mov r9,r3 / mov fp,r2` against our `mov fp,r3 / mov r9,r2`); and
 *       `&st->f101` is r0 in the ROM and r3-then-ip in ours, which costs one
 *       extra `mov ip, r3`.
 *
 * MEASURED, from 71.1% (the first candidate, which had the s8 sign bug):
 *   `signed char` on the four scroll bytes             71.1 -> 74.0%  (+4 insn)
 *   a named temp for the +0x24/+0x28/+0x2C offsets,    74.0 -> 75.6%
 *     and the +0x30/+0x34 tests read inline
 *   a named local for the 0x194 size                   0, byte-identical
 *   `0x500`/`0x600`/`0x700` written literally          0, byte-identical
 *   `&st->f102` taken after its test instead of before 0, byte-identical
 *
 * NEXT.  (a) and (b) together are 5 instructions and 3 pool words and they are
 * the whole size delta.  Both are "gcc-2.96 will not keep a constant where the
 * ROM keeps it", and (a) is shared with InitWorldMap, so ONE finding closes part
 * of two functions.  Look for it in the split passes (.13.combine and earlier),
 * not in the allocator: `thumb_shiftable_const` is a split condition, and a
 * constant introduced after split2 can only go to memory.
 */
#include "gba/types.h"
#include "gba/io.h"

/* Func_8000888 (rom_c0/rom_770.s, ARM, at 0x03000118) is a 32x32->48.16 multiply
 * that returns with `add r12, #1 / bx r12`, so the CALLER passes the return
 * address in r12 and lr is NOT touched.  The tree's established spelling for
 * that convention is this inline-asm shim; see src/rom_c9000/rom_e3958_c_c_c_a.c,
 * which calls the SAME function.  The one difference from that file: the clobber
 * list here must NOT name "lr", because this ROM function keeps a value in lr
 * across the call. */
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
        : "memory", "r12"
    );
    return _a;
}

struct Ent { u8 x; u8 y; signed char dx; signed char dy; signed char dz; signed char dw; u8 w; u8 h; };

struct Rec {
    int f00, f04, f08, f0c, f10, f14, f18, f1c, f20, f24;
    u16 f28, f2a;
    u8 *f2c;
};

struct Arch {
    u8 b[10];
    u8 pad0a[2];
    struct Ent e[3];
    int f24, f28, f2c, f30, f34, f38;
};

struct MapSt {
    u8 pad00[0x10];
    void *f10;
    u16 f14;
    u8 pad16[0xe4 - 0x16];
    int camx;
    int camy;
    int bx, by, bz, bw;
    u8 padfc[0x100 - 0xfc];
    u8 f100, f101, f102, f103;
    struct Rec rec[3];
};

extern u16 L13784[][6] __asm__(".L13784");
extern u8 gBuffer[];
extern u8 ewram_2010001[];
extern u8 ewram_202c000[];
extern u8 ewram_202d000[];
extern u8 ewram_202de00[];
extern u8 ewram_2028000[];

extern void Func_8003bb4(int a);
extern void *galloc_ewram(s32 tag, s32 size);
extern void Func_80008d4(void *p, int n);
extern void *GetFile(int id);
extern void DecompressLZ(void *src, void *dst);
extern void DecompressLZ2(void *src, void *dst);
extern void DecodeMetatileset(void);
extern void UnpackTilemap(void);
extern void Func_80118d8(void *p);
extern void Func_8011a84(void *p);
extern int Func_8000888(int a, int b);
extern int _GetFlag(int f);
extern void _ClearFlag(int f);
extern void *Func_8004938(int n);
extern void Func_8001af8(void *dst, void *src, int n);
extern void free(void *p);
extern void UpdateFieldScreen(void);
extern void StartTask(void (*f)(void), int tag);

int LoadMapData(int index)
{
    struct MapSt *st;
    struct Arch *a;
    struct Ent *e;
    struct Rec *r;
    u16 *arch;
    int *xp;
    int *yp;
    u8 *pf1;
    u8 *pf2;
    u8 *pf3;
    u8 *buf;
    void (*clr)(void *, int);
    void (*cpy)(void *, void *, int);
    int i, t, u, v, px, py, fl, n, id, sz;
    short pal;

    REG_DISPCNT = REG_DISPCNT & 0xc1ff;
    Func_8003bb4(0);
    arch = L13784[index];
    sz = 0xca << 1;
    st = galloc_ewram(8, sz);
    clr = Func_80008d4;
    clr(st, sz);
    a = GetFile(arch[0] + 0x128);
    t = a->f24;
    DecompressLZ((u8 *)a + t, ewram_2010001);
    DecodeMetatileset();
    t = a->f28;
    DecompressLZ((u8 *)a + t, ewram_202c000);
    t = a->f2c;
    DecompressLZ((u8 *)a + t, gBuffer);
    UnpackTilemap();
    if (a->f30 != 0) {
        DecompressLZ((u8 *)a + a->f30, ewram_202d000);
        Func_80118d8(ewram_202d000);
    }
    if (a->f34 != 0) {
        DecompressLZ((u8 *)a + a->f34, ewram_202de00);
        Func_8011a84(ewram_202de00);
    }
    st->f10 = (u8 *)a + a->f38;
    st->bx = a->b[0] << 19;
    st->by = a->b[1] << 19;
    st->bz = a->b[2] << 19;
    st->bw = a->b[3] << 19;
    xp = &st->camx;
    *xp = 0;
    yp = &st->camy;
    *yp = 0;
    st->f100 = a->b[4];
    st->f101 = a->b[5];
    st->f102 = a->b[6];
    r = st->rec;
    e = a->e;
    i = 2;
    do {
        px = e->x << 19;
        r->f08 = px;
        py = e->y << 19;
        r->f0c = py;
        r->f18 = e->dz << 12;
        r->f1c = e->dw << 12;
        r->f28 = e->w;
        r->f2a = e->h;
        r->f20 = 0;
        r->f24 = 0;
        u = e->dx << 12;
        v = e->dy << 12;
        r->f10 = u;
        r->f14 = v;
        r->f2c = gBuffer + ((((e->y >> 1) << 7) + (e->x >> 1)) << 2);
        r->f00 = call_via(Func_8000888, *xp, u) + px;
        r->f04 = call_via(Func_8000888, *yp, v) + py;
        e++;
        r++;
    } while (--i >= 0);
    st->f14 = 0x80 << 5;
    pf1 = &st->f100;
    if (*pf1 != 0)
        st->f14 = 0xc0 << 5;
    pf2 = &st->f101;
    if (*pf2 != 0)
        st->f14 |= 0x400;
    pf3 = &st->f102;
    if (*pf3 != 0)
        st->f14 |= 0x200;
    REG_BG3CNT = *pf1 | (a->b[7] << 2) | (0xa0 << 3);
    REG_BG2CNT = *pf2 | (a->b[8] << 2) | (0xc0 << 3);
    REG_BG1CNT = *pf3 | (a->b[9] << 2) | (0xe0 << 3);
    fl = 0xb8 << 1;
    if (_GetFlag(fl) != 0) {
        _ClearFlag(fl);
    } else {
        n = 0x80 << 7;
        buf = Func_8004938(n);
        if (buf != 0) {
            pal = *(short *)0x5000000;
            id = 0x128;
            DecompressLZ(GetFile(arch[1] + id), buf);
            *(u16 *)buf = pal;
            cpy = Func_8001af8;
            cpy((void *)0x5000000, buf, 0xe0 << 1);
            DecompressLZ2(GetFile(arch[2] + id), buf);
            cpy((void *)0x6004000, buf, n);
            DecompressLZ2(GetFile(arch[3] + id), buf);
            cpy((void *)0x6008000, buf, n);
            DecompressLZ2(GetFile(arch[4] + id), buf);
            cpy((void *)0x600c000, buf, n);
            DecompressLZ2(GetFile(arch[5] + id), ewram_2028000);
            free(buf);
        }
    }
    REG_MOSAIC = 0;
    REG_BLDCNT = 0;
    REG_DISPCNT = 0xa0 << 1;
    StartTask(UpdateFieldScreen, 0xc85);
    return 2;
}
