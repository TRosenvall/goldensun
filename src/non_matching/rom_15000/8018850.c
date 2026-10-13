/* Func_8018850 / MeasureLayoutRuns (0x08018850)
 *
 * NON-MATCHING, 143 of 219 encodings  (re-measured batch 347 brief B).
 *   THE FIGURE IS NOT A DISTANCE.  16-bit INSTRUCTION COUNT ref 181, ours 179 --
 *   an INSTRUCTION GAP OF -2.  219 == 219 is the STREAM length and 512 == 512 is
 *   the byte size; A PAD IS ABSORBING THE DIFFERENCE, and objcmp says so in as
 *   many words on the line directly above the figure:
 *     "NOTE: size and encoding count MATCH -- a pad is absorbing the difference,
 *      so the positional figure below measures MISALIGNMENT, not distance."
 *   The old header's "Reference 219 instructions, ours 219; size and relocations
 *   IDENTICAL, so 143 is a true distance" read that same output and asserted the
 *   opposite.  ALIGNED residue 104 in 35 hunks.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_15000/8018850.c \
 *     asm/rom_15000/rom_17e88_c_c.s --func Func_8018850
 *   ... and tools/aligncmp.py with the same paths plus Func_8018850.
 *
 * WHERE THE GAP IS: THE FRAME, and the first differing encoding is index 7.
 *     ref   sub sp, #32   str r1, [sp, #12]   str r2, [sp, #8]
 *     ours  sub sp, #28   str r1, [sp, #8]    str r2, [sp, #4]
 * Ours is short by ONE four-byte slot and every later slot is shifted down by 4.
 * The ROM's layout is style 0, spill 4, out2 8, out1 0xc, b 0x10, a 0x18; ours
 * has no `spill 4` entry at all.  So this is ONE SITE OF 2, not two of 1: the
 * missing slot's `str` and its reload.  The park named the slot correctly -- the
 * division-loop counter the ROM spills around __divsi3 where gcc keeps it in
 * r11 -- and that part of the diagnosis stands.
 *
 * PER REGION, from aligncmp:
 *     prologue / frame setup (idx 7-29)   count-EQUAL, roles and order differ
 *     the two `a[line]=++n; b[line]=width` blocks   count-EQUAL
 *     the clamp / __divsi3 tail            carries the -2
 *     the 29-entry jump table              EXACT, entry for entry
 *
 * WHAT THE FALSE DISTANCE CLAIM COST, and this is the reason this park was
 * reissued.  The header's one recorded POSITIVE lever is "Writing `j = 0;` at
 * the top of the function DOES produce the ROM's frame (0x20) and its whole slot
 * layout ... but it costs more than it buys (196 differ)".  That 196 was compared
 * against a 143 that was out of phase, so "costs more than it buys" was never a
 * comparison of like with like.  Re-measured:
 *   * `j = 0;` ADDED at the top with the inner one left in place is
 *     BIT-IDENTICAL to this body -- the generated .s differs only in its `.file`
 *     directive.  gcc deletes the dead store, so that spelling measures nothing
 *     and cannot be what produced 196.
 *   * `j = 0;` MOVED to the top (inner one removed -- equivalent, since nothing
 *     else writes `j`) reads 206 of 219 at ins 181/179, size 512/508, ALIGNED
 *     121 in 31 hunks.  So it is genuinely worse on the only scale that compares
 *     across a length change, and the park's verdict survives -- but it survives
 *     for a reason the park could not have stated, and it does NOT produce the
 *     0x20 frame: ours loses four MORE bytes of stack, not gains four.
 *   The recorded lever is therefore unreproducible as written and the frame
 *   claim attached to it is wrong.  Do not spend a round re-deriving it.
 *
 * THE PARK'S OTHER DIAGNOSIS REPRODUCES EXACTLY, and is count-neutral:
 *     ref   mov r2, ip / strh r5, [r3, r2] ... cmp r7, r1      maxw in r7, index in ip
 *     ours              strh r5, [r3, r7] ... cmp ip, r1      maxw in ip, index in r7
 * a two-way swap between `maxw` and the byte index `line * 2`, at both of the
 * `a[line] = ++n` blocks.  It costs zero instructions, which is exactly the shape
 * batch 346's brief warns about: when counts agree and the figure is large,
 * suspect a tie or a preference, not a missing statement.
 *
 * MEASURED INERT at 143, aligned 104 (re-measured this round):
 *   -fno-strict-aliasing and -fno-rerun-loop-opt, bit-identical.
 *   `j = 0;` added at the top (bit-identical, above).
 * FLAG AXIS CLOSED and every other row is a trap -- all five "better" ndiffs
 *   raise the aligned residue: -O1 217/aligned 179, -fno-gcse 217/144,
 *   -fno-schedule-insns2 207/129, -ffixed-r7 216/110, -fno-strength-reduce
 *   210/139, -fno-rerun-cse-after-loop 168.  Note -fno-gcse moves the
 *   instruction count by -21 and -O1 by -18: those are different programs'
 *   shapes, not progress.
 * ANCHOR AXIS CLOSED: all 47 statement boundaries, best 143 against base 143.
 *   Five rows read 141-142 with `first` UNMOVED at index 7 -- phase artifacts.
 * Carried from the previous header and NOT re-measured this round: declaration
 *   order of maxw/c/style/line, `unsigned` on width and n, merging t into maxw
 *   (144, worse).
 *
 * THIS FILE NEEDS A TEXT/DATA SPLIT BEFORE IT CAN LAND, and it cannot be
 * converted whole even byte-identical.  tools/datacheck.py:
 *     data sections : .rodata
 *     functions     : Func_8018850, Func_8018a50
 *     EXPORTS       : .L33e40  (already global -- NOT the set a split needs)
 *     Func_8018850  reads no data label -> split needs NO new export
 * `.L33e40` is a 32-byte .rodata tail read from asm/rom_15000/rom_17e88_a_a.s,
 * and stage1.ld:615 points at rom_17e88_c_c.o(.rodata).  tools/split_s.py
 * --dry-run on (asm/rom_15000/rom_17e88_c_c.s, Func_8018850) reports:
 *     would write asm/rom_15000/rom_17e88_c_c_b.s  (1 function, 252 lines)
 *     would write asm/rom_15000/rom_17e88_c_c_c.s  (1 function, 307 lines)
 *     would REMOVE asm/rom_15000/rom_17e88_c_c.s, would rewrite stage1.ld
 * `exports` is EMPTY for this split -- the function reads no data label, so no
 * `.global` has to be promoted.  Not run: the manifest's `split` field is for
 * landings only and this is a park.
 *
 * NEXT: the question is which allocator owns the division-loop counter, and it
 * must be answered before the -2 is called a priority problem.  A single-block
 * pseudo appears in NEITHER .18.greg's allocno list nor its dispositions --
 * local-alloc.c:1495-1497's QTY_CMP_PRI owns it -- and two of batch 345 brief
 * D's four gaps were local-alloc while both their parks said global_alloc.
 * Check the list membership first.
 *
 * ---- retained from the previous header: WHAT IS RIGHT, the reusable part ----
 *   - CASE ORDER IS SOURCE ORDER AND THE ROM'S BLOCK ADDRESSES RECOVER IT:
 *     case 3, then 0xe/0x1c, then 8/0xa/0xf/0x11, then 9, then 0/1.
 *   - `case 0xe: case 0x1c:` FALLS THROUGH into the 8/0xa/0xf/0x11 group, which is
 *     why the ROM has two consecutive `i = (i + 1) & 0x1ff` blocks.
 *   - The `c > 0x1c` test is gcc's OWN casesi bound check -- do not write it.
 *   - `unsigned int line` is required: the ROM's `cmp r3,#2 / bhi` and `cmp r2,r10 /
 *     bls` are unsigned where a signed local gives bgt/ble.
 *   - The 0xc00 clamp on the __divsi3 result is an UNSIGNED compare (`bls`), so the
 *     quotient needs its own `unsigned int` while the pre-clamp `v < 0` test stays int.
 *   - `unsigned int c` for the character code, and Data_32224 indexed as
 *     `Data_32224[(c - 0x20) * 16]`, give the ROM's `sub r2,#0x20 / lsl r2,#5`.
 *   - The ring is a STRUCT with a member array at 0xeb0 -- that is what produces
 *     `ldrh r2,[r4,r3]` with the base in a register and the whole displacement in the
 *     index, and it is what keeps the ring pointer in a LOW register.
 *   - `*out3 = 0;` written as a bare literal is what produces the ROM's pooled
 *     HImode zero (`ldr r3, .L189ec @ 0`).  Do not reach for an int carrier here.
 *     (Note for whoever revisits this: that pooled HImode zero is the same
 *     *thumb_movhi_insn alternative-1 shape -- pool_range 64, arm.md:4353 -- that
 *     cost OvlFunc_963_2008124 three mid-function pool dumps this batch.  Here the
 *     ROM has it, so here it is wanted.)
 *   - The dead `str r3,[sp,#0]` of the 0xeac style value falls out on its own.
 *   - Its 29-entry jump table reproduces ENTRY FOR ENTRY on the first candidate.
 */
