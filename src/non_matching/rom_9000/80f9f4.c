/* DecodeMetatileset -- NOT MATCHING, a SIZE mismatch: ref 152 bytes against ours 156.
 *
 * BODY RECOVERED FROM scratch/ (batch 338).  This park was PROSE and had no
 * figure; its own header named scratch/hold_f9f4.c, that file still
 * existed and it compiles.  70 of 133 prose parks were in that state --
 * work done, measured, and left in a GITIGNORED directory instead of installed.
 *
 * objcmp stops at the size check, so no encoding figure exists yet; the
 * body must reach the reference's LENGTH before a distance can be read.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/rom_9000/80f9f4.c \
 *     asm/rom_9000/rom_f9cc_a_c_b.s --func DecodeMetatileset
 *
 * The original prose park is preserved below.
 */
extern unsigned char ewram_2010001[];
extern unsigned char ewram_2010002[];
extern unsigned char ewram_2020000[];

void DecodeMetatileset(int n)
{
    unsigned short *src;
    unsigned short *dst;
    unsigned char *hi, *lo;
    int count, i, a, b;
    int prev1, v1;
    int prev2, v2;

    src = (unsigned short *)ewram_2010002;
    dst = (unsigned short *)ewram_2020000;
    count = (n - 1) / 2;
    if ((n & 1) != 0) {
        switch (ewram_2010001[0]) {
        case 0:
            for (i = 0; i < count; i++) {
                *dst = *src;
                src++;
                dst++;
            }
            break;
        case 1:
            hi = (unsigned char *)0x2010002;
            prev1 = 0;
            lo = count + hi;
            for (i = 0; i < count; i++) {
                a = *hi;
                b = *lo;
                a <<= 8;
                v1 = (a | b) ^ prev1;
                *dst = v1;
                lo++;
                hi++;
                dst++;
                prev1 = v1;
            }
            break;
        case 2:
            prev2 = 0;
            for (i = 0; i < count; i++) {
                v2 = *src ^ prev2;
                *dst = v2;
                src++;
                dst++;
                prev2 = v2;
            }
            break;
        }
    }
}

#if 0
/* ===== ORIGINAL PROSE PARK, PRESERVED =====
(* DecodeMetatileset -- 0x0800f9f4, asm/rom_9000/rom_f9cc_a_c_b.s
 *
 * FAKEMATCH.  The instruction stream is exact -- tryc.py reports OK, 78 lines --
 * and the .text is FOUR BYTES LARGER than the reference: 0x9c against 0x98.
 * That shifts everything after it in the translation unit, so `make compare`
 * fails on the ROM sha1 even though every instruction agrees.
 *
 * Candidate kept at scratch/hold_f9f4.c.  The difference is pool padding; the
 * reference keeps its literal pool inside the function body.
 *
 * HOW IT GOT THROUGH, which is the part worth keeping: the screen that produced
 * it used a FOUR-FUNCTION .s as the reference, so tryc printed
 * "[size check skipped: ref has 4 functions]" and only compared instructions.
 * Re-screening the same .c against a single-function reference reported the
 * size mismatch immediately.
 *
 *   Any screen carrying "size check skipped" has NOT been size-checked.
 *   Extract a single-function .s and re-run before wiring it into the build.
 *
 * Worth keeping from the attempt (it was otherwise a correct decompilation):
 * the function loads the same symbol twice, once before its `switch` and again
 * inside case 1, and gcc CSEs the two if both are written as the symbol (46
 * differing).  Writing the second as a bare address literal,
 * `(unsigned char *)0x2010002`, restores the second `ldr` and the ROM's
 * register assignment.  Also: moving `count = (n-1)/2;` from before the two
 * pointer initialisations to after them took it 15 -> exact, where swapping the
 * declarations gave 15 and swapping the assignments gave 23.
 *)

*/
#endif
