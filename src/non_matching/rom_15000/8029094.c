/* Debug_WarpMenu_UI -- 0x08029094, asm/rom_15000/rom_23178_a_c_c_a.s
 *
 * STILL NON-MATCHING, **17 of 163 STREAM encodings** -- RE-DERIVED batch 340
 * brief E and UNMOVED.  Instruction counts **155 / 155** (IN PHASE, a true
 * distance), size 652 bytes equal, relocations IDENTICAL.  First differing
 * index 1.  PINS: 0.  DEVICES: 0.  BODY UNCHANGED from batch 328.
 *   objcmp verbatim:
 *     XX ENCODINGS differ in 17 place(s) (ref 163, ours 163)
 *        [STREAM length, pool+pad INCLUDED; instructions are 155/155]
 *        first at index 1: ref 4e19  ours 1c07
 *
 * ONE FIGURE CORRECTION: the batch-328 header says "Size exact (163 against
 * 163)".  163 is the STREAM count (pool + pad included); the SIZE is 652
 * bytes and the INSTRUCTION count is 155, which the rest of that header uses
 * correctly.  Nothing downstream of it changes.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_15000/8029094.c \
 *     asm/rom_15000/rom_23178_a_c_c_a.s --whole
 * (`--func Debug_WarpMenu_UI` gives the same figure and also prints both
 * instruction counts, which `--whole` does not.)  The .s holds exactly one
 * `.thumb_func_start`, so this converts WHOLE-FILE; datacheck.py exits 0, NO
 * SPLIT.  `Func_8029094` IS NOT A SYMBOL in this tree; the filename is an
 * address.  tools/piece.py confirms a ONE-member piece.
 *
 * ======================================================================
 * BATCH 340: BOTH CHEAP AXES CLOSED, AND THE LOOP-NOTE ROUTE CLOSED AT ALL
 * FIVE PLACEMENTS RATHER THAN ONE.  THE DIAGNOSIS BELOW SURVIVED INTACT.
 * ======================================================================
 *
 * AUTOSWEEP (tools/autosweep.py, run first -- raw in scratch_elev/b340/E/work):
 *   FLAG AXIS, all eight per-file groups on the unchanged body:
 *     -fno-gcse 17, -fno-strict-aliasing 17, -fno-strength-reduce 17,
 *     -fno-rerun-cse-after-loop 17, -fno-rerun-loop-opt 17  (all INERT)
 *     -ffixed-r7 21, -fno-schedule-insns2 31, -O1 160          (all WORSE)
 *   ANCHOR AXIS: **26 boundaries, ALL INERT at 17** -- nested boundaries
 *     included, so every arm of the key chain was anchored.
 *   >> BOTH CHEAP AXES ARE CLOSED ON THIS PARK.  Do not re-screen them. <<
 *   This matters more here than usual, because an anchor was the lever that
 *   moved the other three targets in this brief (Func_8094154 16->8 needed a
 *   statement split instead, AnimEnd 14->2 needed an anchor); this park is the
 *   one where it genuinely has nothing to grab.
 *
 * THE ZERO-INSTRUCTION EIGHTH REFERENCE, CLOSED PROPERLY.  The park asks for
 * "a ZERO-INSTRUCTION eighth reference to the `d` pointer pseudo" and records
 * ONE attempt at it: *"do-while(0) around `*d ^= 1` to get loop_depth's x2 on
 * the reference -- 119 at 164 instructions"*.  A `do { } while (0)` WRAPPER
 * (not an anchor beside the statement -- a wrapper AROUND it) is the right
 * instrument for that, because the loop notes give every reference inside the
 * loop_depth weight at no instruction cost.  All five `d`-bearing statements
 * and three combinations, measured this batch:
 *     wrap the `*d ^= 1` arm                  119, 164 insns, +4 bytes (the
 *                                             park's row, reproduced exactly)
 *     wrap the 0x20 arm's `if (*d == 0)`       28  (155/155, relocs clean)
 *     wrap the 0x200 arm's                     32  (155/155, relocs clean)
 *     wrap the 0x100 arm's                     36  (155/155, relocs clean)
 *     wrap the 0x10 arm's                     138, -4 bytes, RELOCDIFF
 *     wrap the 0x100 and 0x200 arms together   50  (155/155, relocs clean)
 *     wrap all four `if (*d == 0)` chains     137, -4 bytes, RELOCDIFF
 *     wrap the 0x200 arm AND the xor arm      119, +4 bytes, RELOCDIFF
 *   >> THE LOOP-NOTE ROUTE TO REG_N_REFS IS CLOSED IN THIS FUNCTION, now at
 *   >> five placements and three crossings rather than one.  The park's
 *   >> sentence "the REG_N_REFS loop weight is not reachable at zero cost in
 *   >> this function" is CONFIRMED and is no longer a one-sample claim.
 *   Note the shape of the failures: the three that keep 155/155 and clean
 *   relocations (28, 32, 36) are pure allocation churn -- the loop weight
 *   lands on `c` and `b` as well as `d`, so it buys p38 nothing relative to
 *   its neighbours.  That is the reason, not the instruction cost.
 *
 * ===================== THE RESIDUE IS ONE ALLOCNO INVERSION =================
 *
 * All 17 are one cause, unchanged from the old park and re-measured: pseudo 38
 * (`d`) and pseudo 39 (`&gKeyRepeat`) hold each other's hard register.  The ROM
 * puts `d` in r0 and the gKeyRepeat address in r6; we do the reverse.
 *
 * `.17.lreg` reproduces the old park's figures EXACTLY, so they are now twice
 * measured:   R38 = 7, L38 = 44      R39 = 9, L39 = 60
 * `.18.greg`  `;; 10 regs to allocate: 100 120 33 37 56 106 126 39 38 32`
 *   pri(39) = floor_log2(9)*9 / 60 = 27/60 -> 4500
 *   pri(38) = floor_log2(7)*7 / 44 = 14/44 -> 3181
 * so p39 is allocated first, r0 is open to it, and p38 -- whose only call-used
 * option is r0, since it conflicts with hard regs 1, 2 and 3 -- is pushed to r6.
 *
 * *** THE INSTRUMENT, AND WHAT IT PROVES ***
 *
 * `(void)*(volatile short *)d;` as the first statement is a DEVICE (owner
 * standard 4: an instrument, never a result).  It is ONE volatile halfword read
 * and it exists only to add one reference to p38.  Measured:
 *
 *   R38 7 -> 8, L38 44 -> 45, so pri(38) = 24/45 -> 5333, and `.18.greg`
 *   becomes `;; 10 regs to allocate: 101 121 33 37 57 *38* 107 127 40 32`
 *   -- p38 moves from NINTH to SIXTH, ahead of the gKeyRepeat pointer.
 *
 *   The prologue then IS the ROM's, instruction for instruction AND register
 *   for register, including the pool load landing FIRST:
 *       push {r5,r6,r7,lr} / ldr r6,=gKeyRepeat / mov r7,r0 / mov r0,r3 /
 *       [the device] / ldr r3,[r6] / mov r4,r2 / mov r2,#1 / lsl r1,#16
 *
 *   tools/tryc.py --align: **1 instruction in disagreeing regions, of 176** --
 *   a single INSERT at index 4, which is the device itself.  objcmp: ref 155
 *   instructions / ours 156, pool words 5/5, size and encoding count MATCH.
 *
 * >> SO THE WHOLE 17 IS ONE ALLOCNO-ORDER INVERSION, and the only thing still
 * >> missing is a ZERO-INSTRUCTION eighth reference to the `d` pointer pseudo.
 * >> Equivalently, since pri(38) >= pri(39) is the condition and a TIE GOES TO
 * >> p38 (allocno_compare returns v1 - v2 and allocno numbers follow pseudo
 * >> numbers): R38 = 8 at any L38 <= 53, or L38 <= 31 at R38 = 7, or L39 >= 85.
 *
 * ===================== THE ROM'S OWN R38 IS 7, SO ROUTE A IS NOT THE ORIGINAL
 *
 * Counting p38's references in the REFERENCE stream: `mov r0,r3` (the def) plus
 * four `ldrsh r3,[r0,r2]` plus `ldrh r3,[r0]` plus `strh r3,[r0]` = SEVEN.  The
 * original therefore did NOT reach this order by adding a reference, and the
 * free variable is a LIVE LENGTH rather than a reference count:
 *     with R38 = 7 the condition is  14/L38 >= 27/L39, i.e. L39 >= 1.929 * L38
 *     measured 60 against 44, a ratio of 1.36.
 * Both live ranges are already minimal in source terms -- p38 is a parameter,
 * born at assign_parms' copy and used in every arm of the chain; p39 is born at
 * the first read and dies at the `& 0x200` test.  NAMING THE MISSING QUANTITY IS
 * THE RESULT: no source edit found in four batches moves either length.
 *
 * ===================== ROUTE B IS REFUTED =====================
 *
 * The old park's ROUTE B wanted a manufactured conflict between p39 and r0.
 * `.18.greg` says p39's hard conflicts are {2, 3} -- the r2 and r3 parameter
 * copies still pending at its birth; r0 and r1 are ALREADY DEAD there, so there
 * is nothing to conflict with.  No source form moves a pseudo's birth ahead of
 * assign_parms' copies.  ROUTE B also rested on the ROM's `ldr r6,=gKeyRepeat`
 * being the first body insn: that is a sched2 CONSEQUENCE, and the instrument
 * reproduces it the instant allocation flips, so it was never a cause.
 *
 * THE TWO ONE-PIN ROUTES STILL STAND, both 0 of 163 at 163/163 (pass 3, owner
 * decision 3 -- prefer a pin-free body):
 *   1. `register short *d __asm__("r0")` from a renamed 4th parameter.
 *   2. `register volatile unsigned int *k __asm__("r6"); k = &gKeyRepeat;`
 *      with all eight tests through `*k`.  The better pass-3 candidate: r6 is
 *      callee-saved and the ROM genuinely keeps the pointer there throughout.
 *
 * ===================== DIMENSIONS CLOSED BY MEASUREMENT (328) ===============
 * The landed module-mate src/rom_15000/rom_23178_a_c_b.c (Debug_WarpMenu) is
 * this function's ONLY caller and supplies its real prototype:
 *     extern short Debug_WarpMenu_UI(int box, int map, short *cur, short *sel);
 * so a = a UI box handle, b = the map id, c = &cur, d = &sel.  Three facts the
 * module-mate suggested were tested and are INERT or REFUTED:
 *   `int b` + internal narrowing (the module-mate's extern says `int map`)  17
 *        -- and the reference's own `lsl r1,#16` entry narrowing shows either
 *           spelling compiles the same, so `short b` is not wrong either.
 *   `void *a` instead of `int a` (it is a box handle)                      17
 *   drop `volatile` on gKeyRepeat (the module-mate's gKeyHeld IS plain)
 *        162 differing at 147 instructions -- gcse commons the eight reads.
 *        VOLATILE IS REQUIRED.  Positive confirmation, now measured.
 *   `gKeyRepeat & 0xc0` in place of the `||`   143 at 157 instructions --
 *        the `||` really is two separate reads.  Positive confirmation.
 *   do-while(0) around `*d ^= 1` to get loop_depth's x2 on the reference
 *        119 at 164 instructions -- the loop notes are NOT free here, so the
 *        REG_N_REFS loop weight is not reachable at zero cost in this function.
 *   `*c = *d` in the 0x100 arm ONLY / the 0x200 arm ONLY   123 / 34
 *        -- the old park crossed BOTH arms at once (125) and never one at a
 *           time.  Both worse; that cell is now closed.
 *
 * STILL TRUE FROM THE OLD PARK, not re-measured here: the HImode pool facts
 * (arm.md:4318 `*thumb_movhi_insn` constrains operand 1 as "l,mn,l,*h,*r,I", so
 * alternative 1's `n` matches any const_int before alternative 5's `I` and the
 * 8-bit `mov` is UNREACHABLE for a HImode const_int -- the internal control is
 * in this very function, `gKeyRepeat & 1` giving `mov r2,#1` in SImode four
 * instructions from the HImode `*d ^= 1` giving `ldr r2,.L290f8`); the wrap
 * fixups must be written as -0x63/+0x63 and not 0x59; `mov r2,#0 / ldrsh
 * r3,[r0,r2]` is the tell that `d` is `short *`; and no flag closes it.
 *
 * No pins, no barriers, no .equ, no device in the body below.
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
