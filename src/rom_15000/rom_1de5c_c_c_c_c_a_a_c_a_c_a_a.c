/* Func_801f730  @  0x0801f730  [rom_15000]   *** LANDING -- 0 of 33 ***
 * MATCHING. Byte-identical. PIN-FREE (0 pins). DEVICE-FREE. No flag group.
 *
 * FIGURE (measured, batch 328 brief E, on this body):
 *   objcmp --func : OK Func_801f730 -- 76 bytes, 33 encodings and 4
 *                   relocations identical.
 *   objcmp --whole: ok Func_801f730 33 encodings; OK whole file -- 76 bytes,
 *                   33 encodings and 4 relocations identical.
 *   The park this replaces measured 3 of 33, re-derived first (ref 33 / ours 33,
 *   first at index 12, SIZE 76 v 76, relocations identical) -- figure CONFIRMED
 *   before it was beaten.
 *
 * Verify with (INSTALLED PATH, one line):
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work goldensun-build python3 tools/objcmp.py src/rom_15000/rom_1de5c_c_c_c_c_a_a_c_a_c_a_a.c asm/rom_15000/rom_1de5c_c_c_c_c_a_a_c_a_c_a_a.s --func Func_801f730
 *
 * SPLIT SHAPE: NONE NEEDED.
 *   grep -c thumb_func_start asm/rom_15000/rom_1de5c_c_c_c_c_a_a_c_a_c_a_a.s -> 1
 *   python3 tools/datacheck.py <that .s>            -> clean, rc=0
 *   python3 tools/split_s.py   <that .s> --dry-run  -> nothing to split
 *   INSTALL PATH: src/rom_15000/rom_1de5c_c_c_c_c_a_a_c_a_c_a_a.c
 *   RETIRES the park src/non_matching/rom_15000/801f730.c.
 *
 * ===========================================================================
 * WHAT CLOSED THE LAST INSTRUCTION
 * ===========================================================================
 * Batch 327 brief D got this function to the ROM's exact instruction stream with
 * ONE insn missing -- `lsl r3,#0x18` (its f730_w1.c; re-derived here as 20 of 33
 * / ours 31 insns / -4 bytes, and `tryc --full` says `rom 33 lines, ours 32`,
 * sole defect `rom lsl r3,#0x18 | ours add r2,#0x40`).
 *
 * THE MISSING INSN WAS NOT A COMBINE PROBLEM AND NOT AN ALLOCATION PROBLEM.
 * It was `PROMOTE_MODE` applied to the byte temp, and the fix is one type.
 *
 *   `promote_mode` (explow.c:881, switch at :895-902) applies PROMOTE_MODE ONLY to
 *       INTEGER_TYPE  ENUMERAL_TYPE  BOOLEAN_TYPE  CHAR_TYPE  REAL_TYPE  OFFSET_TYPE
 *   A RECORD_TYPE / UNION_TYPE is NOT in that switch, so an aggregate local is
 *   never promoted; and arm.h's PROMOTE_MODE (:597-606) sets `UNSIGNEDP = 1` for
 *   QImode UNCONDITIONALLY, so EVERY scalar char local becomes an SImode
 *   zero-extended pseudo.
 *
 * The two dumps differ in exactly one RTL line.  w1 (.12.life), named `signed char c`:
 *     (insn 51 (set (reg:SI 41) (zero_extend:SI (mem:QI (reg/v:SI 36) 0))))  ldrb
 *     (insn 56 (set (reg/v:SI 36) (plus (reg/v:SI 36) 64)))                  THE BUMP
 *     (insn 58 (set (reg:SI 43) (ashift   (reg:SI 41) 24)))                  lsl #24
 *     (insn 59 (set (reg:SI 42) (ashiftrt (reg:SI 43) 24)))                  asr #24
 *   `nonzero_bits (reg 41) <= 0xff` because the value is a QI `zero_extend`, so
 *   simplify_comparison takes `(eq (ashiftrt (ashift x 24) 24) 0)` all the way to
 *   `(eq x 0)` -- killing BOTH shifts.  Hence 32 insns, not 33.
 *
 * THIS BODY (.12.life), the byte read as a struct field into no temp at all:
 *     (insn 51 (set (reg:QI 41) (mem:QI (reg/v:SI 36) 0)) *thumb_movqi_insn)
 *     (insn 56 (set (reg/v:SI 36) (plus (reg/v:SI 36) 64)))
 *     (insn 58 (set (reg:SI 43) (ashift (subreg:SI (reg:QI 41) 0) 24)))
 *     (insn 59 (set (reg:SI 42) (ashiftrt (reg:SI 43) 24))
 *                REG_EQUAL (sign_extend:SI (reg:QI 41)))      <-- note: NO `/u`
 *   The shift now reads a PARADOXICAL SUBREG of a QImode pseudo, whose high bits
 *   are unknown, so simplify_comparison can drop only the `ashiftrt` (always
 *   valid for a `!= 0` test) and the `ashift` SURVIVES.  That is the ROM's
 *   `lsl r3,#0x18`.
 *
 * AND THE SECOND HALF WAS ALREADY THERE.  Both dumps carry insn 56 -- the
 * pointer bump -- BETWEEN the load and the shift, put there by the SOURCE.  It
 * writes reg 36, the MEM's own address register, so `can_combine_p`
 * (combine.c:933, guard at :1083-1088) refuses on
 * `! all_adjacent && use_crosses_set_p (src, INSN_CUID (insn))` and combine
 * never re-forms `(sign_extend:SI (mem:QI ...))`.  No `ldrsb`.
 *
 * ---------------------------------------------------------------------------
 * THE CROSSING, WITH BOTH CONTROLS -- NEITHER EDIT WORKS ALONE
 *   e1  union temp + bump between read and test                  0  *** MATCH ***
 *   e6  CONTROL: union temp, bump AFTER the `if`                  7  first at 16
 *   e7  CONTROL: w1 verbatim, named `signed char`, bump between  20  33/31 insns
 * The aggregate alone is worth 7; the bump position alone is worth 20; together
 * they are worth 0.  One more instance of the cross-the-lists law.
 *
 * ---------------------------------------------------------------------------
 * WHAT THE LANDED MODULE-MATES TOLD ME.  `tools/upstream_module.py Func_801f730`
 * -> upstream rom_15000/rom_1de5c.s, 42 landed .c against 7 parks.  The
 * IMMEDIATE PREDECESSOR, `src/rom_15000/rom_1de5c_c_c_c_c_a_a_c_a_b.c`
 * (Func_801f704), is landed and walks THE SAME GLOBAL with the same stride:
 *     extern unsigned char iwram_3001f1c[];
 *     r2 = *(unsigned char **)iwram_3001f1c + (0x82 << 5);
 *     do { r3 = *(unsigned char *)(r2 + 0x1c); ... r2 += 0x40; } while (...);
 * So the TU walks an array of 0x40-BYTE RECORDS off a double-indirected
 * `iwram_3001f1c`, reading one byte per record.  This body is written in that
 * idiom -- `extern unsigned char iwram_3001f1c[]` with
 * `*(unsigned char **)iwram_3001f1c`, and a 0x40-byte record type -- which is
 * both why the record struct is not an invention and why the declaration matches
 * its neighbour.  (0x1071 is 0x31 into the record at (0x82 << 5) + 0x40.)
 *
 * ---------------------------------------------------------------------------
 * TWO INHERITED DIAGNOSES REFUTED -- STRIKE THEM WHEREVER THEY ARE PROPAGATED
 *
 * 1. The park's `loop.c` reload bound ("reload can never choose r1", from
 *    loop.c:4408 / :4778) was already called refuted by batch 327.  It is -- but
 *    BATCH 327'S REPLACEMENT MECHANISM IS ALSO WRONG.  It said that with a
 *    walking-pointer biv "the 0x1071 is an ORDINARY PSEUDO that global-alloc
 *    places in r1 ... No reload is involved, and find_reg / spill_cost[r1] never
 *    enter the question."  Measured: IT IS A RELOAD, AND IT IS find_reg's CHOICE.
 *    Combine folds the constant into the add in BOTH bodies -- base and w1 alike
 *    reach `.13.combine` as
 *        (set (reg X) (plus (reg Y) (const_int 4209))) 5 {*thumb_addsi3}
 *    because recog checks the PREDICATE, not the constraint -- and w1's
 *    `.18.greg` then reads
 *        (note 35 ... NOTE_INSN_DELETED 0)
 *        (insn 140 (set (reg:SI 1 r1) (const_int 4209)))
 *    i.e. a reload, into r1.  THE DISCRIMINATOR IS POSITION, NOT PSEUDO-NESS:
 *    the pointer form makes the add the SOURCE'S OWN STATEMENT, which sits
 *    BEFORE `i = 2`, so the counter is not live there and `spill_cost[r1] == 0`;
 *    loop.c's giv init is forced AFTER the counter init and r1 is barred.
 *    So the park's REQUIREMENT (`spill_cost[r1] == 0` at the add, r1 winning the
 *    `inv_reg_alloc_order` tie at equal cost) was exactly right all along; only
 *    its claim that no source shape can order the two insns was wrong.
 *
 * 2. The "MUTUAL EXCLUSION THROUGH loop.c" -- that `ldrb`+`lsl #24` needs
 *    "(a) an UNNAMED `signed char` rvalue ... and (b) the intervening
 *    address-clobbering insn, which only loop.c's GIV update supplies" -- is
 *    REFUTED IN BOTH HALVES.  (b) is supplied by the source's own `p++` written
 *    inside the test expression (w1 had it too, as a separate statement); and
 *    (a) is not "unnamed" but "NOT PROMOTE_MODE'd", which any aggregate-typed
 *    read satisfies.  The exclusion was never mutual, and nothing here needs a
 *    giv, a reload of a giv init, or an invariant base.
 *
 * > The park and batch 327 between them recorded ~45 bodies.  EVERY ONE spelled
 * > the byte as a scalar -- `signed char`, `char`, `unsigned char`, `int` -- and
 * > every scalar integer type goes through PROMOTE_MODE.  The axis never varied
 * > was the TYPE CONSTRUCTOR of the byte's lvalue.
 *
 * ---------------------------------------------------------------------------
 * ALSO MEASURED AT 0 (kept here because pass 4 may prefer a different spelling):
 *   e1 union { signed char c; } temp, bump between      e2 struct, same
 *   e3 bump as `q = q + 0x40`                           e4 truth test `if (u.c)`
 *   e5 up-counting for                                  e8 decl order swapped
 *   e9 `i = 2; do { } while (i >= 0)`                   f1 this body, `signed char *` global
 *   g3 this body with `if ((p++)->f0)`                  g4 this body, up-counting
 * MEASURED WORSE:
 *   f2  record walk, `p++` AFTER the `if`                         7  first at 16
 *   f4  record walk, bump via `&& (p++, 1)` in the test           5  first at 16
 *   g2  field at offset 0x31, base reduced to 0x1040              7  first at 12
 *   e6  union temp, bump after the `if`                           7  first at 16
 *   e7  w1 verbatim                                              20  33/31 insns
 * (Loop form is still one equivalence class under check_dbra_loop -- e5/g4
 * up-counting are bit-identical to the countdown, as the park said.)
 */
extern int Func_80056cc(void);
extern int Func_8005c68(void);
extern void Func_8005cf8(void);
extern unsigned char iwram_3001f1c[];

struct Rec { signed char f0; char pad[0x3f]; };

int Func_801f730(int a)
{
    int r;
    int t;
    int i;
    struct Rec *p;

    t = Func_80056cc();
    r = -9;
    if (t == 0) {
        r = Func_8005c68();
        if (a != 0) {
            p = (struct Rec *)(*(unsigned char **)iwram_3001f1c + 0x1071);
            for (i = 2; i >= 0; i--) {
                if ((p++)->f0 != 0) {
                    r--;
                }
            }
        }
    }
    Func_8005cf8();
    return r;
}
