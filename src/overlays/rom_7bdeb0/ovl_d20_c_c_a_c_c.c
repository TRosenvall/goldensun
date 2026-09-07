/* OvlFunc_934_2008e04 and OvlFunc_934_2008f78 -- THE WHOLE FILE.
 *   [asm/overlays/rom_7bdeb0/ovl_d20_c_c_a_c_c.s, all 310 lines.
 *   `grep -c thumb_func_start` returns 2 and this .c defines BOTH, IN FILE
 *   ORDER, so NO SPLIT IS NEEDED and tools/split_s.py is not involved.
 *   `grep -n "\.section\|\.global\|incbin"` on the .s returns NOTHING -- no
 *   .data/.rodata/.bss blob to strand.
 *
 *   THE .ld LINE STAYS VERBATIM, ON THE `asm/` PATH.  Exactly one .ld line in
 *   the tree names this .o (`grep -rn "ovl_d20_c_c_a_c_c\.o" --include="*.ld" .`):
 *
 *     overlays/rom_7bdeb0/overlay.ld:41   asm/overlays/rom_7bdeb0/ovl_d20_c_c_a_c_c.o(.text)
 *
 *   and it is NOT edited.  The build rule is `asm/%.o: src/%.c`, so the object
 *   is still produced under asm/ from this .c; rewriting the line to name
 *   `src/....o` would match no input section, and an unmatched .ld entry is
 *   SILENTLY IGNORED rather than an error -- both functions would vanish from
 *   the ROM behind a green build.  Only the .s is deleted.
 *
 *   NO NAME COLLISION: src/overlays/rom_7bdeb0/ovl_d20_c_c_a_c_c.c does not
 *   exist.  makefile_flags() on it is set() with WILDCARD_HITS empty -- NO FLAG
 *   GROUP AND NO WILDCARD HAZARD.  rom_7bdeb0 has exactly two explicit .c rules
 *   in the Makefile (lines 4610 and 4615, stems ovl_d20_c_c_c_b and
 *   ovl_d20_c_c_c_a_b) and NO pattern rule at all, so only the generic
 *   `%.o: %.s`, `%.o: %.c` and `asm/%.o: src/%.c` can reach this path.  objcmp
 *   prints no `(built with: ...)` line.]
 *
 * EXACT AS A WHOLE OBJECT -- both functions at once, which is the only check
 * that catches a merged file's pool and layout, since objcmp's --func filters
 * the REFERENCE and not the candidate:
 *
 *   OK WHOLE OBJECT -- 708 bytes, 296 encodings and 51 relocations identical
 *
 * Reproduced on three consecutive runs.  Per function, against the original
 * asm/ path with --func and a single-function extract:
 *
 *   OK OvlFunc_934_2008e04 -- 372 bytes, 156 encodings and 27 relocations identical
 *   OK OvlFunc_934_2008f78 -- 336 bytes, 140 encodings and 24 relocations identical
 *
 * A NEAR-TWIN PAIR: the same "spawn two effect actors, rain 0x44 frames of
 * particles while the actor sinks, then set the flag and delete them" script on
 * a different actor and a different tile.  f78 was solved first from
 * src/overlays/rom_7ed0a0/ovl_30_a_c_c_a_c_a_c_a_c.c -- which hands over the
 * `struct Cfg` layout, the `struct Cfg *c = &s` pointer local, the
 * `(__Random() * K >> 16) * 0x10000 + base` particle-position idiom and the
 * `a->f55 = 0` / `a->f23 |= 2` accessor shapes -- and e04 was then written by
 * SUBSTITUTING CONSTANTS into the finished f78 and adding its three extra
 * opening statements.  That transcription was 3 differing on first screen, all
 * three in the one construct f78 does not have.
 *
 * The constants that differ, for anyone reading the pair:
 *
 *            e04           f78
 *   slot     9             0xa          (except `->f55 = 0`, which is slot 9 in BOTH)
 *   gate     x>>20 == 0x17 x>>20 == 0x1b
 *   arg0 #1  a->x          a->x - 0x80000
 *   arg0 #2  + 0x80<<13    + 0x80<<12
 *   px base  0xb8 << 17    0xd8 << 17
 *   pz base  0x9c << 18    0xa4 << 18
 *   8010704  (0x17,0x29,1,1,0x17,0x27)  (0x1f,0x27,2,1,0x1b,0x29)
 *   flag     0x80 << 2     0x201        (built vs pooled -- the ROM says which)
 *
 * LEVERS, measured as WHOLE-OBJECT differing encodings against 296:
 *
 *   ONE LOCAL PER INDEPENDENT OPERATION, at f78's two
 *     OvlFunc_common0_18 sites -- `p1` and `p2`,
 *     not one reused `p`                                  16 -> EXACT
 *     This is the single largest lever in the file and it is the recorded
 *     "One local per independent operation (batch 57)" -- but the diff it cures
 *     LANDS NOWHERE NEAR THE STATEMENT THAT CAUSES IT.  Reusing one `p` swaps
 *     r5/r6 at both call sites (a shared 0xd0<<14 wants the callee-saved half),
 *     turns the ROM's destructive `add r5, r2` into a three-operand
 *     `add r6, r3, r2`, AND -- 25 instructions later, inside the particle loop
 *     -- moves the 0xd8<<17 base from r2 to r3.  A reader chasing the loop
 *     constant would never look at the declaration list.
 *
 *   PIN3 at e04's __Func_8092158(0, 0xb4<<1, 0xa6<<2)        3 -> EXACT
 *     Unaided gcc emits `mov r1 / mov r2 / lsl r1 / lsl r2 / mov r0`; the ROM
 *     puts `mov r0, #0` BEFORE both shifts.  The recorded "PINNING r0 ALONE
 *     ORDERS A POOLED r1" does NOT reach it -- PIN1 is 4, WORSE than no pin at
 *     all -- and a descending fill (q1, q2, then q0) is 3, i.e. inert.  PIN2 is
 *     also exact; PIN3 ships for uniform ascending fill.
 *
 *   the stack-argument pair named and assigned
 *     adjacent to the call, at BOTH __Func_8010704 sites   3 each -> EXACT
 *     Textbook "The stack-arg-pair lever": literals give
 *     `mov r3 / str / mov r3 / str` where the ROM materialises both first.
 *     Note e04's pair is (0x17, 0x27) while its FIRST register argument is also
 *     0x17 -- and the shared value must NOT be named once and used twice here;
 *     the ROM rebuilds it (`mov r3, #0x17` ... `mov r0, #0x17`), so two fresh
 *     locals and a bare literal first argument is what matches.
 *
 * MEASURED WORSE (whole object, 296 encodings):
 *
 *   spelling                                                    differing
 *   ----------------------------------------------------------  ---------
 *   one reused `p` across f78's two OvlFunc_common0_18 sites        16
 *   PIN1 (q0 only) at __Func_8092158                                 4
 *   bare literals at f78's __Func_8010704 stack pair                  3
 *   bare literals at e04's __Func_8010704 stack pair                  3
 *   no pin at __Func_8092158                                          3
 *   descending fill (q1, q2, q0) at __Func_8092158                    3
 *
 *   INERT (tie at 0, so the plainer or more uniform form ships):
 *     f78's two x expressions written INLINE with no `p1`/`p2` at all
 *       -- the defect is SHARING one local, not naming; per-site locals and
 *       no locals are byte-identical, which is the offset-gated reading of
 *       "A POINTER-RETURNING CALL: SHARING IS THE DEFECT, NOT NAMING"
 *     a named `struct Actor *a2` for e04's first `->x` read (offset +8, so
 *       sharing across it is free) instead of the inline accessor
 *     PIN2 instead of PIN3 at __Func_8092158
 *
 * WHAT NEEDED NOTHING.  The 0x44-iteration loop as a plain
 * `for (i = 0; i <= 0x43; i++)` over an `unsigned int` with the counter bump at
 * the latch; `(__Random() * 0x11 >> 16) * 0x10000` written out (the ROM's
 * `lsl #4 / add` is gcc's own multiply-by-17, and `* 0xe` is its
 * `lsl #3 / sub / lsl #1`); `-0x80000` and `-0x8000` as plain subtractions,
 * which pool exactly as the ROM does; `__SetFlag(0x80 << 2)` for e04 against
 * `__SetFlag(0x201)` for f78 -- the ROM builds one and pools the other and the
 * two spellings are not interchangeable; and the TWO HIGH REGISTERS the ROM
 * holds (r8 and r10, the two spawned actors carried across the loop), which gcc
 * reaches unaided with no eviction pin anywhere in the file.  `hiv=2` was
 * accurate here.
 */
