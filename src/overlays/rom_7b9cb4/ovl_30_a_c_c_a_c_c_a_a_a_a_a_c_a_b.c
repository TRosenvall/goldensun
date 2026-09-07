/* OvlFunc_932_2008d2c
 *   [asm/overlays/rom_7b9cb4/ovl_30_a_c_c_a_c_c_a_a_a_a_a_c_a.s, lines 13-193
 *   of 677.  THE FILE HOLDS TWO FUNCTIONS -- this one and the 466-instruction
 *   OvlFunc_932_2008ec0 -- so A SPLIT IS NEEDED.  `grep -n "\.section\|\.global\|
 *   incbin"` on the .s returns NOTHING, so there is no data blob to strand and
 *   the split is text-only.  No `.L` label crosses the function boundary in
 *   either direction (checked both ways: d2c defines and uses exactly
 *   .Ld8e/.Ldc0/.Ldc8/.Ldfa/.Le04/.Le44/.Le58, 2008ec0 exactly
 *   .L108c/.L10d0/.L10e6/.L121c/.L1250/.L12f0/.L1300/.L1388, and neither
 *   region references a label it does not define).
 *
 *   This function is FIRST in the .s, so tools/split_s.py writes no `_a` part:
 *
 *     src/overlays/rom_7b9cb4/ovl_30_a_c_c_a_c_c_a_a_a_a_a_c_a_b.c   <- this
 *     asm/overlays/rom_7b9cb4/ovl_30_a_c_c_a_c_c_a_a_a_a_a_c_a_c.s   <- 2008ec0
 *
 *   Both stems are FREE: no file of either name exists under asm/ or src/ and
 *   no .ld line anywhere names either .o.
 *
 *   THE .ld LINES STAY ON THE `asm/` PATH.  The build rule is
 *   `asm/%.o: src/%.c`, so the object for this .c is still produced at
 *   asm/overlays/rom_7b9cb4/ovl_30_a_c_c_a_c_c_a_a_a_a_a_c_a_b.o.  A line
 *   naming `src/....o` matches no input section, and an unmatched .ld entry is
 *   SILENTLY IGNORED rather than an error -- the functions would vanish from
 *   the ROM behind a green-looking build.  Exactly one .ld line in the tree
 *   names the pre-split .o (`grep -rn "ovl_30_a_c_c_a_c_c_a_a_a_a_a_c_a\.o"
 *   --include="*.ld" .`):
 *
 *     overlays/rom_7b9cb4/overlay.ld:48
 *
 *   and split_s.py replaces it, in place and in order, with the `_b` and `_c`
 *   asm-path lines.  The .data/.rodata/.bss lists of that .ld name no stem of
 *   this file and must not be touched.
 *
 *   OvlFunc_932_2008ec0 is called from here and stays in asm.  It needs no
 *   `.global` added: include/macros.inc's `.thumb_func_start` expands to
 *   `.global \sym` (macros.inc:13-23), so every `.thumb_func_start` symbol in
 *   the tree is already global.
 *
 *   makefile_flags() on src/overlays/rom_7b9cb4/ovl_30_a_c_c_a_c_c_a_a_a_a_a_c_a_b.c
 *   is set() with WILDCARD_HITS empty -- NO FLAG GROUP AND NO WILDCARD HAZARD.
 *   rom_7b9cb4 has exactly one explicit .c rule in the whole Makefile (line
 *   419, `ovl_30_a_c_c_a_c_c_a_a_a_c_a_c_a_b`, a different stem) and NO pattern
 *   rule at all, so only the generic `%.o: %.s`, `%.o: %.c` and
 *   `asm/%.o: src/%.c` can reach this path.  objcmp prints no
 *   `(built with: ...)` line.]
 *
 * EXACT, measured as a single-function extract and against the ORIGINAL asm/
 * path with --func, reproduced on three consecutive runs of each:
 *
 *   OK OvlFunc_932_2008d2c -- 404 bytes, 175 encodings and 23 relocations identical
 *
 * 171 instructions: a cutscene opener, THREE cos/sin spin loops driven by one
 * halfword angle field, and a fade tail.  The __Func_8012330 PIN3 idiom and the
 * `while (1) { ...; if (...) break; __WaitFrames(1); }` loop shape come from
 * src/overlays/rom_7f2f14/ovl_30_a_c_c_a_c_a.c and
 * src/overlays/rom_7ed0a0/ovl_30_a_c_c_a_c_a_c_a_c.c.
 *
 * ------------------------------------------------------------------- NEW ----
 * A UNION ACCESS IS ALIAS SET **0** IN gcc-2.96's C FRONT END, AND THE MEMBER
 * LIST IS IRRELEVANT.
 *
 * Grepped docs/elevation.md first, in the corpus's vocabulary: "strict-alias"
 * (13 hits), "alias set" , "type-pun", "union", "lang_get_alias_set" (0 hits),
 * "alias set 0" (3 hits, all the `char`-lvalue entry).  The recorded entry is
 * "gcse hashes the MEMORY ALIAS SET", batch 244's "A THIRD ESCAPE: A UNION
 * MEMBER ACCESS", which states the mechanism as:
 *
 *   "A COMPONENT_REF of a UNION carries the union's alias set, and
 *    record_component_aliases makes every member type's set a SUBSET of it, so
 *    alias_sets_conflict_p conflicts against any access whose type is a member
 *    -- AND ONLY THOSE."
 *
 * The "and only those" clause is WRONG, and its table row "union{int; u16},
 * other side a pointer store -- 2 differ" is not evidence for it.  Read from
 * the compiler: `lang_get_alias_set` (c-common.c:3336-3345) walks OUT through
 * every COMPONENT_REF/ARRAY_REF and returns a flat **0** the moment any step's
 * object has UNION_TYPE --
 *
 *       "Permit type-punning when accessing a union, provided the access
 *        is directly through the union."
 *     for (u = t; TREE_CODE (u) == COMPONENT_REF || ARRAY_REF; u = OP (u, 0))
 *       if (COMPONENT_REF && TREE_CODE (TREE_TYPE (OP (u, 0))) == UNION_TYPE)
 *         return 0;
 *
 * -- and 0 conflicts with EVERYTHING, so record_component_aliases and the
 * subset machinery are never consulted.  Four controls on the store that had to
 * be pinned below an `unsigned short` load, all on the finished base:
 *
 *   union { int w; unsigned short h; }  the member type of the other side   EXACT
 *   union { int w; long l; }            NO member related to the other side  EXACT
 *   union { int w; }                    ONE member, nothing to be a subset   EXACT
 *   struct { int w; }                   same wrapping, not a union            8 differing
 *   no wrapper at all                                                         8 differing
 *
 * A one-member union cannot conflict "against any access whose type is a
 * member" with an `unsigned short` load, and it is byte-identical.  So the
 * lever is UNION-NESS, not membership; the shipped two-member spelling is a
 * readability choice and neither member is load-bearing.
 *
 * The recorded "the only source-level escape is `volatile` on BOTH sides"
 * ALSO fails here and should be read as function-specific: `volatile` on both
 * the int field and the halfword field is 131 differing and 8 bytes LONG;
 * `volatile` on the int side alone is 8, i.e. inert.
 *
 * WHAT THE UNION BUYS.  Without it, sched2 hoists the exit test's
 * `ldrh r1, [r6, #0x1e]` THREE SLOTS UP, above the `str r3, [r2, #8]` that
 * writes the actor's x -- legal because an `int` store and an `unsigned short`
 * load are in different alias sets.  The ROM keeps the load below the store.
 * It costs 8 encodings in loops 1 and 2 (loop 3 is unaffected: its `bl __sin`
 * already fills the window).  `-fno-strict-aliasing` cures the identical 8 and
 * confirms the mechanism, but no flag can ship -- this stem falls under no
 * flag group.
 *
 * ------------------------------------------------------ CONFIRMATION --------
 * THE ANGLE STEP IS AN `unsigned short` HELD PRE-SHIFTED LEFT BY 16.  All three
 * loops drive r7, and gcc keeps the variable as `d << 16`: `d += 8` is
 * `add r7, #8<<16`, reading `d` is `lsr rX, r7, #16`, and loop 3's `d += d >> 3`
 * is `lsr r2, r7, #19 / lsr r3, r7, #16 / add r3, r2 / lsl r3, #16`.  Writing
 * `d` as an `int` and spelling loop 3 out as
 * `d = ((d >> 16) + (d >> 19)) << 16` -- the literal transcription -- is 165
 * differing and 24 bytes SHORT; `unsigned int` is 166 differing and 28 bytes
 * short.  The `lsl #16` / `lsr #16` pair around a loop-carried value is the
 * tell for a 16-BIT VARIABLE, not for an awkward `int` expression.
 *
 * ------------------------------------------------------ CONFIRMATION --------
 * THE STATEMENT PERMUTATION AND THE ALIAS LEVER ARE ONE SEARCH (lever 4).  The
 * three field reads at the top -- `p`, `x0`, `y0` -- were swept over all six
 * orders AFTER the union was in place.  Only `p, x0, y0` is exact; the five
 * others are 6, 6, 6, 7 and 10.  Run before the union, the same sweep would
 * have found nothing under 14.
 *
 * MEASURED WORSE (against 404 bytes / 175 encodings), every one re-measured on
 * the finished base:
 *
 *   spelling                                                     differing
 *   ------------------------------------------------------------ ---------
 *   `d` as `unsigned int`                                          166 (28 bytes short)
 *   `d` as `int`, loop 3 spelled `((d>>16)+(d>>19))<<16`           165 (24 bytes short)
 *   `x0` dropped, `a->x` re-read in the loops                      165 (4 bytes long)
 *   drop the __Func_8012330(0x80<<10, ...) pin                     152
 *   drop the __Func_8012330(-1,-1,0xe666) OPENING pin              140
 *   `v` dropped, __cos inlined into the a->x expression            137 (8 bytes short)
 *   `volatile` on BOTH the int field and the halfword field        131 (8 bytes long)
 *   `__sin` written INSIDE the `if` instead of above it            165 (16 bytes short)
 *   drop the __Func_8012330(0xc0<<10, ...) pin                      26
 *   drop the __Func_8012330(-1,-1,0xe666) TAIL pin                  16
 *   the post-loop __WaitFrames(1) dropped                           41 (4 bytes short)
 *   `do { } while (1)` for loop 1's `while (1)`                    136
 *   struct wrapper instead of union / no wrapper / `volatile` on
 *     the int field alone                                            8
 *   the five other orders of the three opening field reads       6, 6, 6, 7, 10
 *
 *   INERT (tie at 0, so the plainer or better-documented form ships):
 *     `union { unsigned short h; int w; }` (members reordered)
 *     `union { int w; long l; }`;  `union { int w; }`
 *
 * FLAGS: NO FLAG GROUP and none is reachable, see the Makefile paragraph above.
 * -fno-strict-aliasing is 8 differing on the union spelling (it re-adds nothing
 * the union has not already added, and perturbs nothing else) and is the
 * diagnostic that identified the class, not a candidate for shipping.
 */
