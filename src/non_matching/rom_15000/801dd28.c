/* Func_801dd28 -- NON-MATCHING, 140 encodings of 148.  SIZE EXACT (308 bytes both), 149
 * instructions against 148.
 *
 * Verify with:
 *   python3 tools/objcmp.py src/non_matching/rom_15000/801dd28.c \
 *     asm/rom_15000/rom_1ca1c_c_c_c.s --func Func_801dd28
 *
 * ITS .s NEEDS A TEXT/DATA SPLIT AND THE SHAPE IS KNOWN.  datacheck.py, verbatim:
 *     data sections : .rodata
 *     functions     : Func_801d9d4, StartMenu_Main, Func_801dd28
 *     EXPORTS       : .L367c9, .L367cc, .L367ce, .L367d0, .L367d6, .L36750
 * CORRECTION (batch 290): SIX exported labels is WRONG -- THE SPLIT NEEDS SEVEN.
 * `.L367dc` is defined in that same `.rodata` at line 586 with NO `.global`, and it is
 * referenced by `ldr r3, =.L367dc` from BOTH Func_801d9d4 (line 159) AND StartMenu_Main
 * (line 276).  It resolves today only because it is file-local; the moment EITHER of those
 * two functions leaves this .s, the data object must carry `.global .L367dc` or the link
 * fails.  datacheck.py's EXPORTS line lists only labels that are ALREADY .global, so it
 * cannot see this -- read the .rodata for label definitions too, not just its .global lines.
 * (split_s.py would refuse and name it, which is the backstop; this note is so the next
 * reader does not have to discover it that way.)
 *
 * The data sits ENTIRELY BELOW all three functions (functions to
 * line 549, then .align 2,0 and a .word 0xf000, then .section .rodata at 566 with seven
 * .incrom ranges spanning 0x36750-0x367e4).  So it is a CLEAN TAIL CUT with no data
 * interleaved between functions -- but matching all three still needs the rehome.
 *
 * BLOCKER: register allocation, a PURE PERMUTATION AT EQUAL COUNT.  Both sides use ten
 * registers (r4-r12, r14) and the ROM's assignment is rotated one role relative to this.
 * The one extra instruction is the buffer address: `add r1,sp,#4` against this candidate's
 * `mov r3,#4 / add r3,sp`.
 *
 * Uses DMA3_COPY from the tree's own include/dma.h, which contains inline asm; that is the
 * tree's own macro and not scaffolding.  No pins, no shims.
 */
