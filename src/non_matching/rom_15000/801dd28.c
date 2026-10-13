/* Func_801dd28 -- NON-MATCHING.
 * 106 of 148 encodings, DOWN FROM 140 -- batch 348 brief E, on the ANCHOR AXIS,
 * which had never been run on this park.  THREE DIMENSIONS, ALL RE-DERIVED:
 *   16-bit instructions   142 ref / 143 ours   (+1, was +2)
 *   32-bit entries        6 ref   / 5 ours     (-1)   [1 is the GetFile `bl`,
 *                                                      so 5 pool words / 4]
 *   bytes                 308 / 308            (EXACT -- the +2 from the one
 *                                               instruction plus a 2-byte align
 *                                               pad against the -4 pool word)
 * Aligned residue (tools/aligncmp.py) 97 in 27 hunks, DOWN FROM 108 in 26.
 * All three dimensions and the aligned residue moved the same way, so this is a
 * real improvement and not the phase artifact autosweep warned it might be.
 *
 * THE CHANGE IS ONE LINE: `do { } while (0);` after `off = v * 32;`.  It costs
 * nothing (emits no instruction, not a pin, not counted by shimcount) and it
 * removes one of the two `mov` copies in the entry block -- base emits
 * `mov r9, r0 / mov r8, ip` where the ROM emits neither and this body emits only
 * `mov r8, ip`.  FOUR placements in that straight-line run measure IDENTICALLY
 * at 106 of 148 -- after `off = v * 32;`, after `s = (u8 *)0x6000000 + off;`,
 * after `w = tmp;`, and the two of them together -- and the four are NOT
 * bit-identical (four distinct md5s on the generated `.s`), so they are four
 * different programs at one figure, not one program four times.  Anchors after
 * `p = iwram_3001e8c;` (139), after `file = GetFile(...)` (140) and after
 * `v = *(u8 *)a;` (140) do nothing.
 *
 * THE OLD +2 SPLIT, measured before the anchor, is kept below because it is the
 * map of what remains.
 * The old header's "149 instructions against 148 -- ONE over" was the STREAM
 * length and is wrong as an instruction count; the real gap is +2, and there is
 * a pool dimension it could not see.
 * Aligned residue (tools/aligncmp.py): re-measure with the command below; the
 * raw 140 saturates because the streams differ in length.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/rom_15000/801dd28.c \
 *     asm/rom_15000/rom_1ca1c_c_c_c_c.s --func Func_801dd28
 *
 * ============================================================
 * THE +2 IS ELEVEN SITES OF +-1 AND +-2, NOT ONE SITE OF TWO
 * ============================================================
 * Counted hunk by hunk off an LCS alignment of the two encoding streams
 * (ours - ref, in stream order):
 *   -1  the ROM's extra `ldr r3,[r3,#0]` in the entry block
 *   +2  `mov r9, r0 / mov r8, ip`
 *   -2  the ROM's `mov r4, r9 / lsls r3, r4, #5`
 *   +2  `lsls r3, r0, #5 / adds r2, r1, r3`
 *   +1  `mov ip, r0`
 *   -1  the ROM's `mov r0, ip`
 *   +2  `movs r3, #1 / add ip, r3`
 *   -2  the ROM's `strb r2,[r3,#0] / add ip, r4`
 *   +1  `lsls r3, r0, #24`
 *   +2  `adds r0, r3, r6 / ldrb r3,[r0,#0]`
 *   -1  the ROM's `strb r3,[r0,r2]`   (register-offset store)
 * = +3 real instructions, and then the ROM pays ONE instruction we do not --
 * the `b.n` around its MID-FUNCTION literal pool -- for a net of +2.
 * So "close the gap, then rank on aligned residue" DOES NOT APPLY to this park:
 * the gap has no separate cause.  Every one of those eleven sites is the SAME
 * fact, and it is the one the park already names.
 *
 * ============================================================
 * THE MECHANISM, UNIFIED: HIGH REGISTERS BUY TWO-OPERAND FORMS
 * ============================================================
 * The ROM keeps its long-lived values in r8-r12/r14 and ours keeps them one
 * role rotated with the hot ones in LOW registers.  Three Thumb encodings are
 * available only off a high base and the ROM uses all three:
 *   `add rD, Hs`        two-operand, destructive      vs our `adds rD, rA, rB`
 *   `ldrb rD, [rB, rI]` register-offset load          vs our `adds` + `ldrb #0`
 *   `strb rD, [rB, rI]` register-offset store         vs our `adds` + `strb #0`
 * Each costs us exactly one instruction per site, which is the whole +3.
 *
 * ============================================================
 * THE MISSING POOL WORD IS 0x0000f000, AND IT IS LEVER 2 OVERSHOOTING
 * ============================================================
 * The ROM's mid-function pool is  0x0000f000, iwram_3001e8c, _FILE_13,
 * 0x040000d4, 0x84000008 -- FIVE words at 0x110..0x123 with a `b.n 0x124`
 * around them.  Ours has FOUR words at the function tail and no `b`, because we
 * build 0xf000 in registers:
 *     rom   ldr r2,=0xf000 ... adds r3, r5, #0 / orrs r3, r2
 *     ours  movs r3,#240 / lsls r3,#8 / orrs r3, r1
 * 0xf000 is 0xf0 << 8, so thumb_shiftable_const builds it and gcc never pools
 * it.  The old header's lever 2 ("a named `int` temporary for every value stored
 * as a halfword") fixed the `ldr r2,=0xfffff000` sign-extension defect and
 * ALSO removed the pool word the ROM keeps -- it overshot.  The two spellings
 * come out the same LENGTH at this site, so this is not where the +2 is, but it
 * is why the pool count differs and why the ROM has the `b` and the early dump.
 * A successor wanting byte-exactness must get 0x0000f000 POOLED, not built:
 * that is the arm.md pool_range / LEGITIMATE_CONSTANT_P question, and the
 * HImode-store route (arm.md:4269-4271) is the one documented way to force it.
 *
 * ============================================================
 * THE MECHANICAL AXES -- full output in scratch_elev/b348/E/work/p3_autosweep.txt
 * ============================================================
 * FLAG AXIS, 8 groups, against the PRE-ANCHOR body: -fno-strict-aliasing,
 * -fno-strength-reduce, -fno-rerun-cse-after-loop and -fno-rerun-loop-opt are
 * all bit-identical at 140; -fno-schedule-insns2 122 (aligned 117, worse);
 * -O1 136 (aligned 139, -7 insns); -fno-gcse 138 (aligned 118, -11 insns);
 * -ffixed-r7 148 (aligned 113, +5 insns).  NOTHING beats base on aligned
 * residue.  These rows have NOT been re-run against the anchored body and a
 * recorded negative expires when the body changes -- re-cross them.
 * ANCHOR AXIS, 40 of 40 boundaries, uncapped: the 106 above is its best.
 *
 * NEXT.  Unchanged: rotate the allocation so v/map/off/p/idx/file land in the
 * ROM's roles.  What batch 348 adds is that the instruction-count gap is NOT a
 * separate obstacle to clear first -- it is eleven instances of the rotation --
 * so there is no "close the gap" step to spend a round on.
 */
/* Func_801dd28 -- NON-MATCHING, 140 encodings of 148.  SIZE EXACT (308 bytes both), 149
 * instructions against 148.
 *
 * Verify with:
 *   python3 tools/objcmp.py src/non_matching/rom_15000/801dd28.c \
 *     asm/rom_15000/rom_1ca1c_c_c_c_c.s --func Func_801dd28
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
 *     asm/rom_15000/rom_1ca1c_c_c_c_c.s --func Func_801dd28
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
    do { } while (0);
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
