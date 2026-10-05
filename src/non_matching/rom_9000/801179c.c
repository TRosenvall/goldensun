/* Func_801179c (TileAnimationTask) -- asm/rom_9000/rom_11568_a_c_c_c.s
 *
 * NON-MATCHING, 32 of 127 encodings, NO PINS  (RE-MEASURED, batch 324 brief H).
 * SIZE EQUAL (objcmp prints no SIZE line); 119 real instructions against 119
 * and 127 encodings against 127, so the figure IS a distance, not misalignment.
 * `--whole` agrees: 32 of 127, first at index 9, relocations clean.  The whole
 * `.s` is this one function, so no split is needed once it matches.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_9000/801179c.c \
 *     asm/rom_9000/rom_11568_a_c_c_c.s --func Func_801179c
 *
 * The body is UNCHANGED from the previous park -- 18 further crossed edits were
 * measured this round and every one is exactly inert or worse.  What is new is
 * the DECOMPOSITION and the magnitudes, which the previous header did not have.
 *
 * ================  THE 32, DECOMPOSED INTO FOUR RUNS  ================
 *
 * Measured by aligning the two instruction streams by mnemonic (scratch
 * harness scratch_elev/b324/H/align.py) rather than by positional index, so
 * each run is a real region and not an artefact of one shift.
 *
 * RUN 1 -- the prologue, ~11 encodings.  POOL ORDER, and ONLY pool order.
 *   rom   iwram_3001e70, 0x6004000, 0xffff, 0x6008000
 *   ours  iwram_3001e70, 0xffff,    0x6004000, 0x6008000
 *   The previous header's BLOCKER 2 is CONFIRMED and is worth stating more
 *   strongly than it was: the HI-REGISTER ASSIGNMENT IS ALREADY CORRECT.  Both
 *   compiles put 0x6004000 in r12 and 0xffff in r14; the ROM reaches them as
 *   `ldr r2,=0x6004000 / ... / mov r12,r2` and `ldr r3,=0xffff / ... / mov r14,r3`
 *   while ours loads the same two values into the same two temporaries in the
 *   opposite order and hence writes `mov r14,r2` / `mov r12,r3`.  Everything
 *   else in the run is the surrounding `mov r5,r8` / `add r5,#0x18` order
 *   moving with it.
 *
 * RUN 2 -- the cursor pointer, ~11 encodings.  `p` and `count` are swapped:
 *   the ROM has p=r4, count=r2; ours has p=r2, count=r4.  Previous header's
 *   BLOCKER 1, confirmed.  `register u16 *p __asm__("r4")` reads 21 of 127
 *   (also confirmed verbatim), so this run is worth 11 and NOT "most of the
 *   residue" as the header claimed.
 *
 * RUN 3 -- the two DMA source adds, ~8 encodings (4 each).
 *   rom   ldr r4,POOL / lsl r2,#3 / lsl r0,#5 / lsl r1,#5 / ldr r3,=REG_DMA3SAD / add r0,r0,r4
 *   ours  lsl r2,r4,#3 / lsl r4,r0,#5 / ldr r0,POOL / lsl r1,r1,#5 / ...     / add r0,r4,r0
 *   The RTL is IDENTICAL -- both are `(plus op32 sym)` with the destination
 *   pinned to r0 by DMA3_SET's `register ... __asm__("r0")` -- so the operand
 *   order is NOT the difference and the three `op * 32 + sym` rewrites below
 *   are inert for that reason.  The difference is which of {op<<5, sym} gets
 *   r0: the ROM loads the symbol FIRST (into r4) and shifts `op` in place in
 *   r0; we shift first (into r4) and load the symbol into r0.  qty numbers in
 *   local-alloc follow first use in the insn stream, so this is decided by the
 *   ORDER of the `ldr` against the `lsl` inside the branch -- a sched2
 *   question, and the next thing to read is `*.23.sched2` for that block.
 *   THIS RUN SURVIVES THE p=r4 PIN UNCHANGED, so it is NOT downstream of
 *   run 2 -- which the previous header left open.
 *
 * RUN 4 -- the `op >= 0x200` compare, 3 encodings.
 *   rom `mov r3,#0x80 / lsl r3,#2 / cmp r0,r3`;  ours the same in r2 (r4 under
 *   the pin).  A free-scratch choice downstream of runs 2 and 3.
 *
 * 11 + 11 + 8 + 3 = 33, and two of those positions coincide, which is the 32.
 *
 * ================  MEASURED THIS ROUND  ================
 *
 * All at 119 instructions against 119, so these are distances.  crossfire.py,
 * depth 2, reference memory profile ldr=16 ldrb=1 ldrh=6 str=3 strh=2 matched
 * on every row below that has a figure.
 *
 *   this body (base)                                         32
 *   `0x6004000` hoisted to a named local before the loop      32  exactly inert
 *   `&ewram_201c000[op * 32]` for the source                  32  exactly inert
 *   `&ewram_2020000[op * 64]` for the source                  32  exactly inert
 *   `op * 32 + ewram_201c000` (operand order flipped)         32  exactly inert
 *   `op * 64 + ewram_2020000` (operand order flipped)         32  exactly inert
 *   an explicit `(unsigned char *)` cast on the source        32  exactly inert
 *   `op = p[0]; p++;` instead of `op = *p++;`                  32  exactly inert
 *   the `op == 0xffff` arm rewritten as `!=` + a goto label    32  exactly inert
 *   ... and every depth-2 crossing of the above               32
 *   `if (op < 0x600)` with the arms swapped       44, RELOC+INSNS (wrong program)
 *   `count = p[0]; dst = p[1]; c->delay = p[2]; p += 2;`      93, 125 insns
 *   `u32 v4 = 0x6004000;` used through `(u8 *)(v4 + ...)`    123, 264 bytes
 *   a block-local `u8 *s = ewram_201c000;` in one arm        124, 129 insns
 *
 * TWO COMPILER FACTS WORTH KEEPING.
 *   * Passing a NON-CONSTANT pointer local as DMA3_SET's `dst` ICEs this gcc:
 *     "Internal compiler error in expand_inline_function, at integrate.c:678".
 *     Four variants hit it.  So the destination argument of DMA3_SET has to be
 *     a constant-folded address expression, which closes off the whole family
 *     of "name the VRAM base" rewrites for the destination.
 *   * The park's BLOCKER 3 wording ("ROM ties op<<5 to r0 and the constant to
 *     r4; ours ties the constant") is right about the symptom but the cause is
 *     not operand order -- see run 3.
 *
 * DEVICE-DERIVED FIGURE, about the blocker only (NOT shipped):
 *   `register u16 *p __asm__("r4")`   21 of 127.  Confirms run 2 at 11 and
 *   leaves runs 1, 3 and 4 standing; with the pin the alignment collapses to
 *   exactly those three regions and nothing else.
 *
 * NEXT MOVE: run 1 (pool order) is the largest and the most tractable -- it is
 * four SImode prologue loads whose `push_minipool_fix` order is instruction
 * order, so read `*.26.mach` and `*.23.sched2` for the preheader block and ask
 * what puts our 0xffff load second.  Do NOT re-sweep the operand orders in
 * run 3; eleven spellings are now measured exactly inert.
 */
