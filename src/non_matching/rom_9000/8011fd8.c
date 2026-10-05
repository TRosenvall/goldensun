/* Func_8011fd8 -- asm/rom_9000/rom_11ce0_c_a_a.s
 *
 * NON-MATCHING, 30 of 45 encodings, NO PINS  (MEASURED, batch 324 brief H).
 * Replaces the installed park body, which measures 32 of 45.
 * SIZE EQUAL: 96 bytes against 96; 42 real instructions against 42, plus the
 * same three pool words in the same order.  So the figure IS a distance.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_9000/8011fd8.c \
 *     asm/rom_9000/rom_11ce0_c_a_a.s --func Func_8011fd8
 *
 * SPLIT SHAPE, for whenever this lands: the `.s` holds TWO functions,
 * Func_8011f54 then Func_8011fd8, so a split is needed either way.
 *
 * WHAT CHANGED, AND WHY THE PARK'S DIAGNOSIS WAS WRONG.
 *
 * The park called the blocker "register rotation / birth order" and closed with
 * "NOT TRIED: nothing further.  The base is the first statement in the C
 * already; gcc reorders it behind the parameter saves regardless."  Birth order
 * is the SYMPTOM.  The cause is ALLOCATION PRIORITY, and priority is reachable
 * from the C.  The ROM's whole register map -- base->r5, sel->r6, tbl->r0,
 * x->r1, y->r4 -- now reproduces with no pins, from two edits:
 *
 *   1. ADVANCE THE POINTER: `tbl += ((y << 7) + x) * 4;` then `tbl[3]`,
 *      instead of `tbl[((y << 7) + x) * 4 + 3]`.
 *   2. `y = py >> 16;` BEFORE `x = px >> 16;`.
 *
 * Singly they read 31 and 31; crossed they read 30.
 *
 * MECHANISM, read out of the compiler.  `global.c:1014-1016` builds find_reg's
 * pass-0 candidate set as `used1 | ~regs_used_so_far | regs_someone_prefers`,
 * and REG_ALLOC_ORDER (config/arm/arm.h:989) is `3, 2, 1, 0, 12, 14, 4, 5, ...`.
 * In the park's compile, `.18.greg` reads
 *
 *     ;; 7 regs to allocate: 50 52 38 37 35 32 36
 *     ;; 35 conflicts: ... 1 2 3 13     (35 = base, NO hard conflict with r0)
 *     ;; 32 preferences: 0              (32 = sel, arrives in r0)
 *     dispositions: 35 in 0, 32 in 5, 36 in 6
 *
 * base, sel and tbl all have three references, so `floor_log2(n)*n*size` ties at
 * 3 and `allocno_compare`'s `live_length` denominator (global.c:607) puts the
 * shortest-lived -- base -- first.  Pass 0 then excludes r0 from base because
 * sel prefers it, pass 0 fails, pass 1 DROPS regs_someone_prefers, walks
 * `3, 2, 1, 0` past base's r1/r2/r3 conflicts and hands base r0.  Advancing
 * `tbl` gives it a FOURTH reference -- floor_log2(4)*4 = 8 against 3 -- so tbl
 * is now allocated first, takes r0, and base is pushed out to r5.  Edit 1 also
 * reproduces the ROM's `add r0, r3` + `ldrb r3, [r0, #3]` pair directly, which
 * is what suggested that the original advanced the pointer.
 *
 * `register unsigned char *tbl __asm__("r0")` on the pre-edit body reads the
 * same 30 and the same map -- kept here as a DEVICE-derived figure about the
 * blocker only; the body below is pin-free and does not need it.
 *
 * THE REMAINING 30 -- FOUR INDEPENDENT CAUSES THAT CANCEL IN THE COUNT.
 * (42 instructions against 42 is therefore not evidence of anything.)
 *
 *   1. rom  `mov r4, r2` + `asr r4, #16`      ours  `asr r4, r2, #16`
 *      The ROM keeps the `py` parameter copy.  NOT combine: `.00.rtl` has the
 *      copy at insn 8 and the shift at insn 24, and combine.c:1128-1144 refuses
 *      a non-adjacent hard-reg substitution under SMALL_REGISTER_CLASSES
 *      (= TARGET_THUMB, arm.h:1061).  What happens instead is that `py`'s pseudo
 *      34 is a LOCAL quantity and local-alloc's copy suggestion gives it r2,
 *      so the copy degenerates to a deleted `mov r2, r2`.  The ROM instead ties
 *      `py` to `y` in r4 and keeps a real `mov r4, r2`.           -1 insn here
 *   2. rom  `ldr r0, [r5, r3]`                ours  `add r3,r3,r5` + `ldr r0,[r3]`
 *      The address is `(plus base (plus X 0x130))`; fold reassociates it to
 *      `(plus (plus base X) 0x130)` and 0x130 exceeds the Thumb SImode `ldr`
 *      displacement, forcing the base add into the chain.  SOURCE-UNREACHABLE,
 *      verified: parenthesising, reordering the terms and `&base[...]` each
 *      produce BIT-IDENTICAL assembly.                             +1 insn here
 *   3. rom divides x IN PLACE -- `cmp r1,#0 / bge / add r1,#0xf / asr r1,#4`,
 *      the `+15` temp coalesced onto x -- where ours copies first,
 *      `mov r3,r1 / cmp r1,#0 / bge / add r3,#15 / asr r1,r3,#4`.  +1 insn here
 *      The ROM's *y* division does keep its copy and COMPARES THE COPY
 *      (`mov r2,r4 / cmp r2,#0`) where ours compares the original
 *      (`cmp r4,#0`): same count, so this is one cause with two faces.
 *   4. rom  `add r3, r2` + `ldrb r3, [r3]`    ours  `ldrb r3, [r2, r3]`
 *      The exact mirror of (2) -- gcc picks register-offset where the ROM adds,
 *      and adds where the ROM register-offsets.                    -1 insn here
 *
 * MEASURED INERT ON THIS BODY (crossfire, depth 2, memory profile clean):
 *   0x130 written before the product                   30
 *   offset via `&base[...]`                            30
 *   `off` named inside the `if`                        30  (INSNS flag)
 *   divide x before y                                  30
 *   an `i` declared for the tile index                 30
 * MEASURED WORSE / WRONG:
 *   `x = x >> 4` for `x / 16`      41 insns, RELOC+COUNT -- drops the sign
 *                                  correction, a WRONG PROGRAM
 *   material byte through a named `unsigned char *m`   34, and it changes the
 *                                  push set to `push {r5, lr}`
 *   if/else instead of pre-initialising tbl            47 insns, 46
 *
 * NEXT MOVE: causes 1 and 3 are both about where the `+15` / parameter copies
 * get coalesced, so they are one allocator question, not two.  Cause 2 is
 * closed (source-unreachable).  Cause 4 is open and untried.
 */
extern int iwram_3001e70;
extern unsigned char gBuffer[];
extern unsigned char ewram_202c000[];

int Func_8011fd8(int sel, int px, int py)
{
    char *base;
    unsigned char *tbl;
    int x;
    int y;

    base = (char *)iwram_3001e70;
    y = py >> 16;
    x = px >> 16;
    tbl = gBuffer;
    if (base != 0)
        tbl = *(unsigned char **)(base + (sel & 3) * 48 + 0x98 * 2);
    x = x / 16;
    y = y / 16;
    tbl += ((y << 7) + x) * 4;
    return ewram_202c000[tbl[3] * 4] & 0xf;
}
