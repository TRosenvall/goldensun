/* OvlFunc_916_2008098  --  0x02008098, asm/overlays/rom_7a37f0/ovl_30_c_c_c_a_a_a.s
 * and its byte-identical twin OvlFunc_947_2008cc0  --  0x02008cc0,
 * asm/overlays/rom_7d0e88/ovl_314_a_c_c_c_c.s
 *
 * Source asm: goldensun/asm/overlays/rom_7a37f0/ovl_30_c_c_c_a_a_a.s
 *
 * BLOCKER CLASS: loop-invariant motion of ADDRESS CONSTANTS, and the register
 * pressure it causes. Status: 88 lines against the ROM's 84.
 *
 * A tilemap blit. Walks a w x h window of a 128-int-wide buffer and, for each
 * cell, uses the low twelve bits as an index into two parallel eight-byte-stride
 * tables in EWRAM, writing one word to 0x6002800 + i*4 and the other to
 * 0x6002840 + i*4. The destination index wraps both axes to sixteen and offsets
 * the row by `page << 4`.
 *
 * THE READING IS RIGHT -- every loop bound, both `& 0xf` wraps, the `<< 3`
 * table stride, the `0x80 - w` row advance and the two VRAM bases all appear on
 * both sides. What differs is how many constants live in registers.
 *
 * THE ROM RE-LOADS THREE POOL CONSTANTS EVERY INNER ITERATION:
 *
 *     ldr r6, =ewram_2020000  ...  ldr r6, =ewram_2020004  ...  ldr r1, =0x6002840
 *
 * and holds only 0xfff, 0xf and 0x6002800 across the loop. gcc hoists all five
 * out, which costs five live values, which spills `w`, the row limit, the row
 * stride and `page << 4` to the stack -- a 0x14 frame against the ROM's 8 --
 * and the extra loads and stores are the four surplus instructions.
 *
 * TRIED AND MEASURED:
 *
 *   `*(int *)(0x6002800 + i)` with byte-array externs (this file)   88 / 74
 *   `int` array externs indexed `[(t & 0xfff) * 2]`                 89 / 75
 *   `((int *)0x6002800)[i >> 2]`                                    89 / 75
 *   `i += 0x6002800` once, then `*(int *)i` and `*(int *)(i + 0x40)` 83 / 70
 *
 * The last is the closest in size and no closer in shape: it removes one
 * hoisted constant and gcc immediately hoists something else.
 *
 * `-fno-loop-optimize` DOES NOT EXIST in this cc1 -- `Unrecognized option`. So
 * the pass cannot be switched off to confirm the diagnosis from the other side,
 * and no source spelling stops gcc hoisting an address it can prove invariant.
 *
 * NOT A REGISTER PERMUTATION. Ours needs four more stack slots than the ROM's,
 * so this is not the transposition class from docs/elevation.md -- it is a
 * different decision about what belongs in a register at all. Two functions
 * come with it.
 */

extern unsigned char gBuffer[];
extern unsigned char ewram_2020000[];
extern unsigned char ewram_2020004[];


/* ============================ BATCH 278 UPDATE ============================
 *
 * Re-screened. The park's numbers REPRODUCE EXACTLY (88 lines / 74 differing for the C below,
 * 83 / 70 for its fourth spelling), which confirms this is a faithful record -- worth stating,
 * because batch 277 found a different park whose candidate had drifted out of step with its own
 * recorded levers and whose numbers were therefore not comparable.
 *
 * VERDICT UPGRADED TO UNREACHABLE-WITH-EVIDENCE, AND THE REASON MATTERS FOR TRIAGE.
 *
 * The blocker is what this park already says: gcc hoists FIVE pool constants out of the inner
 * loop where the ROM hoists THREE. The ROM re-loads ewram_2020000, ewram_2020004 and 0x6002840
 * every inner iteration and holds only 0xfff (r8), 0xf (r7) and 0x6002800 (r14). The two extra
 * live values spill four more stack slots -- OUR FRAME IS `add sp, #0x14` AGAINST THE ROM'S
 * `add sp, #0x8` -- and the surplus loads and stores ARE the whole residue.
 *
 * THAT MEANS THE BATCH-277 PRIORITY FORMULA DOES NOT APPLY HERE. This is a LIVE-RANGE and
 * FRAME-SIZE difference, not a register permutation, so there is no allocno contest to price
 * and nothing to separate by respelling. Read the frame sizes before reaching for the
 * arithmetic: `floor_log2(refs) * refs / live_length` answers "which of these two values wins
 * a register", and that is not the question when the ROM simply keeps FEWER values live.
 *
 * MEASURED THIS ROUND, all against the park's 74:
 *
 *   park's C (baseline)                                  88 lines / 74
 *   row base named as an explicit outer-loop local       88 / 78   (worse)
 *   `(t & 0xfff) << 3` named once and used twice         88 / 74   (inert)
 *   `i += 0x6002800`, second store at `i + 0x40`         83 / 70   (park's best, reproduced)
 *   inner loop as a `goto` loop                          85 / 70
 *   inner `goto` loop + named row base                   87 / 77   (worse)
 *
 * THE `goto` LEVER IS A FALSE POSITIVE HERE, and it is worth recording as one. It is what the
 * lever set points at when the ROM recomputes an invariant inside a loop, and it is the only new
 * thing that REACHED the park's best count -- but it gets there by a different route: the first
 * divergence moves to instruction 9, so the whole prologue and both loop headers go wrong. By
 * the "a difference COUNT is not a difference" rule that is the WORSE object, not a step
 * forward, and it was deliberately not installed. Do not re-try it on the strength of the
 * number.
 *
 * The park's key negative still holds: `-fno-loop-optimize` DOES NOT EXIST in this cc1
 * ("Unrecognized option"), so LICM cannot be switched off to confirm from the other side.
 *
 * NEXT: nothing source-level. Anyone picking this up should be trying to make gcc keep two
 * FEWER values live across the inner loop, not to permute the ones it keeps.
 * ======================================================================== */

void OvlFunc_916_2008098(int col, int row, int w, int h, int page, int x0, int y0)
{
    unsigned int *src;
    int x, y, i;
    unsigned int t;

    src = (unsigned int *)gBuffer + (col + (row << 7));
    for (y = y0; y < y0 + h; y++) {
        for (x = x0; x < x0 + w; x++) {
            t = *src++;
            i = ((((y & 0xf) + (page << 4)) << 5) + (x & 0xf)) << 2;
            *(int *)(0x6002800 + i) = *(int *)(ewram_2020000 + ((t & 0xfff) << 3));
            *(int *)(0x6002840 + i) = *(int *)(ewram_2020004 + ((t & 0xfff) << 3));
        }
        src += 0x80 - w;
    }
}
