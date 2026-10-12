/* ===================== BATCH 342 (brief E, the DEPIN brief) ADDENDUM -- READ FIRST
 *
 * OvlFunc_885_200950c -- STILL NON-MATCHING.  BODY REPLACED WITH THE PIN-FREE
 * ONE under the owner's PREFER-PIN-FREE policy.  The park's 20-pin body is NOT
 * lost; it is in the tree's history and reproduced exactly by the ladder below.
 *
 * THE FIGURE, IN STREAM TERMS, FOR THE BODY THAT IS IN THIS FILE:
 *   187 of 239 stream encodings as built; 180/180 16-bit instructions (IN PHASE,
 *   so a TRUE DISTANCE), 59/59 32-bit entries, size exact, RELOCATIONS DIFFER.
 *   THE BODY IN THIS FILE IS THE PLAIN PIN-FREE ONE, measured AS BUILT, so the
 *   recipe below reproduces 187 with no flag and parkcheck can read it.
 *   A BETTER PIN-FREE FIGURE EXISTS BEHIND A FLAG: 26 of 239, 180/180, also a
 *   true distance, with the 0x808 arm written as four named `int` locals and
 *   CSE_CFLAGS (-fno-rerun-cse-after-loop) on.  That body is at
 *   scratch_elev/b342/E/v1c/b_locals.c and is 220 as built, which is why it is
 *   NOT the installed one -- see the flag-control warning below.
 *
 * Verify with (the installed path, the body as it stands, no flag):
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/ovl_78603c/200950c.c \
 *     asm/overlays/rom_78603c/ovl_30_c_c_a_c_a_c_c_a.s --func OvlFunc_885_200950c
 *
 * *** THE 20-PIN FIGURE OF 2 IS REPRODUCED, AND TWO OF THE 20 PINS ARE DEAD. ***
 *
 *   pins  body                                        flags        ndiff  insns
 *   ----  ------------------------------------------  -----------  -----  -------
 *    20   the previous park, verbatim                 as built       2    180/180
 *    19   that minus the SECOND PIN1 block            as built       2    180/180
 *    18   that minus BOTH PIN1 blocks                 CSE_CFLAGS     2    180/180
 *    15   a808 + a87a only                            CSE_CFLAGS     4    180/180
 *    15   spb + a808 only                             CSE_CFLAGS     5    180/180
 *    12   a808 only                                   CSE_CFLAGS     7    180/180
 *     0   4 named locals in the 0x808 arm (v1c/b_locals) CSE_CFLAGS    26    180/180
 *     0   that same body                              as built     220    180/181
 *     0   pin-free, the 0x808 arm written plainly     CSE_CFLAGS    45    180/179
 *     0   THIS BODY -- pin-free, 0x808 arm plain      as built     187    180/180
 *
 *   All five `2` and `4`..`7` rows have RELOCATIONS CLEAN and dsize 0.
 *   The 19-pin and 18-pin bodies live at
 *     scratch_elev/b342/E/v1b/pins19_flag1-spb-a808-a87a.c   (2, as built)
 *     scratch_elev/b342/E/v1/pins18_spb-a808-a87a.c          (2, CSE_CFLAGS)
 *   and both are STRICT IMPROVEMENTS on the 20-pin body: same figure, same
 *   instruction count, clean relocations, fewer pins.
 *
 * ONE PIN WAS DOING NOTHING.  The park pins the flag id at BOTH
 * `__GetFlag(0x109)` sites and its ladder credits "BOTH" with 222 -> 162.  Only
 * the FIRST site does any work: pinning it alone is worth 187 -> 45, pinning
 * the SECOND alone is INERT at 187, and dropping the second from the full body
 * leaves 2.  CORRECTED.
 *
 * *** THE PARK'S FLAG COLUMN IS NOT A VALID CONTROL, AND THIS IS THE POINT OF
 * *** THE WHOLE ROUND.  The park records `-fno-rerun-cse-after-loop` as 2 --
 * *** INERT.  That is true OF THE PINNED BODY ONLY.  On the pin-free body the
 * *** same flag is worth 142 ENCODINGS (187 -> 45).  The pin and the flag do the
 * *** same job -- they stop rerun-CSE parking the pool-loaded 0x109 in a
 * *** callee-saved register and copying it out, which also evicts the gState
 * *** pointer -- so with the pin in, the flag has nothing left to do and reads
 * *** inert.  REMOVING THE PIN FIRST AND THEN SWEEPING IS A DIFFERENT SEARCH
 * *** SPACE, NOT A HANDICAPPED ONE.  Same inversion on the source axis: four
 * *** named `int` locals in the 0x808 arm are 220 as built (WORSE than 187, and
 * *** one instruction long) and 26 under the flag (BETTER than 45, back in
 * *** phase).  Neither axis alone finds either lever.
 *
 * WHAT EACH SURVIVING PIN GROUP BUYS, from the pin-free + CSE_CFLAGS baseline:
 *   a808 (12 pins, the 0x808 arm's 2 SetPos + 2 Func_8092a1c)   45 -> 7
 *   spb  (3 pins, SetPos(0xb, 0xa6<<15, 0x1090000))             7 -> 2 crossed
 *   a87a (3 pins, SetPos(0x10, 0x84<<16, 0x84<<17))             7 -> 4 crossed
 * The a808 arm is the whole pin cost.  The ROM REBUILDS `188<<17` at each of the
 * three places it needs it -- twice inside ONE call -- and gcc commons it into a
 * callee-saved register and copies with `adds r1, r5, #0`.  Hex literals, the
 * script pointer loaded first, the second call's y through its own shifted
 * local, and `do { } while (0)` anchors between all four calls are 45, 45, 45
 * and 41.  Four named locals get the instruction count back but keep the copies.
 * `cse.c:5742-5745` tests `GET_CODE (PATTERN (insn)) == ASM_OPERANDS`, so the
 * only thing that flushes a cse equivalence is an `__asm__ ("" :: "i"(0))` --
 * a DEVICE, not a spelling.  No source form reaches it; that is what the PIN3
 * blocks are for.
 *
 * THE FLOOR IS 2 AND IT IS PIN-INDEPENDENT, SO NOTHING HERE CAN LAND.  The two
 * encodings are `__Func_8010704` SITE 5 OF 6: the ROM emits
 * `mov r2, r8 / movs r5, #20 / str r2, [sp, #0]` and gcc emits
 * `mov r2, r8 / str r2, [sp, #0] / movs r5, #20` -- the sixth argument's `mov`
 * before the fifth's `str`.  It survives all 20 pins.  The park's blocker class
 * (ORDERING at expand-time stack-argument emission) is REPRODUCED.
 *
 * AUTOSWEEP, THIS BATCH, on the 20-pin body: flag axis -- `-ffixed-r7`,
 * `-fno-strict-aliasing`, `-fno-strength-reduce`, `-fno-rerun-cse-after-loop`,
 * `-fno-rerun-loop-opt` all 2 (inert); `-fno-schedule-insns2` 70; `-fno-gcse`
 * 230.  Anchor axis -- 40 of 78 boundaries, all inert at 2; **CAPPED, so
 * boundaries 40..77 are UNTESTED, not inert.**
 * Flag PAIRS on the pin-free body, all crossed with `-fno-rerun-cse-after-loop`:
 * `-ffixed-r7` / `-fno-strength-reduce` / `-fno-rerun-loop-opt` /
 * `-fno-cse-follow-jumps` / `-fno-expensive-optimizations` all 45 (inert);
 * `-fno-schedule-insns2` 93, `-O1` 95, `-fno-gcse` 225.
 *
 * SPLIT SHAPE, unchanged: `asmfacts.py` says WHOLE, one `.thumb_func_start`, no
 * data, no exports, and overlays/rom_78603c/overlay.ld:28 keeps its asm/ path.
 * Harness: scratch_elev/b342/E (e342_sweep.py, e342_flagcross.py, e342_diff.py,
 * e342_gen_t1.py; v1/ v1b/ v1c/ hold every variant measured above).
 * =====================================================================================
 */
