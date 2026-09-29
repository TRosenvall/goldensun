/* Func_80a7478 -- RunStatusScreen, 0x080a7478, 222 ROM instructions.
 * NON-MATCHING, 178 of 243 encodings differ.
 * SIZE EXACT (600 = 600) and INSTRUCTION COUNT EXACT (243 = 243), so 178 is a
 * TRUE DISTANCE -- but most of it is the pool-offset cascade behind ONE defect;
 * the alignment-tolerant view (tools/aligncmp.py) reads 209 aligned-equal of
 * 243, 46 differing in 28 hunks.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/rom_a1000/80a7478.c \
 *     asm/rom_a1000/rom_a7380_a_c_a_a.s --func Func_80a7478
 *
 * THE SPLIT.  asm/rom_a1000/rom_a7380_a_c_a_a.s holds TWO functions
 * (Func_80a7440 at 0x080a7440, Func_80a7478) and NO data section; datacheck is
 * clean, so the split is text-only and needs NO `.global`.  Func_80a7440 stays
 * in asm/.
 *
 * SHIMS: 4 register pins, all inside the local DMA3_COPY16_RW helper -- the
 * fakematch class.  The helper is VERBATIM from the landed
 * src/rom_a1000/rom_a1814_c_a_a_c_a_c_a_c_c_b.c, whose own note says "Promote it
 * if a second function needs it".  THIS IS THAT SECOND FUNCTION, so the better
 * landing is to move DMA3_COPY16_RW into include/dma.h (the pins then live in the
 * shared header, exactly as they already do for the other six helpers, and this
 * file becomes pin-free).  If it stays local it needs a fakematch.txt row.
 * Do NOT reach for `"r0","r2"` clobbers instead: that names INPUT registers as
 * clobbered, which the tree has twice declined as undefined.  Measured here:
 * the `"+l"` form and the illegal clobber form score IDENTICALLY (178/243, 209
 * aligned), so there is no reason to prefer the illegal one.
 *
 * WHAT THE HELPER BUYS.  With the shared DMA3_COPY16, gcc believes the `stmia`
 * leaves r0 and r2 intact, so reload_cse_move2add derives the later transfers'
 * source and count (`sub r0,#56 / sub r2,#15`) where the ROM reloads both from
 * the pool.  `"+l"` on src and cnt loses those values and the pool loads come
 * back; the DESTINATION stays a plain input and keeps being derived
 * (`add r1,#0x1c`), which is what the ROM has.  186 of 242 -> 178 of 243, and it
 * is what fixed the instruction count.
 *
 * ============================================================
 * THE BLOCKER: THE ALLOCATION RACE FOR r5 BETWEEN A FUNCTION POINTER AND A
 * REMATERIALISABLE CONSTANT.  Pass: local-alloc / global.c, not any expression
 * shape.
 *
 * The ROM's callee-saved roles are r5 = the Func_8001af8 pointer (twice, two
 * non-overlapping live ranges), r6 = 0x2000, r7 = p, r8 = a zero, r9 = the 0x40
 * scratch, r10 = the return value, r11 = the 0x2000 scratch -- and NEITHER
 * 0x5000000 NOR 0x6004000 gets a register: both are rematerialised at every use
 * (`mov r1,#0xa0 / lsl r1,#19` three times, `ldr rN,=0x6004000` three times).
 *
 * Ours gives r5 to the 0x5000000 pseudo and pushes the first function pointer to
 * r8, so the two early indirect calls read `bl _call_via_r8` against the ROM's
 * `bl _call_via_r5`, and the two extra instructions that costs shift the whole
 * mid-function pool four bytes -- which is the 178 minus the ~24 real encodings.
 *
 * THE CAUSE IS READ, NOT GUESSED.  cse commons the constant at the copy call's
 * argument with the same constant at the DMA block's destination: the .17.lreg
 * dump shows `(insn 111 (set (reg:SI 61) (const_int 83886080)))` with a
 * REG_EQUIV and exactly two uses, insn 115 (the argument) and insn 136 (the
 * asm's r1).  local-alloc's qty_compare ranks on
 * (floor_log2(n_refs)*n_refs - n_calls_crossed) / length; that pseudo scores
 * 1/8 against the function pointer's 1/30, so it wins r5.  There is no basic
 * block between the two uses -- the only thing separating them is
 * `bl Func_80a2144`, and a call does not end a cse block -- so nothing
 * source-level keeps them apart.
 *
 * MEASURED AND INERT / WORSE (aligned-equal of 243; do not re-run):
 *   this file (int one/z1 locals, bare HImode 0x1e and 0x80) ........ 209
 *   + the last (*g)[ofs] store through a temp pointer (variant x3) ... 215 but
 *     245 encodings / 604 bytes -- structurally closer, wrong length
 *   `d` (the DMA destination) assigned as the FIRST statement ........ 197
 *   0x6004000 also named at the top ................................. 193
 *   four explicit constant DMA destinations, no carried pointer ...... 197
 *   the same, plus the 0x5000000 named early ........................ 197
 *   a pinned `register unsigned p1 __asm__("r1")` for the copy arg ... 197 (245)
 *   a pinned r1 for the DMA pointer ................................. 197 (245)
 *   two shifted int locals (`s = 0xa0; s <<= 19;`) for the two sites . 209 IDENTICAL
 *   -fno-rerun-cse-after-loop (CSE_CFLAGS exists for 10 files) ....... 180 (246) WORSE
 *   -fno-gcse ....................................................... 209 IDENTICAL
 *   -fno-schedule-insns2 ............................................ 177 WORSE
 *
 * This is the SAME blocker as the sibling park src/non_matching/rom_a1000/80a24d0.c
 * ("the ROM and this candidate disagree about WHICH THREE constants get
 * callee-saved registers"), on the same screen scaffold, and its conclusion
 * holds here: update_equiv_regs collapses a named constant back into its
 * REG_EQUIV before allocation, so naming is inert in both directions.
 *
 * LOAD-BEARING FORM, all first-try or measured:
 *  - `q = iwram_3001e68[0]; one = 1; *(short *)(q + 4) = one;` -- the POINTER
 *    named first, then the constant.  Reversed it is an r2/r3 swap (the recipe
 *    from src/rom_a1000/rom_a7380_a_b.c).  Worth 3 encodings.
 *  - `int z1 = 0;` as an INITIALISER (not an assignment) for the first
 *    `*(short *)(p + 0x220) = 0`: the definition lands at function entry, the
 *    live range lengthens, and the zero takes the ROM's r5.  202 -> 209.
 *  - the second and third zeros left as BARE literals: cse commons them into
 *    one pseudo which takes r8, exactly as the ROM does.
 *  - 0x1e and 0x80 left as BARE HImode literals -- they pool as words, which is
 *    the ROM's `ldr r1, .La75a0 @ 0x1e`.  An `int` local gives `mov r1,#30`.
 *  - `g = &iwram_3001e8c;` with `g[-9]` for the iwram_3001e68 read: the ROM's
 *    `ldr r5,=iwram_3001e8c / mov r3,r5 / sub r3,#0x24 / ldr r3,[r3]`, right
 *    first try, and it keeps the relocation SYMBOL the ROM has (the array
 *    reading `iwram_3001e68[9]` would relocate against the other name).
 *  - `ofs = 0xea6;` named, per the sibling park.
 *  - three SEPARATE function-pointer locals (copy / fill / copy2).  One shared
 *    local puts the pointer in r11 and emits `bl _call_via_fp`.
 *
 * NEXT, if reopened: the only handle left is to make the 0x5000000 pseudo LOSE
 * r5 without lengthening the function-pointer's range -- i.e. raise
 * n_calls_crossed for the constant or lower n_refs, neither of which any of the
 * nine spellings above reaches.  A `-ffixed-r5`-style flag does not exist in
 * this tree's groups.  Do not re-measure the inert list.
 */