struct Actor {
    unsigned char pad00[8];
    int x;
    int y;
    int z;
    unsigned char pad14[0x23 - 0x14];
    unsigned char f23;
    unsigned char pad24[0x55 - 0x24];
    unsigned char f55;
};

struct Cfg {
    int f00;
    int f04;
    int f08;
    int f0c;
    unsigned char pad10[0x28 - 0x10];
};

extern struct Actor *__MapActor_GetActor(int slot);
extern void __SetFlag(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __PlaySound(int id);
extern unsigned int __Random(void);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __DeleteActor(void *a);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __Func_8092158(int a, int b, int c);
extern void __Func_8092adc(int a, int b, int c);
extern void *OvlFunc_common0_18(int a, int b, int c, int d);
extern void OvlFunc_common0_10c(int x, int y, int z, int a, int b, int c,
                                int d, struct Cfg *s);

#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")

void OvlFunc_934_2008e04(void)
{
    struct Cfg s;
    struct Cfg *c;
    struct Actor *a;
    void *b1;
    void *b2;
    unsigned int i;
    int px;
    int pz;
    int p2;
    int m;
    int n;

    __CutsceneStart();
    a = __MapActor_GetActor(9);
    if (a->x >> 20 == 0x17) {
        { PIN3; q0 = 0; q1 = 0xb4 << 1; q2 = 0xa6 << 2;
          __Func_8092158(q0, q1, q2); }
        __Func_8092adc(0, 0xe0 << 8, 0xa);
        __MapActor_GetActor(9)->x += 0x80 << 10;
        b1 = OvlFunc_common0_18(__MapActor_GetActor(9)->x, 0,
                                __MapActor_GetActor(9)->z + (0xd0 << 14), 0xf1);
        p2 = __MapActor_GetActor(9)->x + (0x80 << 13);
        b2 = OvlFunc_common0_18(p2, 0,
                                __MapActor_GetActor(9)->z + (0xd0 << 14), 0xf1);
        __MapActor_GetActor(9)->f55 = 0;
        c = &s;
        c->f08 = 0x9999;
        c->f0c = 0x9999;
        c->f04 = 7;
        __PlaySound(0xd8);
        for (i = 0; i <= 0x43; i++) {
            px = (__Random() * 0x11 >> 16) * 0x10000 + (0xb8 << 17);
            pz = (__Random() * 0xe >> 16) * 0x10000 + (0x9c << 18);
            OvlFunc_common0_10c(px, 0, pz, 0, 0, 0, 0x90 << 12, c);
            __MapActor_GetActor(9)->y -= 0x8000;
            __CutsceneWait(1);
        }
        m = 0x17;
        n = 0x27;
        __Func_8010704(0x17, 0x29, 1, 1, m, n);
        __MapActor_GetActor(9)->f23 |= 2;
        __SetFlag(0x80 << 2);
        __MapActor_GetActor(9)->y = -0x80000;
        __MapActor_SetAnim(9, 2);
        __DeleteActor(b1);
        __DeleteActor(b2);
        __CutsceneWait(0x1e);
    }
    __CutsceneEnd();
}

void OvlFunc_934_2008f78(void)
{
    struct Cfg s;
    struct Cfg *c;
    struct Actor *a;
    void *b1;
    void *b2;
    unsigned int i;
    int px;
    int pz;
    int p1;
    int p2;
    int m;
    int n;

    __CutsceneStart();
    a = __MapActor_GetActor(0xa);
    if (a->x >> 20 == 0x1b) {
        p1 = __MapActor_GetActor(0xa)->x - 0x80000;
        b1 = OvlFunc_common0_18(p1, 0,
                                __MapActor_GetActor(0xa)->z + (0xd0 << 14), 0xf1);
        p2 = __MapActor_GetActor(0xa)->x + (0x80 << 12);
        b2 = OvlFunc_common0_18(p2, 0,
                                __MapActor_GetActor(0xa)->z + (0xd0 << 14), 0xf1);
        __MapActor_GetActor(9)->f55 = 0;
        c = &s;
        c->f08 = 0x9999;
        c->f0c = 0x9999;
        c->f04 = 7;
        __PlaySound(0xd8);
        for (i = 0; i <= 0x43; i++) {
            px = (__Random() * 0x11 >> 16) * 0x10000 + (0xd8 << 17);
            pz = (__Random() * 0xe >> 16) * 0x10000 + (0xa4 << 18);
            OvlFunc_common0_10c(px, 0, pz, 0, 0, 0, 0x90 << 12, c);
            __MapActor_GetActor(0xa)->y -= 0x8000;
            __CutsceneWait(1);
        }
        m = 0x1b;
        n = 0x29;
        __Func_8010704(0x1f, 0x27, 2, 1, m, n);
        __MapActor_GetActor(0xa)->f23 |= 2;
        __SetFlag(0x201);
        __MapActor_GetActor(0xa)->y = -0x80000;
        __MapActor_SetAnim(0xa, 2);
        __DeleteActor(b1);
        __DeleteActor(b2);
        __CutsceneWait(0x1e);
    }
    __CutsceneEnd();
}