struct Sub {
    unsigned char pad00[0x1e];
    unsigned short f1e;
};

union XW {
    int w;
    unsigned short h;
};

struct Actor {
    unsigned char pad00[8];
    union XW x;
    int y;
    unsigned char pad10[0x50 - 0x10];
    struct Sub *f50;
};

extern struct Actor *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __WaitFrames(int n);
extern void __PlaySound(int id);
extern void __Func_8012330(int a, int b, int c);
extern int __cos(int a);
extern int __sin(int a);
extern void OvlFunc_932_2008ec0(int n);

#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")

void OvlFunc_932_2008d2c(void)
{
    struct Actor *a;
    struct Sub *p;
    int x0;
    int y0;
    unsigned short d;
    int v;
    int w;

    a = __MapActor_GetActor(0xa);
    p = a->f50;
    x0 = a->x.w;
    y0 = a->y;
    __CutsceneStart();
    __PlaySound(0x8d);
    { PIN3; q0 = 0x80 << 10; q1 = 0x80 << 9; q2 = 0x80 << 9;
      __Func_8012330(q0, q1, q2); }
    __CutsceneWait(0xa);
    __PlaySound(0x121);
    { PIN3; q0 = -1; q1 = -1; q2 = 0xe666;
      __Func_8012330(q0, q1, q2); }
    __CutsceneWait(0x14);
    d = 0;
    while (1) {
        d += 8;
        p->f1e += d;
        v = __cos(p->f1e + (0x80 << 7));
        a->x.w = x0 + (v << 4);
        if (p->f1e > 0x8fff)
            break;
        __WaitFrames(1);
    }
    d = 0;
    while (1) {
        d += 8;
        p->f1e -= d;
        v = __cos(p->f1e + (0x80 << 7));
        a->x.w = x0 + (v << 4);
        if (p->f1e <= (0xe0 << 7))
            break;
        __WaitFrames(1);
    }
    d = 8;
    while (1) {
        d += d >> 3;
        p->f1e += d;
        v = __cos(p->f1e + (0x80 << 7));
        w = __sin(p->f1e + (0x80 << 8));
        a->x.w = x0 + (v << 4);
        if (p->f1e > (0x80 << 8))
            a->y = y0 - (w << 3);
        if (p->f1e + d > 0xbfff)
            break;
        __WaitFrames(1);
    }
    __WaitFrames(1);
    p->f1e = 0xc0 << 8;
    __PlaySound(0xb7);
    { PIN3; q0 = 0xc0 << 10; q1 = 0xc0 << 10; q2 = 0x80 << 9;
      __Func_8012330(q0, q1, q2); }
    __CutsceneWait(0x14);
    __PlaySound(0x121);
    { PIN3; q0 = -1; q1 = -1; q2 = 0xe666;
      __Func_8012330(q0, q1, q2); }
    OvlFunc_932_2008ec0(5);
    __CutsceneEnd();
}
