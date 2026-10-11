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

 * ========================================================================
 * BATCH 336 BRIEF G -- EXACT MATCH, 0 OF 45.  THIRTY TO ZERO, AND THE PARK'S
 * "SOURCE-UNREACHABLE" CAUSE 2 WAS REACHABLE FROM THE TU-MATE'S OWN PARK
 * ========================================================================
 *
 * CURRENT FIGURE: 0 of 45 encodings, 45 of 45 in STREAM terms, 96 bytes, 3
 * relocations identical.  Superseded figures: thirty, and before that
 * thirty-two.
 *
 * Verify with (INSTALLED path, after the split below):
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/rom_9000/rom_11ce0_c_a_a_b.c \
 *     asm/rom_9000/rom_11ce0_c_a_a_b.s --whole
 *
 * SPLIT SHAPE.  `tools/piece.py asm/rom_9000/rom_11ce0_c_a_a.s` reports TWO
 * functions, Func_8011f54 then Func_8011fd8, both parked.  Func_8011fd8 is the
 * LAST, so `tools/split_s.py asm/rom_9000/rom_11ce0_c_a_a.s Func_8011fd8`
 * writes `_a.s` (1 function, 70 lines -- Func_8011f54, stays assembly) and
 * `_b.s` (1 function, 52 lines -- this one, deleted once the .c lands) and
 * rewrites stage1.ld.  No `_c.s`.  `tools/datacheck.py` on the reference is
 * SILENT.  >> THE PAIR DOES NOT HAVE TO CONVERT TOGETHER.  The brief's
 * "each pair converts whole or not at all" is false for this piece, and
 * docs/brief-template.md now says so.
 *
 * FOUR CAUSES, FOUR EDITS.  The park's four-cause breakdown is EXACTLY RIGHT
 * as an observation and wrong only in its verdicts.  Causes 1 and 3 fell to one
 * edit, cause 2 to the TU-MATE'S OWN PARK, cause 4 to a combine barrier.
 *
 *   1+3. SHIFT AND DIVIDE THE PARAMETERS IN PLACE.
 *        `px >>= 16; py >>= 16;` instead of `x = px >> 16; y = py >> 16;`.
 *        The parameter's OWN pseudo is the shift's destination, so the
 *        parameter-save `mov` that assign_parms emitted cannot be propagated
 *        away -- which is how the ROM's `mov r4, r2` + `asr r4, #16` appears
 *        while `px` keeps r1 and its copy degenerates.  `x = fx; x >>= 16;`
 *        is NOT the same edit and the old park measured it inert: a fresh
 *        variable's pseudo is a copy of the parameter's and gets propagated.
 *        Then `x = px / 16; y = py / 16;` for the divisions.
 *        Measured alone: 30 -> 24.
 *
 *   2.   NAME THE TABLE OFFSET.  `off = (sel & 3) * 48 + (0x98 << 1);` then
 *        `*(unsigned char **)(base + off)`, instead of the three-term inline
 *        expression.  This is cause 2, which the park closed as
 *        "SOURCE-UNREACHABLE, verified: parenthesising, reordering the terms
 *        and `&base[...]` each produce BIT-IDENTICAL assembly."  All three of
 *        those ARE inert -- and the park never tried a NAMED offset, which is
 *        what its own TU-mate `src/non_matching/rom_9000/8011f54.c` had
 *        written down: "named table offset for the reg+reg load  53, 55".
 *        With `off` a variable the address is `(plus base reg)`, which
 *        THUMB_GO_IF_LEGITIMATE_ADDRESS (arm.h:2133-2148) accepts as reg+reg,
 *        giving the ROM's `ldr r0, [r5, r3]`.  Inline, fold reassociates to
 *        `(plus (plus base X) 0x130)` and 0x130 exceeds the Thumb SImode
 *        displacement, forcing the park's extra `add`.
 *        >> A "source-unreachable" verdict that lists three spellings has not
 *        >> tried the fourth.  Read the piece-mate's measured list.
 *        Measured on top of 1+3: 24 -> 8 (and 41 insns against 42).
 *
 *   4.   `(x + (y << 7)) * 4` rather than `((y << 7) + x) * 4`, for the ROM's
 *        `add r3, r1, r3` operand order.  8 -> 7.
 *
 *   5.   THE ADDRESS ADD, which is the park's open cause 4
 *        (`rom add r3,r2 + ldrb r3,[r3]` against `ours ldrb r3,[r2,r3]`).
 *        This is the LAST instruction and the whole remaining length gap.
 *        See the mechanism section below.  7 -> 0.
 *
 * MECHANISM OF THE ADDRESS ADD, read out of the vendored compiler.
 * `ewram_202c000[i]` NEVER reaches an add-then-load, in two stages:
 *   - explow.c `memory_address` calls `break_out_memory_refs` first (because
 *     `cse_not_expected` is false at -O2), which force_reg's the symbol_ref
 *     into a pseudo; the address is then `(plus reg reg)`, which
 *     THUMB_GO_IF_LEGITIMATE_ADDRESS accepts, so the fold happens AT EXPAND.
 *     `<base>.c.00.rtl` shows `(mem/s:QI (plus:SI (reg 60) (reg ...)))`
 *     already formed.
 *   - Spell the address as an integer (`ia = (int)ewram_202c000 + i;` then
 *     `*(unsigned char *)ia`) and expand DOES emit a separate `(set ia (plus
 *     ...))` -- and combine deletes it: insn 96 reads NOTE_INSN_DELETED in
 *     `<base>.c.13.combine`.
 *   - THUMB_LEGITIMIZE_ADDRESS (arm.h:2268-2270) is empty without -fpic, so
 *     there is no target hook to lean on, and `-fforce-addr` -- the one lever
 *     in `memory_address`'s `win` branch -- MEASURED INERT here (still 7 of
 *     45 under OBJCMP_EXTRA=-fforce-addr).
 *
 * SO THE BLOCKER IS `combine`, AND combine.c:1182-1188 IS THE DOOR:
 *
 *     for (p = NEXT_INSN (insn); p != i3; p = NEXT_INSN (p))
 *       if (GET_RTX_CLASS (GET_CODE (p)) == 'i'
 *           && p != succ && volatile_insn_p (PATTERN (p)))
 *         return 0;
 *
 * -- `can_combine_p` refuses when a VOLATILE INSN sits between i2 and i3.
 * `__asm__ volatile ("" ::: "memory")` is one; the body below places it
 * between the pointer computation and the load and the add survives.
 *
 * >> AND A BARE `__asm__ volatile ("")` IS **NOT** A COMBINE BARRIER.
 * >> rtlanal.c `volatile_insn_p` lists ASM_INPUT in the `return 0` group and
 * >> only returns 1 for UNSPEC_VOLATILE or an ASM_OPERANDS with
 * >> MEM_VOLATILE_P.  A no-operand no-clobber asm expands to ASM_INPUT, so it
 * >> is a SCHEDULING barrier (batch 335's readiness lever) and nothing else.
 * >> Measured: the bare form reads 7 of 45, inert.  The `::: "memory"` form
 * >> (ASM_OPERANDS inside a PARALLEL, found by volatile_insn_p's recursive
 * >> scan) reads 0.  Two different instruments with the same spelling in
 * >> everyone's memory.
 *
 * THE ARTIFACT, STATED PLAINLY.  This landing carries ONE inline-asm barrier
 * and nothing else: no pins, no `.equ`, no fictitious symbol, no flag row.
 * `tools/shimcount.py` reports ZERO for it, and that is a BLIND SPOT, not a
 * clean bill: shimcount's BARRIER pattern is `__asm__\s*volatile\s*\(\s*""\s*\)`
 * and matches only the BARE form, so neither `::: "memory"` nor `:: "i"(0)`
 * is counted.  >> FOURTH KNOWN shimcount BLIND SPOT; the regex wants widening
 * before this file is audited.
 *
 * TWO OTHER EXACT FORMS, both measured 0 of 45, recorded so the choice is the
 * reader's and not buried:
 *   A. this one -- `q = ewram_202c000 + i; __asm__ volatile ("" ::: "memory");
 *      return *q & 0xf;`
 *   B. the same with the integer-address spelling and the same barrier.
 *   C. NO BARRIER AT ALL -- `return *(volatile unsigned char *)(ewram_202c000
 *      + tbl[3] * 4) & 0xf;`  A volatile MEM is refused by `can_combine_p`
 *      too, and by cse.  It is ONE instruction shorter in source and carries
 *      no asm.  It is NOT shipped because it asserts something about the data
 *      that nothing here establishes, and owner-decisions.md ruling 4 draws
 *      the line at exactly that: a volatile qualifier is a LEVER only where
 *      the width or ordering is a genuine property of the location.
 *   >> IF THE OWNER PREFERS C, THE EDIT IS ONE LINE AND THE FIGURE IS THE SAME.
 *
 * MEASURED INERT ON THE FINISHED BODY (all 0 of 45 or noted):
 *   integer-address spelling instead of the pointer         0  (form B)
 *   `(sel & 3) * 48` vs `((sel & 3) * 3 << 4)` in `off`     0  (both exact)
 * MEASURED WORSE:
 *   bare `__asm__ volatile ("")`                            7 of 45, 41 insns
 *   `-fforce-addr` (OBJCMP_EXTRA)                           7 of 45, 41 insns
 *   `-fno-schedule-insns2`                                  not tried here
 *   `q = base; q += off;` for the ewram pointer             14 of 45
 *   the barrier WITHOUT edits 1-3 (park body + volatile)    43 of 45, LONGER
 *     -- which is why the park never saw it: cause 4's lever only pays once
 *     causes 1 and 3 have stopped overfeeding the instruction count. */
extern int iwram_3001e70;
extern unsigned char gBuffer[];
extern unsigned char ewram_202c000[];

int Func_8011fd8(int sel, int px, int py)
{
    char *base;
    unsigned char *tbl;
    unsigned char *q;
    int x;
    int y;
    int off;
    int ia;
    int m;

    base = (char *)iwram_3001e70;
    px >>= 16;
    py >>= 16;
    tbl = gBuffer;
    if (base != 0) {
        off = (sel & 3) * 48 + (0x98 << 1);
        tbl = *(unsigned char **)(base + off);
    }
    x = px / 16;
    y = py / 16;
    tbl += (x + (y << 7)) * 4;
    q = ewram_202c000 + tbl[3] * 4;
    __asm__ volatile ("" ::: "memory");
    return *q & 0xf;
}