/* Func_801dd28 -- NON-MATCHING, 140 encodings of 148.  SIZE EXACT (308 bytes
 * both).  149 instructions against 148 -- ONE over.
 *
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py <this> \
 *     asm/rom_15000/rom_1ca1c_c_c_c.s --func Func_801dd28
 *
 * USE objcmp, NOT tryc: this reference keeps its literal pool INSIDE the
 * function body (the 0xf000 word at .L1de38, jumped over by the `b`), which is
 * exactly the case tryc's own `!!` warning marks as unreliable.
 *
 * THE ONLY asm HERE IS DMA3_COPY FROM include/dma.h -- the tree's own header,
 * not a shim.  `stmia r3!, {r0,r1,r2} / sub r3,#0xc` with
 * `0x84000000 | size/4` = 0x84000008 gives the ROM's tail exactly, size 32.
 *
 * WHAT THE FUNCTION DOES.  Unpacks the 32 bytes of VRAM tile `*(u8 *)a` at
 * 0x6000000 into 64 nibbles in a 128-byte stack buffer; overlays the nibbles of
 * tile `idx` of _FILE_13 through the byte table `map`, skipping zero entries;
 * repacks the 64 nibbles into 32 bytes in place; and when the sign bit of
 * `*(u8 *)a` is CLEAR, allocates a scratch tile by walking the counter at
 * iwram_3001e8c+0xea0 modulo 0x80 until the +0xda0 in-use byte is zero, marks
 * it, writes `slot | 0x80 | 0xf000` back through both `a` and `b`, and retargets
 * the DMA at the new tile.
 *
 * TWO LEVERS LANDED, and both are the halfword-constant exception from
 * const.sym showing up twice in one function.
 *
 * 1. `unsigned int` FOR THE PACKED BYTE.  `c >> 4` on a signed `int` emits
 *    `asr`; the ROM has `lsr`.  Two sites.
 *
 * 2. A NAMED `int` TEMPORARY FOR EVERY VALUE THAT IS STORED AS A HALFWORD.
 *    Writing `*(u16 *)(p + 0xea0) = (t + 1) & 0x7f;` makes 0x7f an operand of an
 *    HImode expression and gcc POOLS it -- `ldr r6,=0x7f` where the ROM has
 *    `mov r6,#0x7f`.  Worse, `*a = k | 0xf000;` pools the SIGN-EXTENDED form,
 *    `ldr r2,=0xfffff000` against the ROM's `ldr r2,=0xf000`.  Routing both
 *    through `int u;` fixes both and also removes a spurious `b`/label pair that
 *    cross-jumping created from the duplicated `k | 0xf000` at the two stores.
 *    SIZE 316 -> 308 (EXACT) and 151 -> 149 instructions.
 *
 * BLOCKER: REGISTER ALLOCATION -- A PURE PERMUTATION, SAME COUNT.  Both use ten
 * registers (r4-r12, r14).  The ROM assigns v->r5, map->r6, off->r7, p->r8,
 * idx->r9, file->r10 (reused for the 0xf mask inside loop 2), a->r11,
 * w->r12, buffer base->r14.  We assign map->r5, off->r6, p->r7, base->r8,
 * v->r9, idx->r10, a->r11, w->r12, file->r14 (also reused for 0xf).  Every one
 * of the 140 differences is downstream of that rotation, including the
 * two-operand `add r1,r8` the ROM can use for `p + 0xea0` where our low-register
 * `p` folds into a three-operand `add r0,r7,r1`.
 *
 * THE ONE EXTRA INSTRUCTION IS THE BUFFER ADDRESS: ROM `add r1,sp,#4 / mov
 * r12,r1 / mov r14,r12`, ours `mov r3,#4 / add r3,sp / mov r12,r3 / mov r8,r12`.
 *
 * DROP LADDER:
 *   first candidate                          134 of 151, size 316 vs 308
 *   `unsigned int c`                         inert on the counts, fixes two asr
 *   named `int u` for the halfword stores     140 of 149, SIZE EXACT
 *   declaration order, 16 permutations        ALL INERT
 *   a named `base = tmp;` pointer             139 instructions / 288 bytes --
 *                                             TOO SHORT, it deletes the r12/r14
 *                                             juggling the ROM pays for
 *   `&tmp[0]` at the DMA / `u8 tmp[0x80]`     inert
 */
#include "gba/types.h"
#include "gba/io.h"
#include "dma.h"

extern u8 *iwram_3001e8c;
extern int _FILE_13;
extern u8 *GetFile(int id);

void Func_801dd28(u16 *a, u16 *b, int idx, u8 *map)
{
    u8 tmp[128];
    u8 *p;
    u8 *file;
    u8 *s;
    u8 *w;
    u8 *rd;
    unsigned int i;
    unsigned int n;
    int off;
    unsigned int c;
    int d;
    int t;
    int u;
    int k;
    u8 v;

    p = iwram_3001e8c;
    file = GetFile((int)&_FILE_13);
    v = *(u8 *)a;
    off = v * 32;
    s = (u8 *)0x6000000 + off;
    w = tmp;
    for (i = 0; i <= 0x1f; i++) {
        c = *s++;
        w[0] = c & 0xf;
        w[1] = c >> 4;
        w += 2;
    }
    s = file + idx * 32;
    w = tmp;
    for (i = 0; i <= 0x1f; i++) {
        c = *s++;
        d = map[c & 0xf];
        if (d != 0)
            *w = d;
        w++;
        d = map[c >> 4];
        if (d != 0)
            *w = d;
        w++;
    }
    w = tmp;
    rd = tmp;
    for (i = 0; i <= 0x1f; i++) {
        *w = rd[0] | (rd[1] << 4);
        w++;
        rd += 2;
    }
    if ((signed char)v >= 0) {
        n = 0;
        do {
            t = *(u16 *)(p + 0xea0);
            u = (t + 1) & 0x7f;
            *(u16 *)(p + 0xea0) = u;
            k = (u8)t;
            if (p[0xda0 + k] == 0)
                break;
            n++;
        } while (n <= 0x7f);
        p[0xda0 + k] = 1;
        k |= 0x80;
        u = k | 0xf000;
        *a = u;
        *b = u;
        off = k * 32;
    }
    DMA3_COPY(tmp, (u8 *)0x6000000 + off, 32);
}
