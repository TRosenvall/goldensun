/* Func_80f7df0 -- asm/rom_f6000/rom_f6008_c_c_a_a.s
 *
 * BLOCKER: base-plus-offset addressing that gcc folds into pointers.
 *
 * RE-MEASURED BATCH 317: NON-MATCHING, 30 of 32 encodings
 * (ref 32 encodings / 68 bytes, ours 30 / 64),
 * relocation present but at offset 0x38 against the reference's 0x3c -- a
 * SHIFTED OFFSET consequent on being 4 bytes short, not a separate blocker.
 * The old "27 of 30" claim did not reproduce; neither figure had a recipe.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_f6000/80f7df0.c \
 *     asm/rom_f6000/rom_f6008_c_c_a_a.s --func Func_80f7df0
 *
 * ===== BATCH 317: THIS FUNCTION HAD TWO PARKS AND THE BETTER BODY IS LOST =====
 *
 * src/non_matching/rom_f6000/f7df0.c was a SECOND park for this same function,
 * claiming **18 of 30** -- better than anything here. It has been retired to
 * toDelete/ because it contained NO FUNCTION DEFINITION AT ALL: 34 lines, every
 * one a comment, compiling to a ZERO-BYTE TU. Its figure was measured on a body
 * that was never committed, and objcmp will still print a number against an
 * empty TU (the defect parkcheck's NOBODY verdict exists to catch).
 *
 * So the 18 is not reproducible, but its TWO STRUCTURAL FINDINGS ARE REUSABLE
 * and are preserved here verbatim, because they are the route from this body's
 * 30 down to 18 and they describe the mechanism precisely:
 *
 *  1. The ROM addresses everything as `[r4, rOFF]` -- base register plus a
 *     byte offset held in a register -- not as formed pointers. Writing
 *     `*(char **)(b + no)` with `no` a NAMED LOCAL assigned in its own
 *     statement produces that; writing `b + 0x3404 + idx * 4` inline makes gcc
 *     fold the base in first and emit `ldr r3, [r0]` instead. That alone went
 *     28 of 30 -> 23.
 *  2. `add r5, r1, #4` is a separate register for `no + 4`, so that offset
 *     needs its own local too. That went 23 -> 18 and produced the ROM's
 *     extra `push {r5}`.
 *
 * At 18 the residue it recorded was a FOUR-WAY REGISTER PERMUTATION with the
 * instruction sequence identical operation for operation:
 *     rom  r4 = base   r1 = idx*12   r2 = table value  r5 = idx*12+4
 *     ours r5 = base   r4 = idx*12   r3 = table value  r1 = idx*12+4
 * Batch 316 and 317 both landed functions out of exactly that shape via the
 * destination/dying-source combine -- see docs/elevation.md. Reapply the two
 * findings above to this body FIRST, then read .17.lreg.
 *
 * THE LESSON, which is why this is written at length: A PARK WITH NO BODY IS A
 * LOST CANDIDATE, not a record of progress. Two parks for one function let the
 * better one rot in a file nobody compiled.
 *
 * A doubly-linked list head insertion: node at base + i*12, list head at
 * base + v*4 + 0x3000 where v is read from a table at base + i*4 + 0x3404.
 * The arithmetic is all correct -- i*3 then shifted, the 0xc0<<6 head base,
 * the guarded back-link -- and the two-line shortfall is the addressing.
 *
 * THE ROM KEEPS THE BASE IN r4 THROUGHOUT and reaches every field with a
 * register-offset access carrying a COMPLETE byte offset:
 *
 *     rom    ldr r2, [r4, r0]  /  str r3, [r4, r5]  /  ldr r3, [r4, r2]
 *     ours   add r3, r1 (base into the offset) ... ldr r2, [r3, #0x0]
 *
 * It also pushes r5, which we do not: holding three complete offsets live at
 * once costs a callee-saved register that gcc has no reason to spend when it
 * can fold each base-plus-offset into a pointer instead.
 *
 * MEASURED:
 *   offsets written inline in each access          28 lines, 29 differ
 *   EVERY offset named as a complete byte offset
 *     (`n4 = no + 4`, not `b + no + 4`)            28 lines, 27 differ
 *
 * The second applies the name-the-COMPLETE-offset rule from Func_80b6cdc
 * exactly, and it is worth only two differences here. That bounds the lever:
 * it decides addressing FORM when the base register is already right, and it
 * does not stop gcc folding the base itself when doing so saves a register.
 *
 * On Func_80b6cdc the ROM's base was already in the register gcc chose, so
 * naming the offset was the whole fix. Here the fold and the register spend
 * are the same decision, and the source cannot ask for the more expensive one.
 */
extern int ewram_2004c00;

void Func_80f7df0(int i)
{
    char *b;
    int v;
    int vo;
    int no;
    int n4;
    int ho;
    char *nx;

    b = (char *)ewram_2004c00;
    vo = i * 4 + 0x3404;
    v = *(int *)(b + vo);
    no = i * 12;
    n4 = no + 4;
    ho = v * 4 + 0xc0 * 64;
    *(int *)(b + n4) = (int)(b + ho);
    *(int *)(b + no) = *(int *)(b + ho);
    *(int *)(b + ho) = (int)(b + no);
    nx = *(char **)(b + no);
    if (nx != 0)
        *(int *)(nx + 4) = (int)(b + no);
}