/* OvlFunc_885_200950c -- NON-MATCHING, 2 differing STREAM encodings of 239.
 *
 * BODY RECOVERED FROM scratch/ (batch 338).  This park was PROSE -- a header,
 * a diagnosis, and no compilable body -- so it had no figure, was NO CLAIM in
 * parkcheck, and was unrankable in every band.  But its own header named a
 * candidate at scratch_elev/b256/rich/final/ovl_30_c_c_a_c_a_c_c_a.c, that file still existed, and it
 * compiles.  The work had been done and left in a GITIGNORED directory instead
 * of installed, so nothing in the tree could see it.
 *
 * ref 239 / ours 239 stream encodings; 180/180 16-bit instructions -- IN PHASE, so a true DISTANCE.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/ovl_78603c/200950c.c \
 *     asm/overlays/rom_78603c/ovl_30_c_c_a_c_a_c_c_a.s --func OvlFunc_885_200950c
 *
 * 70 of the 133 prose parks were in this state; tools/scratchfill.py found them.
 * THE FIGURE IS EVIDENCE; the prose below it is a HYPOTHESIS, and across pass
 * two a park's stated blocker has been wrong roughly 40 times in 42.  The
 * original park text is preserved underneath.
 */
// fakematch
/* ovl_30_c_c_a_c_a_c_c_a.c  --  OvlFunc_885_200950c
 *   [the WHOLE of asm/overlays/rom_78603c/ovl_30_c_c_a_c_a_c_c_a.s -- ONE
 *    `.thumb_func_start`, asmfacts.py says `WHOLE / convert directly`, so NO
 *    split and overlays/rom_78603c/overlay.ld:28 stays VERBATIM as
 *    `asm/overlays/rom_78603c/ovl_30_c_c_a_c_a_c_c_a.o(.text)`]
 *
 *   makefile_flags() on this path is the EMPTY set and WILDCARD_HITS is empty:
 *   the rule that fires is the generic `asm/%.o: src/%.c`.  The .s carries NO
 *   .section, .data, .bss, .lcomm, .word or .byte, and all ten `.L` symbols are
 *   branch targets defined in this file, so nothing needs exporting.
 *
 *   PARK -- 2 of 239 encodings.  SIZE SILENT, RELOCATIONS SILENT, so by the
 *   recorded discriminator this is an ORDERING residue, not a CSE loss.  The
 *   two are one adjacent transposition at one of six __Func_8010704 sites.
 *
 * THE LADDER
 *
 *     plain C                                                222 (+3 insns)
 *     + the 0x109 flag id pinned to r0 at BOTH __GetFlag      162 (+2 insns)
 *     + the strh constant taken through an `int` local         45 (exact size)
 *     + five pin blocks (one SetPos, the 0x808 arm, 0x87a)      2
 *
 * TWO FINDINGS, one of them new.
 *
 * 1. A `strh` OF A LITERAL POOL-LOADS IT AND DRAGS THE WHOLE POOL FORWARD.
 *    NEW.  `*(short *)(r + 0x20) = 0xc;` compiles to `ldrh r3, .L13` with
 *    `.word 12` in the pool -- gcc has no HImode immediate move on Thumb, so a
 *    2-byte store of a constant becomes a POOL LOAD even for a value that fits
 *    in `mov`.  Because a PC-relative halfword load has a far shorter reach
 *    than `ldr`, gcc then DUMPS THE WHOLE CONSTANT POOL IN THE MIDDLE OF THE
 *    FUNCTION behind a `b` -- a branch-over-pool the ROM does not have, and
 *    every later `ldr [pc, #N]` in the function reads a different offset.  The
 *    symptom is therefore enormous (117 encodings here) and the cause is one
 *    literal.  THE FIX IS AN `int` LOCAL: `n = 0xc; *(short *)(r + 0x20) = n;`
 *    gives `mov r3, #12 / strh r3, [r0, #32]` and the pool goes back to the
 *    end.  DISCRIMINATOR, cheap to check on any function: an `ldrh rX, .Ln`
 *    or a `.word` below 256 in a MID-FUNCTION pool, against a ROM whose pool
 *    is at the end.
 *
 * 2. THE FLAG-ID PIN, AND WHY IT IS WORTH 60.  0x109 is read twice
 *    (`__GetFlag(0x109)` in the opening guard and again inside the else arm).
 *    Unpinned, rerun-CSE parks it in r5 and copies it (`ldr r5,[pc] /
 *    adds r0,r5,#0`), which ALSO evicts the gState pointer from r5 into r6 --
 *    the ROM's `add r5, r3, r2` becomes `add r6, r3, r2` and the whole
 *    function renumbers.  `{ PIN1; q0 = 0x109; if (__GetFlag(q0) ... ) }` at
 *    BOTH sites is the sibling ovl_30_c_c_c_c_a_a_a_c_c_c_a.c's recorded
 *    "an added push holding a commoned constant is a FLAG tell", here
 *    presenting not as an added push but as a DISPLACED POINTER.
 *
 * THE OPENING RANGE TEST IS `(unsigned short)(v - 5) <= 1`.  The ROM reads the
 * area id at gState+0x1c2 with `ldrh`, subtracts 5, shifts LEFT 16 and compares
 * unsigned against 0x10000: that is gcc's own encoding of a 16-bit `<= 1`,
 * with the constant pre-shifted so no `lsr` is needed.  The same halfword is
 * read again later with `ldrsh` through the same r5, so the source keeps ONE
 * `short *` local built with the neighbour's rebuild idiom
 * (`r3 = (unsigned int)&gState; r2 = 0xe1; r2 <<= 1; r3 += r2;`).
 *
 * THE SCRIPT POINTER IS A LOCAL AND ITS TWO SITES ARE PINNED.  The ROM loads
 * gScript_885__02009ce0 ONCE into r5 and copies it (`mov r2, r5`) at both
 * __Func_8092a1c calls while REBUILDING 0x80<<9 at each.  `sc = gScript...;`
 * plus `{ PIN3; q0; q1; q2 = (int)sc; }` gives exactly that.  This is the
 * "symbol the ROM loads once and copies" rule and the "pin the shifted
 * constant" rule applying to the SAME call site, in opposite directions.
 *
 * THE PARK: ONE TRANSPOSITION AT __Func_8010704 SITE 5 OF 6.
 *
 *     ROM   mov r2, r8 / mov r5, #0x14 / str r2, [sp] / mov r0,#9 / ...
 *     ours  mov r2, r8 / str r2, [sp]  / mov r5, #20  / mov r0,#9 / ...
 *
 * The six calls are (0xe,0x15) (0xf,0x15) (0x17,0x13) (0x18,0x13) (0x17,0x14)
 * (0x18,0x14); gcc commons 0x17 into r8, 0x18 into r6 and each row constant
 * into r5 exactly as the ROM does, and FIVE of the six sites are byte-exact.
 * At site 5 the ROM's scheduler materialises the SIXTH argument before storing
 * the FIFTH and ours does not.  `-fno-schedule-insns2` REGRESSES the function
 * (2 -> 70), so by the recorded sign rule sched2 is already right and alias is
 * the wrong axis; the difference is in the PRE-SCHEDULE order that expand
 * emits, which no argument spelling reached.
 *
 * MEASURED AGAINST IT, all 2 unless stated:
 *   - the stack-argument pin in every register the site could use:
 *     s0=r2/s1=r5 (227, and 8 bytes SHORT), s0=r3/s1=r2 (227), s1=r5 alone
 *     (125), s0 alone in r0/r1/r2/r3 (228).  Every one destroys the `mov r2,r8`
 *     read of the commoned 0x17, which is why the recorded stack-argument pin
 *     -- right on the sibling ovl_30_c_a_c_c_c_a_a.c -- is WRONG HERE.  NEW,
 *     and the discriminator is whether the stack argument is REBUILT (pin it)
 *     or COPIED OUT OF A COMMONED REGISTER (do not).
 *   - `do { } while (0)` before site 4 (4), before site 5 (4), after site 5
 *     (9): EVERY BARRIER IS WORSE, which is the recorded "the barrier is only
 *     available where the ROM does not use r8-r11" -- this function's hi count
 *     is 4 -- holding on a SEVENTH function.
 *   - PIN4 on the four register arguments at site 5 (13), site 6 (31), all six
 *     sites (32); at site 4 alone it is INERT at 2.
 *   - naming 0x17 or 0x14 in a local, before the run or between the calls;
 *     `(e2 = 0x14, 0x17)` and `0x17, e2 = 0x14` comma forms; `0xa + 0xa` and
 *     `(int)0x14` for the constant: all 2, all inert.
 *   - four prototypes for __Func_8010704 -- `unsigned int` tail, `unsigned
 *     char` tail, no prototype at all: all 2.  A seventh parameter is 177.
 *   - restructuring the 0x834 guard from early-return to if/else: INERT.
 *   - seven flag combinations: -fno-gcse (230, and 2 insns long -- gcse is
 *     LOAD-BEARING here, the opposite of the sibling file in this batch),
 *     -fno-schedule-insns2 (70), -fno-rerun-cse-after-loop,
 *     -fno-expensive-optimizations, -fno-cse-follow-jumps, -fno-peephole2,
 *     -fno-strength-reduce: the last five all 2.
 *
 * BLOCKER CLASS: ORDERING, at the expand-time stack-argument emission order.
 * DISCRIMINATOR: a six-argument call whose FIFTH argument is a copy out of a
 * commoned high register -- the ROM interleaves the sixth argument's `mov`
 * before the fifth's `str`, and every device that would express that
 * (register pin, barrier, named local, comma expression) either is inert or
 * destroys the commoning.  FLOOR: 2 of 239 encodings, SIZE and RELOCATIONS
 * both silent.
 *
 * Harness: scratch_elev/b256/rich/t2 -- b1.c (this body), b2.c (the if/else
 * restructure), sw*.py (the sweeps above); the shared tools are one directory
 * up (objcmp_all.py, objcmpf.py, cmp2.py, dis.py).
 */
