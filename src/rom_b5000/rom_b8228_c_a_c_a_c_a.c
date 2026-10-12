/* Func_80b8824 -- asm/rom_b5000/rom_b8228_c_a_c_a_c_a.s
 *
 * BYTE-IDENTICAL.  objcmp: 100 bytes, 43 encodings and 6 relocations identical;
 * 35 instructions against the ROM's 35.  Pin-free (shimcount: 0).
 * The reference holds ONE function, so this is a whole-file conversion -- no split.
 *
 * Verify with:
 *     python3 tools/objcmp.py src/rom_b5000/rom_b8228_c_a_c_a_c_a.c \
 *         asm/rom_b5000/rom_b8228_c_a_c_a_c_a.s --func Func_80b8824
 *
 * WHAT CLOSED IT, from the park's own 30-of-43 body (batch 338, brief E).
 * The parked figure was PHASE, not distance: 35 instructions against 34.
 * `aligncmp` put the real residue at 8 differing in 6 hunks, and it was one
 * cause plus two consequences (a pool offset and the section-tail pad):
 *
 *     rom   mov r5, sp / ... / adds r1, r5, #0 / bl Func_80b6b40 / ... / mov r8, r5 / mov r5, #0
 *     ours  mov r8, sp / ... / mov  r1, r8     / bl Func_80b6b40 / ...              / mov r5, #0
 *
 * ONE LOCAL DOING TWO JOBS.  `-fcall-used-r4` leaves only r5/r6/r7 call-saved
 * low, and four values cross the loop's two calls (base, off, n, i), so one goes
 * hi -- r8, by REG_ALLOC_ORDER (arm.h:988-993, `3, 2, 1, 0, 12, 14, 4, 5, 6, 7, 8`).
 * BOTH streams do that identically.  The ROM's extra instruction is a FIFTH live
 * range in the same four registers: the address's pre-guard range holds r5, dies
 * at the copy, and `off` is born into r5 behind it.
 *
 * The park had tried a second local (`walk = base;`) and correctly measured it
 * coalesced away.  The problem is not that there are two locals -- it is that the
 * first one is still live and UNCHANGED afterwards, so the copy is redundant and
 * gets propagated.  Make the first local change value immediately after the copy
 * and the copy survives: `p` is the ADDRESS before the guard and the byte OFFSET
 * inside it.  Also lands with `p` declared `short *` and cast at each use, so the
 * artifact is the ROLE REUSE, not the type.
 *
 * A SECOND FIX, and it was invisible to the figure.  The ROM's load is
 * `ldrsh r0, [r5, r2]` -- base register = the OFFSET.  Writing the index as
 * `off + base` rather than `base + off` reproduces that; the reg+reg operand
 * order follows the source order of the `plus`.  All eight v3/ variants read
 * exactly `ndiff=30 first=8 dsize=-4` whether or not they fixed it, because the
 * positional count saturates on a length mismatch.  USE tools/aligncmp.py when
 * you are an instruction short.
 *
 * RECORDED INERT (eight spellings, ONE code generation): unsigned-int base; no
 * second local; the array passed to the call with the base re-derived inside the
 * guard; declarations reordered; the address re-taken inside the guard; `char *`
 * base; `for (;;)` + `if (i == n) break;`; `while (1)` with i and off fused.
 * The loop-shape and base-type axes are both closed on this function.
 */
extern int Random(void);
extern int Func_80b6b40(int a, short *buf);
extern void Func_80b8064(int n);
extern void WaitFrames(int n);
extern void _Func_80175a0(int id);

int Func_80b8824(void)
{
    short buf[14];
    int n;
    int i;
    unsigned int p;
    unsigned int base;

    if ((((unsigned int)Random() << 4) >> 16) != 0) {
        p = (unsigned int)buf;
        n = Func_80b6b40(1, (short *)p);
        i = 0;
        if (n != 0) {
            base = p;
            p = 0;
            do {
                Func_80b8064(*(short *)(p + base));
                i++;
                WaitFrames(8);
                p += 2;
            } while (i != n);
        }
        WaitFrames(0x16);
        return 1;
    }
    _Func_80175a0(0x844);
    return 0;
}
