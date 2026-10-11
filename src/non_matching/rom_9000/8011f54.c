/* Func_8011f54 (0x08011f54) -- NON-MATCHING.
 *
 * NON-MATCHING, 59 of 60 encodings  (MEASURED, batch 319 recipe backfill).
 *   COUNT DIFFERS (ref 60, ours 55) -- so this positional figure measures
 *   MISALIGNMENT, not distance.  Read the count before the figure.
 *   *** RELOCATIONS DIFFER IN THEIR SYMBOLS, NOT ONLY THEIR OFFSETS --
 *   gBuffer
 *   SO THIS FIGURE IS NOT A DISTANCE: `make compare` cannot pass a
 *   relocation difference.  Fix this before trusting the encoding count. ***
 *   SIZE ref 132 bytes, ours 120.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_9000/8011f54.c \
 *     asm/rom_9000/rom_11ce0_c_a_a.s --func Func_8011f54
 *
 * This recipe was ADDED by the batch-319 backfill: the park had none, so
 * parkcheck.py could not report its figure and nothing had ever checked it.
 * Blocker class: instruction selection -- gcc COMBINES a copy and a shift that
 * the original build kept separate.
 *
 * 53 lines against the ROM's 57. The body is right: every instruction from the
 * table lookup through the indirect call appears in the ROM's order, and the
 * push list matches. Four lines are missing, and two of them are this, twice:
 *
 *     rom    mov r5, r1 / ... / asr r5, #0x10
 *     ours   asr r5, r1, #0x10
 *
 * The parameter is copied into a callee-saved register and shifted IN PLACE.
 * gcc-2.96 has the three-operand `asr rd, rs, #imm` encoding and uses it, so
 * the copy never becomes its own insn. Writing `x = fx; x >>= 16;` as two
 * statements is byte-identical -- the fold happens well before RTL.
 *
 * MEASURED (rom 57 lines):
 *   baseline                                             55, 55
 *   `x = fx; x >>= 16;` as two statements                55, 55 (inert)
 *   named table offset for the reg+reg load              53, 55
 *   named pointer for the ewram_202c000 read             53, 55  <- kept
 *   -fno-gcse / -fno-strict-aliasing / -fno-strength-reduce /
 *     -fno-rerun-cse-after-loop / -fno-schedule-insns2   53, 55 (ALL inert)
 *
 * WHAT IS RIGHT, and is why this park is worth reading -- the file contains
 * BOTH halves of the batch-174 addressing discriminator, in one function, ten
 * instructions apart:
 *
 *   `ldr r2, [r1, r3]` for the table pointer -- base plus INDEX. Written with
 *   the offset inline, gcc computes the address first. Naming the offset
 *   (`off = ((a & 3) * 3 << 4) + (0x98 << 1); src = *(...)(base + off);`)
 *   gives the ROM's form.
 *
 *   `add r0, r1, r3 / ldrb r0, [r0, #0x0]` for the ewram read -- address
 *   COMPUTED first. Written as `ewram_202c000[t]` gcc gives reg+reg, the
 *   opposite. A named pointer (`q = ewram_202c000 + t; k = *q & 0xf;`) gives
 *   the ROM's form, AND fixes the push list from {r5, r6, lr} to
 *   {r5, r6, r7, lr}.
 *
 * So the discriminator is not a property of the function or the file -- the
 * same translation unit wants an index in one place and a pointer ten
 * instructions later, and the ROM tells you which each time. Read the load.
 *
 * Also right: `_call_via_r3` from an ordinary call through
 * `void (*L134fc[])(unsigned char *, int, int)`; the signed `/ 16` expansion
 * (`cmp / bge / add #0xf / asr #4`) from writing plain division; and the
 * `iwram_3001e70`-based table at `(a & 3) * 48 + 0x130`, the same table
 * OvlFunc_947_2008fcc uses.
 *
 * NEXT: nothing source-level. Five flag groups and two spellings of the shift
 * are inert.

 * ========================================================================
 * BATCH 336 BRIEF G -- FIFTY-NINE TO SEVENTEEN, AND THE PARKED BODY WAS A
 * WRONG PROGRAM.  THE TU-MATE Func_8011fd8 LANDED EXACT FROM THESE FINDINGS.
 * ========================================================================
 *
 * CURRENT FIGURE: 17 of 60 encodings in STREAM terms (ref 60, ours 60),
 * 53 instructions against 53, SIZE 132 against 132, RELOCATIONS IDENTICAL.
 * So the figure IS a distance.  Barrier-free the body below reads
 * TWENTY-SEVEN (ref 60, ours 58 -- two short, so that figure measures a
 * length gap).  Superseded figure: fifty-nine, which was never a distance.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_9000/8011f54.c \
 *     asm/rom_9000/rom_11ce0_c_a_a.s --func Func_8011f54
 *
 * *** THE PREVIOUS BODY WAS A WRONG PROGRAM AND THAT IS WHY ITS RELOCATIONS
 * *** DIFFERED IN THEIR SYMBOLS.  It read
 * ***
 * ***     if (base != 0)
 * ***         off = ((a & 3) * 3 << 4) + (0x98 << 1);
 * ***         src = *(unsigned char **)(base + off);
 * ***
 * *** with NO BRACES, so the `src = ...` was unconditional, `src = gBuffer`
 * *** was dead, and gcc dropped the gBuffer pool word entirely -- which is
 * *** exactly the "RELOCATIONS DIFFER IN THEIR SYMBOLS -- gBuffer" line at
 * *** the top of this header.  Adding the braces restores the symbol and all
 * *** six relocations now match in name and order.  The park diagnosed the
 * *** symptom for two batches and never read the two lines above it.
 *
 * THE DECISIVE EDIT, and it is the park's own refuted note.  The park wrote
 * "Writing `x = fx; x >>= 16;` as two statements is byte-identical -- the fold
 * happens well before RTL", and measured that inert.  True, and the wrong
 * variable: shift the PARAMETER in place.
 *
 *     fx >>= 16;  fy >>= 16;
 *
 * The parameter's own pseudo is the shift's destination, so the `mov rN, rParm`
 * that assign_parms emitted has nowhere to be propagated and survives as the
 * ROM's `mov r5, r1` + `asr r5, #16`.  A FRESH variable's pseudo is a copy of
 * the parameter's and gets propagated away, which is what the park measured.
 * One line: 59 -> 33 with the braces, and the first 33 encodings then align.
 *
 * SECOND EDIT: ADVANCE THE POINTER.  `src += ((fx / 16) + (fy / 16) * 128) * 4;`
 * then `t = src[3] * 4;` -- the same edit that was already in the TU-mate's
 * park, giving the ROM's destructive `add r2, r3` + `ldrb r1, [r2, #3]`.
 * 33 -> 27.
 *
 * THIRD EDIT: THE RETURN TYPE.  The ROM's epilogue is `pop {r5, r6, r7}` /
 * `pop {r1}` / `bx r1`.  `pop {r1}` encodes a 1-4 byte return (arm.c
 * `thumb_exit`), so this function is NOT `void` as the old body declared it;
 * it returns the result of the indirect call.  `.L134fc` is therefore an array
 * of `int (*)(unsigned char *, int, int)`.
 *
 * FOURTH EDIT: THE ADDRESS ADD, via combine.c:1182-1188.  The ROM reads the
 * material byte with `add r0, r1, r3` + `ldrb r0, [r0]`; gcc folds that to
 * `ldrb rX, [rY, rZ]`.  The full mechanism is written up in the TU-mate's
 * landed file src/rom_9000/rom_11ce0_c_a_a_b.c -- short version: the fold is
 * at EXPAND for an array or pointer spelling (explow.c `memory_address` ->
 * `break_out_memory_refs` -> THUMB_GO_IF_LEGITIMATE_ADDRESS's reg+reg clause,
 * arm.h:2133-2148) and at COMBINE for an integer-address spelling, and
 * `can_combine_p` refuses across a VOLATILE INSN.  A bare
 * `__asm__ volatile ("")` is NOT one (rtlanal.c `volatile_insn_p` returns 0
 * for ASM_INPUT); `__asm__ volatile ("" ::: "memory")` is.  27 -> 17, and the
 * stream lengths become equal.
 *
 * WHAT THE REMAINING 17 IS, and it is ONE allocator decision, not seventeen.
 * Fifteen of the 17 are a REGISTER PERMUTATION in the last basic block plus
 * the `push`/`pop` pair:
 *
 *   value                  ROM   ours
 *   src[3] / t             r1    r0
 *   ewram_202c000 + t      r0    r3
 *   the 0xf mask           r2    r1
 *   k                      r3    r2
 *   ewram_202c001 base     r7    r3
 *   .L134fc base           r4    r4
 *
 * The ROM spends EIGHT hard registers and pushes {r5, r6, r7, lr}; we spend
 * SEVEN and push {r5, r6, lr}.  `.18.greg` reads
 *     ;; 8 regs to allocate: 51 53 33 34 36 50 35 32
 *     dispositions: 51 in 3  53 in 3  33 in 5  34 in 6  36 in 2  50 in 1
 *                   35 in 1  32 in 0
 *     ;; Hard regs used:  0 1 2 3 4 5 6 14 25 26      <- no r7
 * and MAXIMUM SIMULTANEOUS LIVENESS IN THE TAIL IS SEVEN on both sides, so
 * the ROM's r7 is NOT pressure-forced -- it is an allocation-ORDER artifact,
 * and nothing in the source has been found that reproduces it.
 *
 * ONE LEVER MOVES IT AND BREAKS SOMETHING ELSE.  Naming the second table's
 * base early -- `d = ewram_202c001; ... L134fc[k](d + t, ...)` -- reads
 * TEN of 60.  It is not shippable as written because it REVERSES THE POOL
 * ORDER: the ROM's pool is iwram_3001e70, gBuffer, ewram_202c000, .L134fc,
 * ewram_202c001, and pool slots are filled in order of FIRST REFERENCE at
 * expand, so an early `ewram_202c001` puts it ahead of `.L134fc` and the
 * relocation symbols land at swapped offsets.  Naming the callee instead
 * (`fn = L134fc[k];`) fixes the pool order and costs the call register --
 * `_call_via_r4` against the ROM's `_call_via_r3` -- for 16.
 *   >> THE OPEN QUESTION IS EXACTLY THIS: get the ten WITHOUT referencing
 *   >> ewram_202c001 before .L134fc.  That is the whole park now.
 *
 * MEASURED INERT ON THE 17 BODY (every one 17 of 60):
 *   `unsigned` t / `unsigned` k / `unsigned long` ia
 *   `*(src + 3)` for `src[3]`;  `src[3] << 2` for `src[3] * 4`
 *   `((fy / 16) << 7)` for `(fy / 16) * 128`
 *   the advance split into `off = ...; src += off;`
 *   `char *base` for `unsigned char *base`
 *   `15` for `0xf`, in the mask and in both arguments
 *   `(0x98 << 1) + ...` and `(a & 3) * 48 + ...` orderings of `off`
 *   declaration order permuted three ways x four index spellings (12 rows)
 *   `t = src[3]; t *= 4;` split;  `&ewram_202c001[t]` for the argument
 *   `__asm__ volatile ("" ::: "memory")` placed before `t` as well
 * MEASURED INERT BARRIER-FREE (every one 27 of 60, ours 58):
 *   array index / named pointer / `&arr[t]` / integer address / two-step
 *   `q = arr; q += t;` / reversed `t + arr` / `(unsigned)t` / `const` cast
 *   -- TEN addressing spellings, all folded.  The form is SPELLING-PROOF.
 * MEASURED WORSE:
 *   naming `xo = fx & 0xf; yo = fy & 0xf;` before the call   26-35, worse
 *   `q[-1]` off the second table                             35
 *   `d = ewram_202c001 + t` named before the read            26
 *   both index expressions written out (no shared `t`)       55, 62 long
 *   bare `__asm__ volatile ("")` (not a combine barrier)     27, inert
 * A SECOND EXACT-FIGURE INSTRUMENT: `*(volatile unsigned char *)q` reads the
 * same 17 with no asm at all.  Not preferred -- owner-decisions.md ruling 4. */
extern unsigned char *iwram_3001e70;
extern unsigned char gBuffer[];
extern unsigned char ewram_202c000[];
extern unsigned char ewram_202c001[];
extern int (*L134fc[])(unsigned char *p, int x, int y) __asm__(".L134fc");

int Func_8011f54(int a, int fx, int fy)
{
    unsigned char *base;
    unsigned char *src;
    unsigned char *q;
    unsigned char *d;
    int t;
    int k;
    int off;

    base = iwram_3001e70;
    fx >>= 16;
    fy >>= 16;
    src = gBuffer;
    if (base != 0) {
        off = ((a & 3) * 3 << 4) + (0x98 << 1);
        src = *(unsigned char **)(base + off);
    }
    src += ((fx / 16) + (fy / 16) * 128) * 4;
    t = src[3] * 4;
    q = ewram_202c000 + t;
    __asm__ volatile ("" ::: "memory");
    k = *q & 0xf;
    return L134fc[k](ewram_202c001 + t, fx & 0xf, fy & 0xf);
}