#include "gba/types.h"
#include "dma.h"

extern unsigned char *iwram_3001e70;
extern unsigned char ewram_201c000[];
extern unsigned char ewram_2020000[];

typedef struct {
    u16 *base;
    u16 *cursor;
    u16 delay;
    u16 paused;
} TileAnim;

void Func_801179c(void)
{
    unsigned char *st = iwram_3001e70;
    TileAnim *c = (TileAnim *)(st + 0x18);
    u32 i;

    for (i = 0; i <= 15; i++, c++) {
        u16 *p;
        u32 op, count, dst;
        if (c->base == 0 || c->paused != 0)
            continue;
    next:
        if (c->delay != 0)
            goto dec;
        p = c->cursor;
        op = *p++;
        if (op == 0xffff) {
            c->cursor = c->base;
            goto next;
        }
        if ((op & 0xff00) == 0xfe00) {
            u32 t = op & 0xff;
            if (t == 0xff)
                continue;
            c->cursor = (u16 *)((u8 *)c->base + (t << 2));
            goto next;
        }
        count = *p++;
        dst = p[0];
        c->delay = p[1];
        if (st[0x16] == 0) {
            if (op >= 0x600)
                DMA3_SET(ewram_201c000 + op * 32, (u8 *)0x6004000 + dst * 32, 0x84000000 | (count * 8));
            else
                DMA3_SET((u8 *)0x6004000 + op * 32, (u8 *)0x6004000 + dst * 32, 0x84000000 | (count * 8));
        } else {
            if (op >= 0x200)
                DMA3_SET(ewram_2020000 + op * 64, (u8 *)0x6008000 + dst * 64, 0x84000000 | (count * 16));
            else
                DMA3_SET((u8 *)0x6008000 + op * 64, (u8 *)0x6008000 + dst * 64, 0x84000000 | (count * 16));
        }
        c->cursor += 4;
        goto next;
    dec:
        c->delay--;
    }
}
