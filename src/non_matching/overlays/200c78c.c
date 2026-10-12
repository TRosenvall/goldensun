/* OvlFunc_896_200c78c -- NON-MATCHING, 8 differing STREAM encodings of 90.
 *
 * BODY RECOVERED FROM scratch/ (batch 338).  This park was PROSE -- a header,
 * a diagnosis, and no compilable body -- so it had no figure, was NO CLAIM in
 * parkcheck, and was unrankable in every band.  But its own header named a
 * candidate at scratch/Lc78c.c, that file still existed, and it
 * compiles.  The work had been done and left in a GITIGNORED directory instead
 * of installed, so nothing in the tree could see it.
 *
 * ref 90 / ours 90 stream encodings; 80/80 16-bit instructions -- IN PHASE, so a true DISTANCE.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/overlays/200c78c.c \
 *     asm/overlays/rom_78ef88/ovl_314_c_c_c_c_c_c.s --func OvlFunc_896_200c78c
 *
 * 70 of the 133 prose parks were in this state; tools/scratchfill.py found them.
 * THE FIGURE IS EVIDENCE; the prose below it is a HYPOTHESIS, and across pass
 * two a park's stated blocker has been wrong roughly 40 times in 42.  The
 * original park text is preserved underneath.
 */
#include "dma.h"

extern unsigned char L5140[] __asm__(".L5140");
extern unsigned char L5168[] __asm__(".L5168");
extern unsigned char *__galloc_ewram(int tag, int size);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __Func_800c548(unsigned char *a, int n);
extern void __StartTask(void *fn, int pri);
extern void OvlFunc_896_200c49c(void);

void OvlFunc_896_200c78c(int slot, unsigned int n)
{
    unsigned char *buf;
    unsigned char *p;
    unsigned char *a;
    unsigned int i;
    unsigned int j;
    int z;

    buf = __galloc_ewram(0x21, 0xca << 1);
    p = buf;
    DMA3_CLEAR(buf, 0xca << 1);
    if (n > 0xa)
        n = 0xa;
    i = 0;
    if (n != 0) {
        z = 0;
        j = 0;
        do {
            a = __MapActor_GetActor(slot);
            *(unsigned char *)(*(int *)(a + 0x50) + 0x26) = z;
            *(unsigned char **)p = a;
            a += 0x55;
            *a = z;
            a = __MapActor_GetActor(slot);
            __Func_800c548(a, 1);
            *(int *)(p + 0x1c) = *(int *)((char *)L5140 + j);
            *(int *)(p + 0x20) = -*(int *)((char *)L5168 + j);
            p[0x24] = 3;
            i++;
            j += 4;
            p += 0x28;
            slot++;
        } while (i != n);
    }
    *(short *)(buf + (0xc8 << 1)) = n;
    __StartTask(OvlFunc_896_200c49c, 0xc8 << 4);
}

#if 0
/* ===== ORIGINAL PROSE PARK, PRESERVED =====
(* OvlFunc_896_200c78c -- 0x0200c78c, asm/overlays/rom_78ef88/ovl_314_c_c_c_c_c.s
 * and its twin OvlFunc_897_200b30c -- 0x0200b30c,
 * asm/overlays/rom_791794/ovl_30_c_c_c_c_c.s
 *
 * The two have IDENTICAL opcode streams and differ in exactly three constants
 * (two table labels and one task function), so one solution elevates both.
 *
 * 88 of 88 lines, EIGHT differing.  Candidate at scratch/Lc78c.c.
 *
 * SOLVED:
 *   - `DMA3_CLEAR(buf, 0xca << 1)` from include/dma.h reproduces the
 *     `stmia r3!, {r0,r1,r2} / sub r3,#0xc` pair; the size is recoverable from
 *     the control word (0x85000065 -> 0x65 words -> 0x194 bytes).
 *   - `p = buf;` must come BEFORE the DMA call, not after: gcc otherwise
 *     schedules `mov r5, r9` past the whole DMA sequence.  50 differing -> 44.
 *   - Consuming the actor pointer (`a += 0x55; *a = z;` rather than
 *     `a[0x55] = z;`) -- 44 -> 8, the single biggest step.
 *
 * BLOCKER, three small things that together are the whole residue:
 *   - the ROM interleaves `str r0, [r5]` between the two byte stores and gcc
 *     will not (three lines, and swapping the source statements only moves
 *     which three);
 *   - the two table loads use OPPOSITE reg+reg operand orders in the ROM --
 *     `ldr r3, [r7, r2]` for the first and `ldr r3, [r3, r7]` for the second --
 *     and gcc emits the same order for both.  Writing the two address
 *     expressions with the operands reversed (`j + (int)T` versus
 *     `(int)T + j`) changes nothing; gcc canonicalises before it picks.
 *   - `lsl r1, #4` and `ldr r0, =fn` are swapped at the __StartTask call.
 *
 * TRIED: a separate byte-offset variable per table (56 differing, much worse);
 * both statement orders for the stores; naming the task priority so the
 * interleave lever could act on the final call.
 *)

*/
#endif
