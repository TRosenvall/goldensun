/* Func_80228e4 -- 0x080228e4, sole function of asm/rom_15000/rom_21dfc_c_a.s
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/rom_15000/80228e4.c \
 *     asm/rom_15000/rom_21dfc_c_a.s --whole
 * (grep -ci func_start = 1), so it converts WHOLE-FILE; no data section
 * (datacheck), no split needed.
 *
 * NON-MATCHING: 142 encodings of 162 differ (objcmp).
 *
 * 142 IS NOT A DISTANCE, and the figure is corrected from the draft's 145 on
 * re-measurement after parkcheck flagged the claim against the body.  objcmp:
 * `142 of 162 differ (ours 146)` plus `SIZE ref 340 bytes, ours 304` -- 36 bytes and
 * 16 instructions short, so the streams misalign early and nearly everything after
 * index 8 is counted.  Read the named blocker below, not this number.
 * "RELOCATIONS differ" because the sizes differ.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_15000/80228e4.c \
 *     asm/rom_15000/rom_21dfc_c_a.s --whole
 *
 * 145 IS NOT A DISTANCE.  Ours is 154 instructions against the ROM's 168, so
 * the streams misalign 14 instructions in and everything past that is being
 * counted.  Do not read 145 as a distance and do not compare it with the 107
 * on DrawInventoryIcon.  objcmp confirms the shortfall is real and localised
 * after the first call: the FIRST _GetMoveInfo relocation is at 0x34 in both,
 * the second is at ref 0xc2 / ours 0xb6 -- so loop 1's body is 12 bytes short.
 *
 * THE CONTROL FLOW AND THE ARITHMETIC ARE RIGHT.  What is wrong is register
 * pressure, and it is one mechanism with two visible consequences.
 *
 * BLOCKER: the ROM holds TWO simultaneous copies of the mask 0x3fff -- r11 for
 * the outer body (`mov r3,r11 / and r3,r2`) and r7 for the inner search loop
 * (`ldr r7,=0x3fff` in the inner pre-header, `and r3,r7`) -- and that second
 * live copy is what pushes four values out of registers and into memory:
 *   out   -> [sp,#0xc]   (stored at entry, reloaded in loop 2's pre-header)
 *   pc1   -> [sp,#0x8]
 *   c1    -> [sp,#0x4]   incremented IN MEMORY: ldr / add #1 / str
 *   c2    -> [sp,#0x0]   likewise
 * We keep only one 0x3fff live, so we have a spare register, so c1 and c2 stay
 * in registers (r11 and r4) and each increment costs one instruction instead of
 * three.  That accounts for the bulk of the 14.
 *
 * The pressure diagnosis is CONFIRMED, not guessed: forcing the counters to
 * memory with `volatile int c1, c2` takes the length from 154 to 163 against
 * the ROM's 168.  volatile is not the answer (it is a shim, AND it grows the
 * frame to `sub sp,#0x14` against the ROM's `sub sp,#0x10`, because volatile
 * forces addressed access) -- but it proves the residue is the spill set.
 *
 * WHAT WOULD CLOSE IT: a source spelling that keeps two live copies of 0x3fff
 * without a pin.  Two separate named locals both set to 0x3fff (one used by the
 * outer body, one by the inner loop -- kept as scratch_elev/.../t1_e.c) gets
 * the length to 160 of 168 and does spill c1, but gcc still keeps a register
 * copy between the spill and the reload and the frame grows to 0x14.  A single
 * named mask local goes the WRONG way (150 of 168): cse2 collapses it to one
 * copy and REDUCES pressure.  The documented route is
 * `__asm__ ("" : "+r" (m))` on the FIRST use of the repeated pool constant
 * (docs/elevation.md, "Pool-constant CSE: the complete rule"), which would need
 * a fakematch row; not attempted here because the owner has to book the shim.
 *
 * LOAD-BEARING CONSTRUCTS ALREADY CORRECT (each measured):
 *  - The inner search must be a GOTO loop, not a `for`.  The ROM RELOADS `*p`
 *    every iteration (`ldrh r3,[r4]`, r4 a copy of p made in the inner
 *    pre-header); a real C `for` loop lets loop.c hoist the invariant load into
 *    a register and the eor reads the hoisted copy instead.  Measured: `for`
 *    form 160 differ at 157 instructions, goto form 145 differ at 154.
 *  - No entry guard on the pointer bound.  The ROM tests only `*b != 0` before
 *    loop 1 and `*a != 0` before loop 2; the `p > b + 62` half is tested only at
 *    the BOTTOM.  jump.c deletes a provably-true entry test by itself, so the
 *    bound is written as a `break` after the increment, which also puts the two
 *    bottom tests in the ROM's order (bound first, then `*p != 0`).
 *  - Loop 1 stores UNCONDITIONALLY and then ORs 0x8000 into the slot it just
 *    wrote (`out[n] = *p & 0x3fff; n++; ... out[n-1] |= 0x8000;`), which is what
 *    produces the ROM's pointer biased by -2 (`strh r3,[r6,#2] / add r6,#2 /
 *    ldrh r3,[r6]`).  Loop 2 is NOT symmetric -- it stores only in the
 *    not-found branch and combines the mask and 0x4000 before the single store,
 *    so its pointer is unbiased.  The asymmetry is the ROM's, not a mistake.
 *  - c1/c2 are LOCALS copied out at the end (`*pc1 = c1; *pc2 = c2;`), not
 *    `(*pc1)++` in place: the ROM increments [sp,#0x4] and only stores through
 *    pc1 once, in the epilogue.
 *  - The 5th parameter is a genuine stack argument, read once from [sp,#0x30]
 *    (0x20 of saved registers + 0x10 of frame).
 *
 * No pins, no "+r" barriers, no volatile, no DMA3_SET, no .equ in THIS file.
 * NO fakematch row needed as it stands.
 */
extern unsigned char *_GetMoveInfo(int id);

int Func_80228e4(unsigned short *a, unsigned short *b, unsigned short *out,
                 int *pc1, int *pc2)
{
    unsigned short *p;
    unsigned short *q;
    int n;
    int j;
    int c1;
    int c2;

    c1 = 0;
    c2 = 0;
    n = 0;
    p = b;
    while (*p != 0) {
        if (_GetMoveInfo(*p)[1] & 0x80) {
            out[n] = *p & 0x3fff;
            n++;
            j = 0;
            q = a;
            if (((*p ^ *q) & 0x3fff) != 0) {
            inner1:
                j++;
                if (j > 0x1f)
                    goto done1;
                q += 2;
                if (((*p ^ *q) & 0x3fff) != 0)
                    goto inner1;
            }
        done1:
            if (j == 0x20) {
                c1++;
                out[n - 1] |= 0x8000;
            }
        }
        p += 2;
        if (p > b + 62)
            break;
    }
    p = a;
    while (*p != 0) {
        if (_GetMoveInfo(*p)[1] & 0x80) {
            j = 0;
            q = b;
            if (((*p ^ *q) & 0x3fff) != 0) {
            inner2:
                j++;
                if (j > 0x1f)
                    goto done2;
                q += 2;
                if (((*p ^ *q) & 0x3fff) != 0)
                    goto inner2;
            }
        done2:
            if (j == 0x20) {
                c2++;
                out[n] = (*p & 0x3fff) | 0x4000;
                n++;
            }
        }
        p += 2;
        if (p > a + 62)
            break;
    }
    *pc1 = c1;
    *pc2 = c2;
    return n;
}
