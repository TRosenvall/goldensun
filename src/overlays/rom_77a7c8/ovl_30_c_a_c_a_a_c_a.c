/* src/overlays/rom_77a7c8/ovl_30_c_a_c_a_a_c_a.c -- BOTH functions of
 * asm/overlays/rom_77a7c8/ovl_30_c_a_c_a_a_c_a.s, in ROM order.
 * ***** LANDS. BYTE-IDENTICAL. NO SPLIT. PIN-FREE. PRODUCTION FLAGS. *****
 *
 * This single file is the deliverable for BOTH brief-B targets 2 and 3:
 *   OvlFunc_881_20082cc   3 of 16 -> 0      (13 instructions + 2 pool words)
 *   OvlFunc_881_20082f0   5 of 17 -> 0      (15 instructions + 1 pool word)
 *
 * FIGURES IN, re-measured this batch on the INSTALLED park bodies:
 *   20082cc  src/non_matching/ovl_77a7c8/20082cc.c
 *            3 of 16, SIZE ref 36 ours 32, RELOCATION at 0x1c vs ref 0x20
 *   20082f0  src/non_matching/ovl_77a7c8/20082f0.c
 *            5 of 17, SIZE and RELOCATIONS exact, first diff at index 3
 * FIGURE OUT:
 *   `OK whole file -- 72 bytes, 33 encodings and 2 relocations identical`
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/overlays/rom_77a7c8/ovl_30_c_a_c_a_a_c_a.c \
 *     asm/overlays/rom_77a7c8/ovl_30_c_a_c_a_a_c_a.s --whole
 *   (while still parked, point the first argument at this file.)
 *
 * NO SPLIT, NO LINKER-SCRIPT CHANGE.
 *   python3 tools/datacheck.py asm/overlays/rom_77a7c8/ovl_30_c_a_c_a_a_c_a.s
 *       -> rc=0, NO data section.  The .s holds exactly these two functions and
 *          BOTH land, so the whole .s converts at once -- which is what its own
 *          park anticipated ("IF BOTH LAND, NO SPLIT IS NEEDED AT ALL").
 *   split_s.py was run --dry-run anyway, for the record: cutting 20082f0 would
 *   have written ..._a.s (24 lines) + ..._b.s (22 lines) and rewritten
 *   overlays/rom_77a7c8/overlay.ld.  DO NOT RUN IT.
 *
 * INSTALL AT : src/overlays/rom_77a7c8/ovl_30_c_a_c_a_a_c_a.c
 *              and delete asm/overlays/rom_77a7c8/ovl_30_c_a_c_a_a_c_a.s.
 *              The next build writes a generated .s to that same path; commit
 *              it (CLAUDE.md).  Retire both parks.
 * fakematch.txt ROW: NOT NEEDED for either function.  0 pins, no inline asm,
 *              no non-production flag, no device.  tools/shimcount.py reports
 *              nothing on this file.
 *
 * THE SYMBOL REQUEST IS DEAD -- AND THE ARGUMENT FOR IT IS REFUTED, NOT MERELY
 * UNNECESSARY.
 * ===========================================================================
 * 20082cc's park withheld a `*.sym` request for a zero-valued symbol, on this
 * structural argument: "gcc-2.96 Thumb will not put a value in the pool that an
 * 8-bit `mov` can build, and no 32-bit value V with V == 0 fails to be
 * 8-bit-movable.  So the ROM's pool word 0x00000000 is a RELOCATION against a
 * symbol whose linked value is zero."  That premise is false, and one line of C
 * shows it:
 *
 *     void f(unsigned char *p) { *(unsigned short *)(p + 4) = 0; }
 *
 *     ldrh    r3, .L3
 *     strh    r3, [r0, #4]
 *     bx      lr
 *   .L3:
 *     .word   0
 *
 * gcc-2.96's Thumb `movhi` has NO immediate alternative, so EVERY HImode
 * constant goes to the literal pool -- the value 0 included.  The park tested
 * six spellings and all six were SImode or QImode moves, where `mov rN,#imm8`
 * does exist.  The bound it inferred was a bound on the two modes it happened
 * to test.
 *
 * In 20082cc this fires for free, because the function already performs a
 * halfword store: given a halfword store and a byte store of 0 in the same
 * function, gcc materialises the zero ONCE in HImode and reuses it for the
 * `strb`.  Reduced to the essentials (probe/himode/):
 *     q->f26 = 0;                  alone  ->  mov r3, #0          (no pool)
 *     q->f1e = v; q->f26 = 0;             ->  ldrh r3, .L3 / .word 0
 * So the ROM's 0x00000000 is a LITERAL POOL WORD, there is no symbol, and
 * `ldr r1, .L2e8 @ 0` in the hand-written .s is a faithful transcription of
 * exactly what the compiler emitted.
 *
 * AND IT SOLVES THE PARK'S "BLOCKER 2" (POOL WORD ORDER) AT THE SAME TIME.
 * =======================================================================
 * The park's second blocker was that gcc emitted the pool as
 * [iwram_3001e70, zero] where the ROM has [zero, iwram_3001e70], and that no
 * statement order moved it (60 permutations swept).  The park was right that
 * source order cannot move it, and the reason is worth recording because it is
 * a general law about this compiler:
 *
 *   THE THUMB MINIPOOL IS BUILT AFTER sched2 AND IS SORTED BY `max_address`,
 *   WHICH IS (THE REFERENCING INSN'S ADDRESS + THAT INSN'S POOL RANGE).
 *
 *   - built after sched2: on target 1's body, `-fno-schedule-insns2` REVERSES
 *     the pool word order, so the pool is laid out over the FINAL stream.
 *   - sorted by max_address, not by reference order: for entries of the same
 *     mode the two coincide, but a NARROWER mode has a much smaller pool range
 *     and therefore sorts EARLIER even when it is referenced LATER.
 *
 * The ROM references the iwram word at instruction 0 and the zero word at
 * instruction 7, yet the zero word is FIRST.  That is unreachable for two
 * SImode entries -- and it is exactly what a narrow-mode entry does.  Measured
 * on this function with a placeholder symbol standing in for the zero:
 *     `unsigned int  z`  (SImode fix) -> pool [iwram, z], reloc at 0x20/0x1c
 *     `unsigned char z`  (QImode fix) -> pool [z, iwram], reloc at 0x1c/0x20
 * i.e. the narrow fix reproduces the ROM's pool order AND the ROM's two
 * pool-load encodings exactly (4b07 and 4903 -- gas assembles `ldrb rN, label`
 * and `ldrh rN, label` as a word `ldr rN,[pc,#imm]`).  QImode is still no good,
 * because a QImode fix's range is under 8 bytes and gcc ALWAYS branches around
 * such a pool (`b .L4` + the pool + `bx lr`, +4 bytes; verified on 6 probe
 * shapes with the fix 1 to 6 instructions from the end).  HImode is the mode
 * that works: narrow enough to sort first, wide enough for the pool to fall
 * past the function's end barrier with no branch.
 *
 * So the park's two "independent blockers" were one blocker with one cure, and
 * the cure is not a symbol.
 *
 * WHAT MOVED BOTH FUNCTIONS: TYPE THE PARAMETER AS A STRUCT AND REACH THE
 * FIELDS AS MEMBERS, instead of casting offsets off an `unsigned char *`.
 * ====================================================================
 * Both park bodies did `*(unsigned char **)(p + 0x50)`, `*(p + 0x26)`,
 * `*(p + 0x59)`.  Declaring the layout and writing `p->tgt`, `q->f26`,
 * `p->flag` lands both.  One edit, two functions, and it reaches three
 * different mechanisms:
 *
 *  - 20082cc: it puts the zero's materialisation in HImode (above), which
 *    supplies the missing pool word AND its position AND the size.
 *  - 20082f0 residue B (the `lsl`/`strb` adjacent swap at indices 8-9): the
 *    park showed this is a `rank_for_schedule` DEPENDENT-COUNT tie-break that
 *    the store wins 6 to 3 because it collects a memory dependence against
 *    every later memory reference, and argued the edges were unremovable
 *    because a `strb` is a character access, hence alias set 0, hence
 *    `DIFFERENT_ALIAS_SETS_P` can never fire.  A genuine struct member access
 *    does not take its alias set from the member's type, so it fires anyway.
 *    Isolated: the park body with ONLY the flag byte moved into a
 *    `__attribute__((packed))` one-byte struct (address still a cast) reads
 *    3 of 17 -- residue B closed on its own.
 *  - 20082f0 residue A (the register swap at indices 3-5): a `regclass`
 *    effect.  `-fno-expensive-optimizations` also fixes it, because that flag
 *    gates regclass's SECOND costing pass (`for (pass = 0; pass <=
 *    flag_expensive_optimizations; pass++)`); the `.17.lreg` dumps show pass 1
 *    demoting the flag pointer's pseudo from `pref BASE_REGS` to
 *    `pref LO_REGS`, after which the flag byte takes r3 instead of the ROM's
 *    r2.  With the struct-typed parameter the production two-pass costing
 *    reaches the ROM's assignment by itself, so NO FLAG ROW IS NEEDED and the
 *    park's `-fno-expensive-optimizations` observation is superseded.
 *
 * MEASURED LISTS are in scratch_elev/b318/B/FINDINGS.md: 612 crossed variants
 * on 20082f0 (statement order x declaration order x constant-local x
 * flag-local x return type x struct kind x base type x address spelling x
 * two-word aggregate) all floor at 3 once the packed flag is in, and 13 flags
 * are exactly inert on top of it -- the struct-typed parameter is the only
 * thing that closes residue A without a pin or a flag.
 *
 * THE STRUCT LAYOUTS ARE READ OFF THE ROM, NOT INVENTED
 *   struct Spr   0x50 tgt   (`ldr rN,[r0,#0x50]`, a pointer)
 *                0x59 flag  (`add r0,#0x59` then `ldrb`/`strb`)
 *   struct Tgt   0x1e f1e   (`strh rN,[rM,#0x1e]`)
 *                0x26 f26   (`add rN,#0x26` then `strb`)
 *   struct Blk   0x11a f11a (0x8d<<1 added to the pointer, then `ldrh`)
 * `Spr` must NOT be packed as a whole -- that misaligns `tgt` and costs 40
 * extra bytes (measured: 37 of 17).
 */
struct Tgt {
    unsigned char pad[0x1e];
    unsigned short f1e;
    unsigned char pad2[6];
    unsigned char f26;
};

struct Blk {
    unsigned char pad[0x11a];
    unsigned short f11a;
};

struct Spr {
    unsigned char pad[0x50];
    struct Tgt *tgt;
    unsigned char pad2[5];
    unsigned char flag;
};

extern unsigned int iwram_3001e70;

unsigned int OvlFunc_881_20082cc(struct Spr *p)
{
    struct Blk *blk;
    struct Tgt *q;
    int v;
    unsigned int z;

    blk = (struct Blk *)iwram_3001e70;
    q = p->tgt;
    v = blk->f11a;
    z = 0;
    q->f1e = v;
    q->f26 = z;
    return 1;
}

unsigned int OvlFunc_881_20082f0(struct Spr *p)
{
    unsigned int base;
    struct Tgt *out;

    base = iwram_3001e70;
    out = p->tgt;
    p->flag = 1 | p->flag;
    out->f1e = ((struct Blk *)base)->f11a;
    return 1;
}