#include "dma.h"

/* DMA3_COPY16 without the promise that the transfer preserves src and count.
 * Verbatim from src/rom_a1000/rom_a1814_c_a_a_c_a_c_a_c_c_b.c, whose note says
 * "Promote it if a second function needs it" -- this is that second function. */
/* DMA3_COPY16_RW now comes from include/dma.h -- PROMOTED in batch 299 on the
 * standing note left with it, because this function is the second one needing the
 * form that does not promise the transfer preserves src and count.  Without it,
 * reload_cse_move2add derives the later transfers from the preserved operands.
 * The legal "+l" form and an illegal "r0","r2"-clobber form measure IDENTICALLY
 * (178 of 243), so there is no reason to prefer the illegal one. */

extern unsigned char *iwram_3001e68[];
extern unsigned char *iwram_3001e8c;

extern unsigned char *galloc_iwram(int tag, int size);
extern void *Func_8004970(int size);
extern void _Func_80170f8(int a, int b, int c, int d);
extern void WaitFrames(int n);
extern void Func_80a1070(void);
extern void Func_80a1090(int a);
extern int _Func_80796c4(unsigned char *buf);
extern void Func_80a8034(int a, int b, int c, int d);
extern void Func_8001af8(void *dst, void *src, int len);
extern void Func_80a2144(int a);
extern void Func_80008d8(void *dst, int len, int val);
extern void _Func_801e3c8(int a);
extern int _CreateUIBox(int a, int b, int c, int d, int e);
extern int _GetNumDjinn(int who);
extern void Func_80ad274(int win, int a);
extern void _Func_80219c8(int addr);
extern void Func_80a2474(void);
extern int Func_80a76d0(void);
extern void Func_80a2490(void);
extern void _Func_80164ac(int a);
extern void Func_80ad318(void);
extern void Func_80a1050(void);
extern void _Func_801e318(void);
extern void free(void *p);
extern void Func_80a34c0(void);
extern void gfree(int tag);
extern void _ClearUIRegion(int a, int b, int c, int d);