struct A59 {
    unsigned char pad00[0x59];
    unsigned char f59;
};

typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern int __GetFlag(int id);
extern void __ClearFlag(int id);
extern void __StartRain(void);
extern void __StartThunder(void);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_SetBehavior(int slot, void *s);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __Func_8092a1c(int a, int b, unsigned char *c);
extern void OvlFunc_885_2008964(void);
extern unsigned char gOvl_02009c34[];
extern unsigned char gScript_885__02009ce0[];

int OvlFunc_885_200950c(void)
{
    unsigned char *r;
    short *p;
    unsigned int r3;
    unsigned int r2;
    int n;
    unsigned char *sc;

    r3 = (unsigned int)&gState;
    r2 = 0xe1;
    r2 <<= 1;
    r3 += r2;
    p = (short *)r3;
    if ((unsigned short)(*p - 5) <= 1)
        __ClearFlag(0x12f);
    if (__GetFlag(0x109) != 0)
        __ClearFlag(0x242);
    if (__GetFlag(0x834) != 0) {
        __StartRain();
        __StartThunder();
        __CutsceneStart();
        __MapActor_SetPos(0xc, 0, 0);
        __MapActor_SetPos(0xd, 0, 0);
        __MapActor_SetPos(0xe, 0, 0);
        __MapActor_SetPos(0xf, 0, 0);
        __MapActor_SetPos(5, 0, 0);
        ((struct A59 *)__MapActor_GetActor(8))->f59 |= 8;
        __MapActor_SetPos(0xb, 0xa6 << 15, 0x1090000);
        __Func_80921c4(0xb, 0x53, 0x111);
        __MapActor_SetAnim(0xb, 5);
        r = __MapActor_GetActor(0xb);
        n = 0xc;
        *(short *)(r + 0x20) = n;
        __MapActor_SetBehavior(0xb, gOvl_02009c34);
        if (__GetFlag(0x839) != 0)
            __MapActor_SetPos(0xb, 0, 0);
        __CutsceneEnd();
        __Func_8010704(9, 0x18, 1, 1, 0xe, 0x15);
        __Func_8010704(9, 0x18, 1, 1, 0xf, 0x15);
        __Func_8010704(9, 0x18, 1, 1, 0x17, 0x13);
        __Func_8010704(9, 0x18, 1, 1, 0x18, 0x13);
        __Func_8010704(9, 0x18, 1, 1, 0x17, 0x14);
        __Func_8010704(9, 0x18, 1, 1, 0x18, 0x14);
        return 0;
    }
    __MapActor_SetPos(9, 0, 0);
    __MapActor_SetPos(0xa, 0, 0);
    __MapActor_SetPos(1, 0, 0);
    __MapActor_SetPos(0xb, 0, 0);
    __CutsceneStart();
    if (__GetFlag(0x109) == 0 && *p == 0xa)
        OvlFunc_885_2008964();
    if (__GetFlag(0x801) != 0) {
        __MapActor_SetPos(0xd, 0, 0);
        __MapActor_SetPos(0xe, 0, 0);
        __MapActor_SetPos(0xf, 0, 0);
    } else if (__GetFlag(0x808) != 0) {
        __MapActor_SetPos(0xe, 0xc4 << 17, 0xbc << 17);
        __MapActor_SetPos(0xf, 0xbc << 17, 0xbc << 17);
        sc = gScript_885__02009ce0;
        __Func_8092a1c(0xe, 0x80 << 9, sc);
        __Func_8092a1c(0xf, 0x80 << 9, sc);
    }
    if (__GetFlag(0x87a) != 0)
        __MapActor_SetPos(0x10, 0x84 << 16, 0x84 << 17);
    __CutsceneEnd();
    return 0;
}