struct R {
    unsigned char pad000[0xea4];
    unsigned char fea4;
    unsigned char padea5[7];
    unsigned short feac;
    unsigned char padeae[2];
    unsigned short text[0x200];
};

extern struct R *iwram_3001e8c;
extern unsigned short Data_32224[];

void Func_8018850(int i, int *out1, int *out2, unsigned short *out3)
{
    struct R *ring;
    unsigned short a[4];
    unsigned short b[4];
    unsigned int c;
    unsigned int maxw;
    int w;
    int width;
    int n;
    int height;
    unsigned int line;
    int style;
    int j;
    int t;
    int v;
    unsigned int u;

    ring = iwram_3001e8c;
    line = 0;
    height = 0xf;
    n = 0;
    maxw = 0;
    width = 0;
    while (1) {
        c = ring->text[i];
        i = (i + 1) & 0x1ff;
        if (c > 0x1f) {
            if (c == 0x20) {
                width += 5;
                n++;
            } else {
                w = Data_32224[(c - 0x20) * 16];
                style = ring->feac;
                if (style == 1 || style == 5)
                    w++;
                width += w;
            }
        } else {
            switch (c) {
            case 3:
                a[line] = ++n;
                b[line] = width;
                if (maxw < width)
                    maxw = width;
                if (line <= 2)
                    line++;
                height += 0xf;
                n = 0;
                width = 0;
                break;
            case 0xe:
            case 0x1c:
                i = (i + 1) & 0x1ff;
            case 8:
            case 0xa:
            case 0xf:
            case 0x11:
                i = (i + 1) & 0x1ff;
                break;
            case 9:
                ring->feac = ring->text[i];
                i = (i + 1) & 0x1ff;
                break;
            case 0:
            case 1:
                a[line] = ++n;
                b[line] = width;
                if (maxw < width)
                    maxw = width;
                if (ring->fea4 != 0)
                    maxw += 2;
                *out1 = maxw;
                *out2 = height;
                t = ((maxw + 0x13) >> 3) * 8 - 0x10;
                if (out3 == 0)
                    return;
                j = 0;
                do {
                    if (a[j] <= 1) {
                        *out3 = 0;
                    } else {
                        v = t - b[j] - 4;
                        if (v < 0)
                            v = 0;
                        u = (v << 8) / (a[j] - 1);
                        if (u > 0xc00)
                            u = 0x200;
                        *out3 = u;
                    }
                    out3++;
                    j++;
                } while (j <= line);
                return;
            }
        }
    }
}
