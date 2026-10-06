/* Func_80f0254 (ClearBackgroundPage) -- NON-MATCHING.
 *
 * NON-MATCHING, 2 differing encodings of 39  (MEASURED, batch 329 brief J;
 * was 4 of 39).  Counts agree (ref 39, ours 39) and objcmp prints no SIZE and
 * no POOL WORD line, so size and pool content match.  First differing index 14.
 *
 * Verify with: docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work goldensun-build python3 tools/objcmp.py src/non_matching/rom_f0000/80f0254.c asm/rom_f0000/rom_f0254_a_a.s --func Func_80f0254
 *
 * ----- everything below to the next ===== heading is the INHERITED park text,
 * ----- kept verbatim; its figure line and recipe are superseded by the two
 * ----- above.  Its "13 differing" count predates the batch-319 backfill.
 *
 * This recipe was ADDED by the batch-319 backfill: the park had none, so
 * parkcheck.py could not report its figure and nothing had ever checked it.
 * Blocker class: PLACEMENT OF A POOL LOAD among cheap register setup.
 * 33 lines against the ROM's 34, 13 differing.
 *
 * Two DMA3 fixed-source fills, the fill word supplied from a single stack
 * slot. Both `stmia` blocks, both control words and the whole second transfer
 * are exact.
 *
 *     rom    mov r1, #0xc0 / mov r5, #0xa0 / ldr r3, =0x1010101
 *            / lsl r1, #19 / lsl r5, #19
 *     ours   mov r1, #0xc0 / mov r5, #0xa0 / lsl r1, #19 / lsl r5, #19
 *            / ldr r3, =0x1010101
 *
 * The ROM puts the pool load between the two `mov`s and the two `lsl`s; gcc
 * emits it after both shifts. This is the same shape as the argument-precompute
 * class HANDOFF.md diagnoses, except these are not call arguments, so that
 * section's mechanism does not obviously apply and no claim is made here that
 * it does.
 *
 * SOLVED, and worth reusing: SEPARATE THE REGISTER TEMP FROM THE STACK SLOT.
 * Written with one `int v` whose address is taken, gcc stores into the stack
 * slot INSIDE each switch arm -- 20 differing. The ROM computes the fill word
 * into a register across the branch and stores ONCE after the join. Two
 * variables, `value` for the register and `slot` for the address-taken word,
 * reproduces that and is worth 20 -> 14.
 *
 * Tried after that:
 *   - assigning `value` between the two base assignments in arm 0, which is
 *     where the ROM's pool load sits: 13, the best seen
 *   - a named `int *sp = &slot` so the store goes through a pointer the way
 *     the ROM's `mov r4, sp / str r3, [r4]` suggests: 14, no change. gcc
 *     addresses the slot off sp regardless.
 *   - zeroing through the register temp before the second transfer: 14
 *
 * The stream stays one line short in every form, which by the length rule
 * means something the ROM does is still missing rather than merely reordered.
#include "gba/types.h"
#include "gba/io.h"
#include "dma.h"

void Func_80f0254(int page)
{
    int slot;
    int value;
    void *dst;
    void *pal;

    if (page == 0) {
        dst = (void *)(0xc0 << 19);
        value = 0x1010101;
        pal = (void *)(0xa0 << 19);
    } else {
        value = 0x81818181;
        dst = (void *)0x6008000;
        pal = (void *)0x5000100;
    }
    slot = value;
    DMA3_SET(&slot, dst, 0x85001e00);
    slot = 0;
    DMA3_SET(&slot, pal, 0x85000040);
}
 * ===== HALF THE PARK'S RESIDUE WAS A STATEMENT ORDER IN ARM 0. =====
 *
 * Two runs of two at 4 of 39.  The first was the pool load's position:
 *
 *   rom   mov r1,#0xc0 / mov r5,#0xa0 / ldr r3,=0x1010101 / lsl r1,#19 / lsl r5,#19
 *   park  mov r1,#0xc0 / mov r5,#0xa0 / lsl r1,#19 / ldr r3,=0x1010101 / lsl r5,#19
 *
 * `value` must be assigned FIRST in arm 0.  The park's best row put the
 * assignment BETWEEN the two base assignments "which is where the ROM's pool
 * load sits" and read 13; FIRST reads 2.  The park reasoned from where the pool
 * load LANDS IN THE OUTPUT.  sched2's last rung is INSN_LUID, so arm 0's three
 * sets can only ever be permuted by their order IN THE INPUT -- the lever is
 * the statement's position, and reading it off the output is reading it off the
 * wrong end.
 *
 * ===== THE REMAINING 2 ARE A REACHABILITY FLOOR, AND HERE IS THE PASS. =====
 *
 *   rom   mov r4,sp / str r3,[r4,#0x0] / mov r0,r4   (twice)
 *   ours  mov r4,sp / str r3,[sp,#0x0] / mov r0,r4   (twice)
 *
 * `mov r4,sp` and both `mov r0,r4` are already exact; only the two stores use
 * sp.  `-da` dumps of the variant that names the pointer (`q = &slot;
 * *q = value;`) show where it is decided:
 *
 *   .01.sibling   (insn 45 (set (mem:SI (reg/v:SI 34) 5) ...))
 *   .03.cse       (insn 45 (set (mem:SI (addressof:SI (reg/v:SI 38) 33) 5) ...))
 *   .04.addressof (insn 45 (set (mem:SI (plus:SI (reg:SI 25 sfp) ...)) ...))
 *
 * So the EXPANDER does give the store a register address.  cse1 (pass 03)
 * propagates the pointer's value -- which is an ADDRESSOF rtx -- into the MEM,
 * and purge_addressof (pass 04) then lowers it to an sfp displacement.  From
 * pass 04 on, the store is a frame reference and no later pass can hand it a
 * register address again.
 *
 * That is also why no cse flag helps: --no-rerun-cse only disables cse2
 * (pass 09).  MEASURED, each identical at 2 of 39: --no-rerun-cse, -fno-gcse,
 * -fno-cse-follow-jumps.  (--O1 10, --no-sched2 10.)
 *
 * BOUND, with its evidence attached: reaching `str r3,[r4,#0x0]` needs a
 * pointer gcc cannot see as `&local`, which is a device.  Five spellings all
 * measured 2 of 39 and all byte-identical to each other:
 *   - `slot = value` direct (this body)
 *   - `q = &slot;` after the if, store AND argument through q
 *   - `q = &slot;` after the if, store through q, argument still `&slot`
 *   - `int slot[1]; slot[0] = value; DMA3_SET(slot, ...)` -- an ARRAY decl's rtl
 *     is a frame MEM from expand, so it never even takes the addressof route
 *   - `*(int *)&slot = value`
 * MEASURED WORSE: `q = &slot;` BEFORE the if, 14 of 39 (it perturbs arm 0 and
 * undoes the fix above); DMA3_FILL + DMA3_CLEAR 23 of 39 and TWO LINES SHORT;
 * DMA3_FILL twice, the same 23 and two short.
 *
 * SOLVED EARLIER AND STILL KEPT: separate the register temp from the stack
 * slot.  One `int v` whose address is taken puts the store inside each switch
 * arm; `value` for the register and `slot` for the address-taken word is what
 * made the stream the right length.
#include "gba/types.h"
#include "gba/io.h"
#include "dma.h"

void Func_80f0254(int page)
{
    int slot;
    int value;
    void *dst;
    void *pal;

    if (page == 0) {
        value = 0x1010101;
        dst = (void *)(0xc0 << 19);
        pal = (void *)(0xa0 << 19);
    } else {
        value = 0x81818181;
        dst = (void *)0x6008000;
        pal = (void *)0x5000100;
    }
    slot = value;
    DMA3_SET(&slot, dst, 0x85001e00);
    slot = 0;
    DMA3_SET(&slot, pal, 0x85000040);
}
 */
#include "gba/types.h"
#include "gba/io.h"
#include "dma.h"

void Func_80f0254(int page)
{
    int slot;
    int value;
    void *dst;
    void *pal;

    if (page == 0) {
        value = 0x1010101;
        dst = (void *)(0xc0 << 19);
        pal = (void *)(0xa0 << 19);
    } else {
        value = 0x81818181;
        dst = (void *)0x6008000;
        pal = (void *)0x5000100;
    }
    slot = value;
    DMA3_SET(&slot, dst, 0x85001e00);
    slot = 0;
    DMA3_SET(&slot, pal, 0x85000040);
}
