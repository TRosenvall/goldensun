/* Debug_WarpMenu_UI -- 0x08029094, the only function in
 * asm/rom_15000/rom_23178_a_c_c_a.s (grep -c func_start = 1), so it converts
 * WHOLE-FILE; no data section (datacheck), no split needed.
 *
 * NON-MATCHING: 17 encodings of 163 differ (objcmp, production flags).
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_15000/8029094.c \
 *     asm/rom_15000/rom_23178_a_c_c_a.s --whole
 *
 * 17 IS A TRUE DISTANCE: size and instruction count both match (163 / 163).
 * Every one of the 17 is the same two-register swap, printed with operands:
 *     idx 1   ldr r6,=gKeyRepeat | mov r7,r0
 *     idx 2   mov r7,r0          | ldr r0,=gKeyRepeat
 *     idx 3   mov r0,r3          | mov r6,r3
 *     idx 4,15,23,28,38,62,87,125   ldr r3,[r6] | ldr r3,[r0]
 *     idx 33,36   ldrh/strh [r0]  | [r6]
 *     idx 44,68,94,132  ldrsh [r0,r2] | [r6,r2]
 * The ROM puts `d` in r0 and &gKeyRepeat in r6; we put them the other way round.
 * The idx 1/2 ORDER difference is a CONSEQUENCE, not a second defect: with the
 * pool load targeting r0 it cannot be scheduled before `mov r7,r0`.
 *
 * *** BATCH-316b CORRECTION: THE OLD HEADER'S CLOSURE PROOF IS FALSE. ***
 * It read "THE RESIDUE IS UNREACHABLE BY ARITHMETIC" and turned on this step:
 * "every use of &gKeyRepeat precedes `d`'s last use ... so `d`'s live range is a
 * SUPERSET of &gKeyRepeat's and live_length(38) >= live_length(39)".
 * Measured off `.17.lreg` at production flags:
 *
 *     p38 (d)            R=7   L=44
 *     p39 (&gKeyRepeat)  R=9   L=60
 *
 * L38 = 44 is LESS than L39 = 60.  d's live range is the SHORTER of the two, not
 * a superset.  The live lengths were GUESSED (the brief's "off by ~2x"), and the
 * closure argument was built on the guess.  `allocno_compare` is directly
 * checkable and should never be argued about again:
 *
 *   ;; 10 regs to allocate: 100 120 33 37 56 106 126 39 38 32
 *     p100 R=3  L=3   1.0000 ->r3      p126 R=3  L=6   0.5000 ->r2
 *     p120 R=3  L=3   1.0000 ->r3      p39  R=9  L=60  0.4500 ->r0
 *     p33  R=22 L=117 0.7521 ->r5      p38  R=7  L=44  0.3182 ->r6
 *     p37  R=19 L=110 0.6909 ->r4      p32  R=5  L=104 0.0962 ->r7
 *     p56  R=5  L=17  0.5882 ->r1
 * floor_log2(R)*R/L sorted descending reproduces that printed order EXACTLY, on
 * all ten allocnos.  So the park's CONCLUSION (an allocation order decided by
 * allocno priority, which no flag closes) stands; only its proof of closure dies.
 *
 * THE TARGET IS NOW A NUMBER.  Need pri(38) > 0.4500, i.e. any of:
 *     R38 = 7 and L38 <= 31          (from 44 -- a 13-insn shortening)
 *     R38 = 8 and L38 <= 53          (the floor_log2 step at 8 does the work)
 *     R39 <= 7                       (RULED OUT: the ROM really reads gKeyRepeat
 *                                     eight times and loads its address once --
 *                                     `grep r6` on the reference: one
 *                                     `ldr r6,=gKeyRepeat`, eight `ldr rN,[r6]`)
 * And the flip gives the ROM EXACTLY, verified from the conflict sets rather
 * than assumed: with 38 allocated first it takes r0 (it conflicts with hard regs
 * 1,2,3, so r0 is its only call-clobbered option), and 39 is then pushed off r0
 * (conflict with 38) and off r1 (conflict with 56) onto r6.
 *
 * ========== MEASURED INERT THIS BATCH (R/L unmoved at 7/44 and 9/60) ==========
 *   `*d = *d ^ 1`;  `d[0]` throughout;  `!*d` for the four tests;
 *   `register short *d`;  return type `int`.
 *   `*d = *d;` as a free eighth reference -- DELETED as a no-op store.
 *   AND THE INTERESTING ONE: five placements of a range-splitting local copy
 *   `short *e = d;` with all arms switched to `*e` -- at the top, after the `&1`
 *   early return, after the `&2` early return, and as a braced initialiser.
 *   All four measure ndiff=17 with R=7 L=44 BIT-IDENTICAL.  **Copy propagation
 *   deletes the split before flow measures it** -- the pseudo count drops by one
 *   (the allocno list renumbers 39->40) and nothing else moves.  A source-level
 *   copy is NOT a region split in this compiler.  The one placement that did
 *   split (inside the 0x80/0x40 arm) split the wrong way: R=7 L=48 pri=0.2917,
 *   ndiff 31.
 *
 * Still true from the old park, and still the reading evidence:
 *  - `mov r2,#0 / ldrsh r3,[r0,r2]` is the tell that d is `short *`: thumb-1 has
 *    no ldrsh with an immediate offset, so a signed halfword load through a
 *    pointer always costs a zero register.
 *  - `ldrh r3,[r4] / lsl r3,#16 / cmp r3,r2` with r2 = 0x63<<16 is a SIGNED
 *    halfword compare in the shifted domain -> c is `short *`.
 *  - `ldr r2,=1` / `ldr r3,=0x63` are pooled because they are HImode constants.
 *  - The wrap fixups must be `*c = *c - 0x63` / `*c = *c + 0x63`, NOT 0x59:
 *    gcc CSEs the pre-store load and folds the constants outermost
 *    (0xa - 0x63 = -0x59), which is the ROM's `mov r3,r2 / sub r3,#0x59`.
 *    Writing 0x59 directly is wrong arithmetic AND wrong code (19 of 163).
 *  - No flag closes it: -fno-gcse, -fno-rerun-cse-after-loop,
 *    -fno-strict-aliasing, -fno-caller-saves, -fomit-frame-pointer all 17;
 *    -fno-force-mem 19, -fno-schedule-insns2 31, -fno-cse-follow-jumps 90,
 *    -fno-expensive-optimizations 177.  NOT a per-file flag row.
 *
 * No pins, no barriers, no .equ, no DMA3_SET.  NO fakematch row needed.
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
