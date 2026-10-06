/* p4 -- Func_800bfa4 -- PARK at 8 differing encodings of 44.  PIN-FREE, DEVICE-FREE.
 *
 * Figure I measured (not inherited): 8 differing encodings of 44 (ref 44, ours
 * 44), first differing index 7 -- ref 685a (ldr r2,[r3,#4]) against ours 685b
 * (ldr r3,[r3,#4]); indices 7..14 contiguous.  PINS 0.
 * (`--whole` prints SIZE ref 164 / ours 96 because the .s still carries the
 * .rodata the recorded split would separate; the per-function figure is the
 * distance.)
 *
 * Verify with: docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work goldensun-build python3 tools/objcmp.py src/non_matching/rom_9000/rom_bfa4.c asm/rom_9000/rom_be70_c_c.s --func Func_800bfa4
 *
 * SPLIT SHAPE: unchanged from the park -- it needs a text/data split;
 * split_s.py --dry-run yields rom_be70_c_c_b.s (the function) ->
 * src/rom_9000/rom_be70_c_c_b.c and rom_be70_c_c_c.s (the .rodata) stays asm.
 * No new export is needed.
 *
 * THE RESIDUE is indices 7..14, the whole interleaved middle: the second camera
 * word's register and, following from it, which of the two delta chains runs
 * first.  One run, one cause.
 *
 * *** THE PARK'S NAMED REMAINING CAUSE IS ALREADY SATISFIED IN THE dx-FIRST
 *     FORM, SO THE OPEN QUESTION IS THE WRONG ONE. ***
 * The park ends: "a third local-alloc quantity of higher priority than cam[1]'s
 * would have to occupy r3 across cam[1]'s range, and nothing in the program
 * supplies one."  Measured, `.17.lreg` for the dx-chain-first body gives
 *     cx -> r2    cy -> r1    o[2] -> r3    o[4] -> r3
 * BOTH object reads are already in r3, which is the ROM's allocation -- the third
 * quantity exists and does occupy r3 across the second camera word's range.  What
 * goes wrong in that form is different and twofold:
 *   1. cx and cy are SWAPPED against the ROM (ROM: cx -> r1, cy -> r2).  cx takes
 *      r2 because REG_ALLOC_ORDER's head r3 (arm.h:989, `3, 2, 1, 0, ...`) is
 *      still held by the live camera pointer at cx's birth, and nothing bars r2.
 *   2. Because the LOCAL cy holds r1, the GLOBAL allocno for dx is pushed out to
 *      r6 -- `;; 38 in 6` with `Hard regs used: 0 1 2 3 4 5 6` in `.18.greg` --
 *      which is the extra `push {r5, r6, lr}` the park reports.
 * The 8-body's own dump, for contrast: cx -> r1, cy -> r3, o[4] -> r2, o[2] -> r3,
 * `;; 38 in 1` and no r6.
 *
 * SO THE QUESTION IS NOW: what can hold r2 across cx's range?  Over the ROM's
 * indices 6..10 the register file is r0 = the object pointer, r3 = the camera
 * pointer then o[2], r4 = the mask, r5 = the output pointer -- and r2 is held by
 * nothing but cy, which QTY_CMP_PRI allocates AFTER cx.  QTY_CMP_PRI
 * (local-alloc.c:1496) is floor_log2(n_refs) * n_refs * size / (death - birth)
 * and qty_compare (:1504) sorts it descending, so cx's shorter span beats cy's
 * at equal reference counts (four each: load-set, and-use, and-set, minus-use).
 * Lowering cx to two references needs the masked value to be a SEPARATE quantity,
 * which needs cx live past the `and`, which costs a move.
 *
 * MEASURED THIS ROUND -- a COMPLETE 16-cell grid over the two axes the park
 * identified as contradictory, crossed with pre-masking, which the park had only
 * sampled.  Every cell: read order (which camera word first) x pre-mask
 * (none / x / y / both) x chain order (dx / dy).  NOTHING BELOW 8.
 *   8   cam[0] read first, no pre-mask, dy first  -- the park's body, reproduced
 *   8   cam[0] read first, cam[1] PRE-MASKED, dy first  -- NEW, exactly inert,
 *         44 instructions: a second alternative body at the figure
 *  10   cam[1] read first, dy first -- the ROM'S REGISTER MAP; pre-masking either
 *         or both values is exactly inert on it (four cells all read 10)
 *  12   cam[1] read first, dx first; pre-masking cam[0] exactly inert on it
 *  13   both values pre-masked, dx first
 *  14   cam[0] read first, dx first; pre-masking cam[0] EXACTLY INERT on it --
 *         the cross the span arithmetic predicted would work, and it does not
 *  25/31  the two pre-mask-both and pre-mask-y cells that drop an instruction
 *         (42 encodings, 92 bytes, RELOCDIFF) -- structurally different, dead
 * All dx-first cells differ at index 0, ref b520 against ours b560: the extra
 * callee-saved register.  The park's contradiction is now confirmed cell by cell
 * rather than by sampling.
 */
extern int iwram_3001e70;

int Func_800bfa4(int *o, int *out)
{
    int *cam;
    int mask;
    int cx;
    int cy;
    int dx;
    int dy;

    cam = (int *)(iwram_3001e70 + 0xe4);
    mask = 0xffff0000;
    cx = cam[0];
    cy = cam[1];
    dy = o[4] - (cy & mask);
    dx = o[2] - (cx & mask);
    if (dx > -0x200000 && dx < 0x1100000 && dy > 0 && dy < 0xe00000) {
        *out++ = dx >> 16;
        *out = dy >> 16;
        return 0;
    }
    *out++ = 0;
    *out = 0;
    return -1;
}
