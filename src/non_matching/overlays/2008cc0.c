/* OvlFunc_947_2008cc0 -- NON-MATCHING, 70 encodings of 86 differ (objcmp).
 *
 * ref 80 instructions / 184 bytes / 86 stream entries
 * ours 79 instructions / 180 bytes / 85 stream entries   -- gap -1 INSTRUCTION
 * aligned-equal 24 of 86 (27.9%).  THE 70 IS MOSTLY PHASE: the streams are
 * compared out of phase from index 7 onward.  Re-derived in batch 345 brief B;
 * the figure, the counts and the byte size all reproduce the park exactly.
 * NO IMPROVEMENT FOUND THIS BATCH.  Body unchanged; header rewritten.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/overlays/2008cc0.c \
 *     asm/overlays/rom_7d0e88/ovl_314_a_c_c_c_c_c.s --func OvlFunc_947_2008cc0
 *
 * LANDING SHAPE: asm/overlays/rom_7d0e88/ovl_314_a_c_c_c_c_c.s holds 1 function
 * (the TARGETS.md piece block for batch 345 confirms it, re-derived with
 * piece.py).  PIN-FREE, DEVICE-FREE.
 *
 * ===== DUPLICATE PAIR, carried forward =====
 * tools/dupfuncs.py pairs this with OvlFunc_916_2008098 (overlays/2008098.c),
 * at 78 against this body's 70.  The two ROM bodies are byte-identical twins
 * (both 80 instructions, differing only in four branch label names), so IF THIS
 * ONE LANDS THE PORT IS OWED ON THE TWIN -- and it is not three renames: the
 * bodies reference different numbers of data labels, so label and pool order
 * must be mapped by hand against both references, then MEASURED.
 *
 * ===== THE PARK'S DIAGNOSIS SURVIVES, AND IT IS NOW COUNTED =====
 *
 * The park (via 2008098.c) says "LICM hoists five pool constants where the ROM
 * hoists three".  Confirmed by reading the reference, with the counts:
 *
 *   ROM, outer preheader:   ldr r2, =gBuffer
 *                           mov r3,#0x80 / sub r3,r6 / lsl r3,#2 / mov r11,r3
 *                           ldr r3,[sp,#0x28] / lsl r3,#4 / mov r9,r3
 *   ROM, middle preheader:  ldr r3, =0xfff / mov r7,#0xf / mov r8,r3
 *                           ldr r6, =0x6002800 / mov r14,r6
 *   ROM, INSIDE the inner loop, reloaded EVERY iteration -- THREE of them:
 *                           ldr r6, =ewram_2020000
 *                           ldr r6, =ewram_2020004
 *                           ldr r1, =0x6002840
 *
 *   OURS, middle preheader: FOUR consecutive `ldr rX,[pc,...]` (ours[40:43] at
 *                           0xa4/0xa8/0xac/0xb0) -- ewram_2020000,
 *                           ewram_2020004, 0x6002800 and 0x6002840 ALL hoisted.
 *
 * So the gap is not one site: THE ROM DECLINES TO HOIST THREE POOL LOADS OUT OF
 * THE INNERMOST LOOP, and the surplus hoisted invariants are what rotate the
 * register assignment through the entire body -- which is why the aligned
 * residue is only 24 of 86 for a function that is one instruction from the
 * right length.  Our r9/r10/r11/r12/r14 assignment and the ROM's agree on
 * neither the count nor the contents.
 *
 * ===== MEASURED NEGATIVES, batch 345 brief B =====
 *
 * 1. SPLITTING THE BASE CONSTANT IS WORSE, and it looked like the obvious read
 *    of the reference.  The ROM does NOT derive the second store's address by
 *    adding 0x40; it pools 0x6002800 and 0x6002840 SEPARATELY and uses a
 *    register-offset store for the first (`str r2,[r3,r6]`) and `add r3,r1 /
 *    str r2,[r3]` for the second.  Written that way --
 *        *(int *)(i + 0x6002800) = ...;   *(int *)(i + 0x6002840) = ...;
 *    instead of the park's `i += 0x6002800;` then `i` and `i + 0x40` -- the
 *    function goes to 84 instructions against 80 (gap -1 -> +4), 192 bytes, and
 *    aligned-equal FALLS 24 -> 23 of 86.  Same at -fno-gcse (92 entries).
 *    Body at scratch_elev/b345/B/work/p2_a.c.  The park's `i +=` form is right.
 *
 * 2. THE `-fno-gcse` FLAG ROW IS REAL BUT SMALL, and autosweep's own guard
 *    flags it: figure 70 -> 66 AND the 16-bit instruction count goes to +0 --
 *    but the byte size stays 180 against 184 and the stream stays 85 against
 *    86, so a pool word is still missing and the row is still partly phase.
 *    Aligned-equal does improve for real, 24 -> 32 of 86 (27.9% -> 37.2%), so
 *    this is NOT a false lead -- it is the right axis and it is CROSSABLE.
 *    A next round should start from -fno-gcse, not from plain -O2: gcse is one
 *    of the two passes that can hoist a pooled address out of a loop, and the
 *    diagnosis above is a hoisting diagnosis.
 *
 * AUTOSWEPT (batch 345, --max 60).  ANCHOR AXIS CLOSED: 12 of 12 boundaries,
 * all inert at 70 (4 produced no row -- untested, not inert).  FLAG AXIS: only
 * -fno-gcse moves anything; -O1 (80), -fno-schedule-insns2 (74), -ffixed-r7
 * (84), -fno-rerun-cse-after-loop (89) and -fno-rerun-loop-opt (78) are all
 * WORSE, and -fno-strict-aliasing / -fno-strength-reduce are exactly inert.
 *
 * NEXT, concretely: the question is what stops loop.c / gcse from hoisting a
 * `(mem (symbol_ref))` out of the INNERMOST loop while still hoisting it out of
 * the middle one.  Count the `ldr` insns per dump (not reason about the pass --
 * batch 344's Func_80167e0 had its attribution wrong in three successive
 * headers) and compare .13.loop against .07.gcse.
 */
extern unsigned char gBuffer[];
extern unsigned char ewram_2020000[];
extern unsigned char ewram_2020004[];



void OvlFunc_947_2008cc0(int col, int row, int w, int h, int page, int x0, int y0)
{
    unsigned int *src;
    int x, y, i;
    unsigned int t;

    src = (unsigned int *)gBuffer + (col + (row << 7));
    for (y = y0; y < y0 + h; y++) {
        for (x = x0; x < x0 + w; x++) {
            t = *src++;
            i = ((((y & 0xf) + (page << 4)) << 5) + (x & 0xf)) << 2;
            i += 0x6002800;
            *(int *)i = *(int *)(ewram_2020000 + ((t & 0xfff) << 3));
            *(int *)(i + 0x40) = *(int *)(ewram_2020004 + ((t & 0xfff) << 3));
        }
        src += 0x80 - w;
    }
}