int Func_80a7478(void)
{
    void (*copy)(void *, void *, int);
    void (*copy2)(void *, void *, int);
    void (*fill)(void *, int, int);
    unsigned char *p;
    void *pal;
    void *tiles;
    unsigned char **g;
    unsigned char *q;
    unsigned short *d;
    int ret;
    int ofs;
    int i;
    int one;
    int z1 = 0;

    p = galloc_iwram(0x37, 0xa7 << 4);
    pal = Func_8004970(0x40);
    tiles = Func_8004970(0x80 << 6);
    q = iwram_3001e68[0];
    one = 1;
    *(short *)(q + 4) = one;
    _Func_80170f8(0, 0, 0x1e, 0x14);
    WaitFrames(1);
    Func_80a1070();
    Func_80a1090(0);
    *(short *)(p + (0x88 << 2)) = z1;
    *(unsigned char *)(p + 0x219) = _Func_80796c4(p + (0x82 << 2));
    Func_80a8034(0, 3, 0, 7);
    copy = Func_8001af8;
    copy(pal, (void *)(0xa0 << 19), 0x40);
    Func_80a2144(0xe);
    d = (unsigned short *)(0xa0 << 19);
    DMA3_COPY16_RW((void *)0x5000200, d, 0x40);
    d += 0xe;
    DMA3_COPY16_RW((void *)0x50001c8, d, 4);
    d += 2;
    DMA3_COPY16_RW((void *)0x5000200, d, 0x40);
    d += 0xe;
    DMA3_COPY16_RW((void *)0x50001e8, d, 4);
    copy(tiles, (void *)0x6004000, 0x80 << 6);
    fill = Func_80008d8;
    fill((void *)0x6004000, 0x80 << 6, 0x33333333);
    _Func_801e3c8(1);
    *(int *)(p + (0x86 << 1)) = _CreateUIBox(0xd, 0, 0x11, 5, 2);
    for (i = 7; i >= 0; i--)
        *(short *)(p + 0x144 + i * 2) = 0x1e;
    if (_GetNumDjinn(-1) != 0)
        Func_80ad274(*(int *)(p + (0x86 << 1)), 0);
    for (i = 0; i < 4; i++) {
        *(short *)(p + (0x8d << 2) + i * 2) = 0x82 + i * 0x20;
        *(short *)(p + (0x8d << 2) + 8 + i * 2) = 0x80;
    }
    _Func_80219c8(0x6002500);
    Func_80a2474();
    *(short *)(p + (0x88 << 2)) = 0;
    ret = Func_80a76d0();
    Func_80a2490();
    _Func_80164ac(*(int *)(p + 0x24));
    Func_80ad318();
    Func_80a1050();
    _Func_80170f8(0, 0, 0x1e, 0x14);
    WaitFrames(1);
    _Func_801e318();
    _Func_801e3c8(0);
    WaitFrames(1);
    copy2 = Func_8001af8;
    copy2((void *)(0xa0 << 19), pal, 0x40);
    copy2((void *)0x6004000, tiles, 0x80 << 6);
    free(tiles);
    free(pal);
    g = &iwram_3001e8c;
    ofs = 0xea6;
    (*g)[ofs] = 1;
    Func_80a34c0();
    _Func_80170f8(0, 0, 0x1e, 0x14);
    gfree(0x37);
    *(short *)(g[-9] + 4) = 0;
    WaitFrames(1);
    _ClearUIRegion(0, 0, 0x1e, 0x14);
    (*g)[ofs] = 0;
    return ret;
}