#if 0
/* ===== ORIGINAL PROSE PARK, PRESERVED =====
(* OvlFunc_885_200950c -- 0x0200950c, asm/overlays/rom_78603c/ovl_30_c_c_a_c_a_c_c_a.s
 *
 * TWO differing encodings of 239, with SIZE AND RELOCATIONS BOTH SILENT --
 * squarely the recorded ordering band. Candidate:
 * scratch_elev/b256/rich/final/ovl_30_c_c_a_c_a_c_c_a.c.
 *
 * BLOCKER CLASS: ORDERING AT EXPAND-TIME STACK-ARGUMENT EMISSION. At ONE of
 * six `__Func_8010704` sites the ROM materialises argument 6 before storing
 * argument 5. The other five sites are exact under the same spelling, which is
 * what makes this a site property rather than a callee property.
 *
 * ALIAS IS THE WRONG AXIS: `-fno-schedule-insns2` REGRESSES, 2 -> 70, so
 * sched2 is already producing the ROM's order.
 *
 * REFUTED, with measurements in the scratch NOTES.md: every pin width and
 * register at the site; 3 barrier placements; 4 prototypes; comma expressions;
 * named locals; 7 flags; an if/else restructure.
 *
 * WHAT IS WORTH TRYING NEXT, and was not: the stack-argument pin has a
 * discriminator found on this function's sibling -- it is exact where the ROM
 * REBUILDS both stack arguments and catastrophic (227) where the ROM COPIES
 * one out of a commoned high register. Check which case this site is before
 * reaching for a pin again.
 *
 * LANDING SHAPE IF CLOSED: src/overlays/rom_78603c/ovl_30_c_c_a_c_a_c_c_a.c.
 * `asmfacts.py` says WHOLE -- convert directly. One function, no data, no
 * split. `makefile_flags()` is empty with no wildcard hazard, and
 * overlays/rom_78603c/overlay.ld:28 MUST KEEP its asm/ path. No cross-file
 * `.L` symbol is referenced.
 *)

*/
#endif
