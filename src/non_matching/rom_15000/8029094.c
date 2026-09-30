/* Debug_WarpMenu_UI -- 0x08029094, the only function in
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/rom_15000/8029094.c \
 *     asm/rom_15000/rom_23178_a_c_c_a.s --whole
 * asm/rom_15000/rom_23178_a_c_c_a.s (grep -ci func_start = 1), so it converts
 * WHOLE-FILE; no data section (datacheck), no split needed.
 *
 * NON-MATCHING: 17 encodings of 163 differ (objcmp).
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_15000/8029094.c \
 *     asm/rom_15000/rom_23178_a_c_c_a.s --whole
 *
 * 17 IS A TRUE DISTANCE: size and instruction count both match (ours 163,
 * rom 163; 176 asm lines each).  Every one of the 17 is the SAME two-register
 * swap -- nothing else in the function differs:
 *
 *   rom  ldr r6,=gKeyRepeat  ... ldr r3,[r6]  ... ldrsh r3,[r0,r2]
 *   ours ldr r0,=gKeyRepeat  ... ldr r3,[r0]  ... ldrsh r3,[r6,r2]
 *
 * The ROM puts the parameter `d` in r0 and &gKeyRepeat in r6; we put them the
 * other way round.  Everything else -- all four arms, both call sites, both
 * pool placements, the shifted-domain halfword compares -- is byte-identical.
 *
 * BLOCKER: global.c allocno_compare, the global-register-allocation pass
 * (-da .18.greg).  The two pseudos are 38 (`d`, copied from r3 at entry) and
 * 39 (&gKeyRepeat).  Our allocation order is
 *     ;; 10 regs to allocate: 100 120 33 37 56 106 126 39 38 32
 * so 39 is allocated before 38.  By then r1,r2,r3,r4 are held by
 * higher-priority allocnos (56->r1, 106/126->r2, 100/120->r3, 37->r4), all of
 * which already agree with the ROM, so r0 is the only call-clobbered register
 * left: 39 takes r0 and 38 falls through to callee-saved r6.  The ROM needs the
 * reverse order.
 *
 * THE RESIDUE IS UNREACHABLE BY ARITHMETIC.  allocno_compare ranks on
 *     pri = (floor_log2(n_refs) * n_refs / live_length) * 10000 * size
 * (verified against /opt/camelot-gcc/gcc-2.96/gcc/global.c).  size is 1 word
 * for both.  The instruction stream -- which matches the ROM exactly -- fixes
 * n_refs: &gKeyRepeat is one def plus eight `ldr rN,[r6]` = 9 refs, giving
 * floor_log2(9)*9 = 27; `d` is one def plus one ldrh, one strh and four ldrsh
 * = 7 refs, giving floor_log2(7)*7 = 14.  And live_length cannot rescue it:
 * every use of &gKeyRepeat precedes `d`'s last use (the ldrsh at the head of
 * the 0x200 arm) on every path, so `d`'s live range is a SUPERSET of
 * &gKeyRepeat's and live_length(38) >= live_length(39).  Hence
 *     pri(38) = 14/L38 <= 14/L39 < 27/L39 = pri(39)
 * for every spelling that emits this instruction stream.  To reverse the order
 * &gKeyRepeat would have to fall to 6 refs (floor_log2(6)*6 = 12 < 14), i.e.
 * five reads of gKeyRepeat instead of eight -- a different instruction stream.
 *
 * INERT (each measured singly, all still 17 of 163):
 *   - return type `int` instead of `short`
 *   - reading the register through a local `volatile unsigned int *k`
 *   - Func_8028ef0 declared `void`, declared `()`, and not declared at all
 *     (the bank-specific inverted int-return lever the brief flags: INERT here)
 *   - -fno-gcse, -fno-rerun-cse-after-loop, -fno-strict-aliasing,
 *     -fno-caller-saves, -fomit-frame-pointer  (all 17)
 *   - -fno-force-mem 19, -fno-schedule-insns2 31, -fno-cse-follow-jumps 90,
 *     -fno-expensive-optimizations 177.  NO FLAG CLOSES IT, so this is not a
 *     per-file flag row; it is an allocation tie.
 *
 * Reading notes that ARE load-bearing (each verified by dropping it singly):
 *  - `mov r2,#0 / ldrsh r3,[r0,r2]` is the tell that d is `short *`: thumb-1
 *    has no ldrsh with an immediate offset, only a register offset, so a signed
 *    halfword load through a pointer always costs a zero register.
 *  - `ldrh r3,[r4] / lsl r3,#16 / cmp r3,r2` with r2 = 0x63<<16 is a SIGNED
 *    halfword compare in the shifted domain -> c is `short *`, not
 *    `unsigned short *` (an unsigned compare would not shift).
 *  - `ldr r2,=1` and `ldr r3,=0x63` are pooled small constants but NOT the
 *    symbol tell: they are HImode constants and gcc-2.96 has no immediate
 *    alternative for one.  The zeros that are NOT pooled come free from a
 *    register the preceding failed `and` left holding 0 -- which is why the
 *    0x10 arm stores r1 and the 0x20 arm pools 0x63.
 *  - The wrap fixups must be written `*c = *c - 0x63` / `*c = *c + 0x63`, NOT
 *    `- 0x59` / `+ 0x59`: gcc CSEs the pre-store load into r2 and folds the
 *    constants outermost (0xa - 0x63 = -0x59), which is exactly the ROM's
 *    `mov r3,r2 / sub r3,#0x59`.  Writing 0x59 directly is both wrong
 *    arithmetic and wrong code (measured: 19 of 163).
 *
 * No pins, no barriers, no volatile beyond gKeyRepeat (a real qualifier -- the
 * key state is written by the interrupt handler), no .equ, no DMA3_SET.
 * NO fakematch row needed.
 */
extern volatile unsigned int gKeyRepeat;

extern int Func_8028ef0(int a, int b, short *c);

short Debug_WarpMenu_UI(int a, short b, short *c, short *d)
{
    if (gKeyRepeat & 1)
        return -1;
    if (gKeyRepeat & 2)
        return -2;
    if ((gKeyRepeat & 0x80) || (gKeyRepeat & 0x40)) {
        *d ^= 1;
    } else if (gKeyRepeat & 0x10) {
        if (*d == 0) {
            b = b + 1;
        } else {
            *c = *c + 1;
            if (*c > 0x63)
                *c = 0;
        }
        if (b > 0xc8)
            b = 0;
        Func_8028ef0(a, b, c);
    } else if (gKeyRepeat & 0x20) {
        if (*d == 0) {
            b = b - 1;
        } else {
            *c = *c - 1;
            if (*c < 0)
                *c = 0x63;
        }
        if (b < 0)
            b = 0xc8;
        Func_8028ef0(a, b, c);
    } else if (gKeyRepeat & 0x100) {
        if (*d == 0) {
            *c = 0;
            b = b + 0xa;
        } else {
            *c = *c + 0xa;
            if (*c > 0x63)
                *c = *c - 0x63;
        }
        if (b > 0xc8)
            b = 0;
        Func_8028ef0(a, b, c);
    } else if (gKeyRepeat & 0x200) {
        if (*d == 0) {
            *c = 0;
            b = b - 0xa;
        } else {
            *c = *c - 0xa;
            if (*c < 0)
                *c = *c + 0x63;
        }
        if (b < 0)
            b = 0xc8;
        Func_8028ef0(a, b, c);
    }
    return b;
}
