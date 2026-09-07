/* Cluster OvlFunc_932_2009770 + OvlFunc_932_2009838 + OvlFunc_932_2009d0c
 *   [asm/overlays/rom_7b9cb4/ovl_30_a_c_c_a_c_c_a_a_a_a_c_a_a.s, lines 1-897 --
 *   the WHOLE FILE.  It holds exactly these three functions and NO data: zero
 *   `.incbin`, zero `.section`, zero `.byte`, zero `.global`.  The only `.word`
 *   directives are the two MID-FUNCTION pools (lines 186-189 and 682-685),
 *   which sit inside the functions and are gcc's own.  So there is NO SPLIT.
 *
 *   overlays/rom_7b9cb4/overlay.ld:51 is
 *     `asm/overlays/rom_7b9cb4/ovl_30_a_c_c_a_c_c_a_a_a_a_c_a_a.o(.text)`
 *   and it is the ONLY line in ANY .ld naming this object; no other overlay
 *   carries a file of this basename.  rom_7b9cb4's `.data` (ld:89-91) and
 *   `.bss` (ld:93-95) each list ovl_30_c_c_c.o and nothing else, so no section
 *   named for this .o goes unaccounted, and the compiled .o's own .data and
 *   .bss are both zero-length.  LANDING IS ONE LINE: asm/ -> src/ at
 *   overlay.ld:51, plus deleting the .s.
 *
 *   NO FLAG GROUP.  tools/tryc.py's makefile_flags() returns set() for
 *   src/overlays/rom_7b9cb4/ovl_30_a_c_c_a_c_c_a_a_a_a_c_a_a.c with no
 *   WILDCARD_HITS; there is no `rom_7b9cb4/%` pattern rule in the Makefile at
 *   all, so the generic cross-dir `asm/%.o: src/%.c` fires and the tree default
 *   -O2 -mthumb -mthumb-interwork -fcall-used-r4 applies.]
 *
 * EXACT, all three, measured as single-function extracts of THIS header
 * (objcmp --func cannot isolate one function inside a multi-function
 * candidate):
 *
 *   OK OvlFunc_932_2009770 -- 200 bytes, 92 encodings and 6 relocations identical
 *   OK OvlFunc_932_2009838 -- 1236 bytes, 489 encodings and 109 relocations identical
 *   OK OvlFunc_932_2009d0c -- 788 bytes, 310 encodings and 70 relocations identical
 *
 * And WHOLE-OBJECT, which is the stronger statement objcmp cannot make here:
 * assembling the reference .s and compiling this file give .text sections of
 * 0x8b0 bytes that are BYTE-IDENTICAL, with 185 relocations agreeing on
 * offset, type and symbol in order, and .data / .bss empty on both sides.
 *
 * 2009770 is the per-frame handler the other two install in `f6c`; 2009838 and
 * 2009d0c are the two cutscenes that drive actor 0xa's boat, setting save bits
 * 0x904 and 0x905 respectively.  One `struct Actor` serves all three.
 *
 * ------------------------------------------------------------------- NEW ----
 * THE "POOLED ZERO IS A SYMBOL" TELL HAS A HIGH-REGISTER EXCEPTION.  The
 * recorded rule (THE SYMBOL TELL, generalised) says `ldr r2, =0` feeding BYTE
 * stores means the source named a symbol, because "byte stores have NO QImode
 * analogue of the halfword exception -- compiling `p[0x55] = 0; q[0x26] = 0;`
 * emits `mov r2, #0`".  Both cutscenes here contradict it and match anyway:
 *
 *     rom / ours   ldr r3, =0x0 / mov r10, r3   ... later ... strb r3, [r2]
 *
 * from a plain `p->f5b = 0;`.  What buys the pool load is the DESTINATION: the
 * zero is commoned into a callee-saved HIGH register, `mov r10, #0` is not a
 * Thumb encoding, and gcc reaches r10 through the constant pool rather than
 * through `mov rLOW, #0`.  This file carries its own control: 2009770 stores
 * the same field from the same literal with the value in a LOW register and
 * gets `mov r3, #0` with no pool, exactly as the recorded rule predicts.
 *
 * DISCRIMINATOR, cheap and local: look at the instruction AFTER the pool load.
 * `mov r8..r11, rLOW` means ordinary codegen; anything else and the symbol tell
 * still stands.  Nothing had to be spelled to provoke this -- the pool load
 * falls out of the allocator once the pin set stops competing for r10.
 *
 * ------------------------------------------------------------------- NEW ----
 * A FOURTH LOOP SHAPE: DUPLICATED GUARD + BOTTOM TEST.  The recorded taxonomy
 * (MATCH THE ROM'S LOOP SHAPE) lists bottom-test-with-entry-jump (a plain
 * `while`), and top-test-with-backward-branch (do/while(1)+break, or goto).
 * 2009d0c's tail loop is neither:
 *
 *     ldr r3, [r2] / cmp r3, #0 / bne END / mov r6, r2
 *   L2: mov r0, #1 / add r5, #1 / bl __CutsceneWait
 *     cmp r5, #0x3b / bhi END / ldr r3, [r6] / cmp r3, #0 / beq L2
 *
 * -- the condition appears TWICE, once as a pre-loop guard and once at the
 * bottom, with a SECOND exit inside the body.  gcc-2.96 duplicates a guard like
 * that only for loops it rotates, and a `while` carrying a `break` is not one:
 * `while (gKeyPress == 0) { ...; if (i > 0x3b) break; }` comes out un-rotated,
 * 67 differing and FOUR INSTRUCTIONS SHORT.  Writing the guard yourself --
 * `if (c) do { ... if (x) break; } while (c);` -- is exact.  The `goto`
 * spelling, screened as the parent lever demands rather than predicted, is
 * WORSE than both at 75 differing.
 *
 * ------------------------------------------------------------------- NEW ----
 * STATEMENT ORDER AND THE PIN SET ARE NOT SEPARABLE -- SWEEP THE ORDER AFTER
 * THE STRIP, NEVER BEFORE.  2009838 opens each of its three boat resets with
 * five stores to one actor, and the emitted order is the ROM's whole diagnosis.
 * All 120 permutations of the first block were screened TWICE:
 *
 *     block-1 order          all 59 sites pinned      minimal 30-pin set
 *     ---------------------  -----------------------  ------------------
 *     f68 f64 f66 f48 f6c     84 differing              2 differing  <- ships
 *     f66 f64 f48 f6c f68     84                       23
 *     f66 f68 f6c f64 f48     79 (best of 120)         -
 *
 * Under the full pin set the natural order is NOT the winner and it puts the
 * ROM's r6 and r8 tenants in the wrong registers; a sweep run at that point
 * selects a wrong source order, and the wrong order then survives the strip and
 * reads as a structural blocker.  The pins and the store order compete for the
 * same low registers, so the two searches are one search.
 *
 * ---------------------------------------------------------------- 2009770 --
 *
 * 89 instructions, TWO PINS of nine pinnable sites, and the pins are the whole
 * lever: bare it is 80 differing of 96 and two instructions short, because gcc
 * commons the `-1` and the `0x80 << 9` the ROM rebuilds.  Pinning the
 * `__Func_8012330(-1, -1, 0xe666)` site alone is 53; the pair is exact.  That
 * is the recorded EVICTION PINS COMPETE AND MUST BE ADDED AS A SET, at its
 * smallest yet -- two sites.
 *
 * THE `ldrsh` / `ldrh` SPLIT ON ONE FIELD comes free from `short f64; short
 * f66;` and the recorded rule: the `!= 0` tests read `ldrsh` and the `--` / `++`
 * read `ldrh`.  `a->f66 == 1` after the decrement reproduces the ROM's
 * `lsl r3, #16 / cmp r3, #0x10000` comparison against the shifted literal, and
 * `a->f64 == 0x3c` its `mov r2, #0xf0 / lsl r2, #14`; neither needed spelling.
 *
 * The `a->f28` load is a bare `if (a->f28 == 0)` even though the ROM keeps it
 * in r7 across two calls and later stores it as the zero (`str r7, [r5, #0x68]`)
 * -- so the recorded "survives a call in a callee-saved register" promoter did
 * NOT apply here and a plain literal `0` is what matches.
 *
 * ---------------------------------------------------------------- 2009838 --
 *
 * 467 instructions, THIRTY PINS of fifty-nine pinnable sites, minimal by
 * measurement from BOTH ends: dropping any survivor costs between 2 and 469,
 * and re-adding any of the twenty-eight dropped sites is 0 -- except the six
 * `__Func_8010704` sites, where a pin costs 420 to 425.
 *
 * THE SIX-ARGUMENT SITES MUST NOT BE PINNED.  `__Func_8010704(3, 0, 1, 1, 0x11,
 * 0xd)` passes two arguments on the stack, and the ROM interleaves the `str
 * [sp]` between the register fills.  A PIN4 over r0-r3 forces the four register
 * arguments into one run and the stack stores after them, which is 420
 * differing.  Bare literals reproduce the ROM's own commoning of 0x11 / 0x12 /
 * 0x13 into r9 / r10 / r8 and of 0xd into r5 with nothing spelled -- the
 * recorded "a pair of named locals per site" cure for
 * `__Func_8010704` applies to sites whose two slots DIFFER between sites, not
 * to a family that reuses them, and named locals here are 465 differing.
 *
 * THE LAST INTERLEAVE IS A POOLED ARGUMENT.  `__Func_8092a1c(0xa, 0x80 << 9,
 * (int)gScript_932__0200bd34)` leaves `ldr r2, =gScript` after the `lsl` where
 * the ROM puts it between the `mov` and the `lsl` -- 2 differing with the rest
 * exact.  Naming it IMMEDIATELY BEFORE THE CALL closes it.  Measured:
 *
 *   `sc = (int)gScript...;` on the line before the call        0   <- ships
 *   the same pinned PIN3 DESCENDING                            0
 *   PIN3 ascending                                             2
 *   an `unsigned char *` prototype, unnamed                    2
 *   `sc` assigned at the TOP of the function                 463 (+5 insns)
 *   an `unsigned char *` local assigned at the top           463
 *
 * That sharpens "Name a POOLED argument to reach an interleave": the recorded
 * precondition is only that the naming DOMINATE the call, and here every
 * position dominates -- yet hoisting it to the entry block costs five
 * instructions.  Name it adjacent to the call.
 *
 * ---------------------------------------------------------------- 2009d0c --
 *
 * 293 instructions, THIRTEEN PINS of thirty-three, a both-ends fixpoint: the
 * survivors cost 2 to 260 to drop and all twenty dropped sites re-add at 0.
 *
 * THE PIN SET IS AN EVICTION DEVICE HERE IN THE TEXTBOOK WAY.  Bare, gcc spends
 * the SAME four high registers as the ROM but on the wrong tenants: it commons
 * `0x400000`, `0x80 << 9`, `-1`, `0x13333` and `0x9999` into callee-saved
 * registers the ROM rebuilds, and then has nothing left for the byte zero the
 * ROM holds in r10.  267 -> 140 -> 67 as the pins go in; the r10 tenant
 * arrives on its own once the constants leave the commoning pool.
 *
 * ADDING THE THREE `__MapActor_SetSpeed` PINS ALONE IS 256, WORSE THAN 140.
 * Freeing r5 lets `0xa0 << 7` move straight into it -- the recorded "pins
 * compete for the same register" at full strength, and a greedy one-at-a-time
 * ADD pass would have rejected the set and parked a function that was six
 * spellings from exact.
 *
 * THE COUNTER'S ASSIGNMENT POSITION DECIDED THE WHOLE ALLOCATION.  With
 * everything else in place the residue was 27 differing at exact length, and
 * every one of them was a register RENAME: {zero, i} and {f6c value, &gKeyPress}
 * had swapped r5 and r6, and the four high registers were a permutation of the
 * ROM's.  `i = 0;` at the top of the function -> 27.  The same statement moved
 * to the line before the loop -> 2.  That is "Register allocation follows
 * ASSIGNMENT position, not declaration order" run in the OPPOSITE direction to
 * the recorded case, which moved an initialiser EARLIER; here it had to move
 * LATER.  Declaration order (`i` before `p`) is inert, and `int i` for
 * `unsigned int i` is 28.
 *
 * The final two were the counter bump, and the recorded cure applied verbatim:
 * `i++;` before `__CutsceneWait(1)` is 2 differing, `i++;` AFTER the call is
 * exact, because gcc hoists the increment above the call itself.
 *
 * MEASURED WORSE OR INERT (2009d0c, against 310 encodings / 788 bytes):
 *
 *   spelling                                              differing
 *   ---------------------------------------------------  ---------
 *   no pins at all, `while` loop                            267 (-12 insns)
 *   8 pins (80933f8 x2, 8012330 x6)                         140
 *   ... + the three SetSpeed pins ONLY                      256
 *   all 33 pinned, `while` + break                           67 (-4 insns)
 *   all 33 pinned, goto                                      75 (-2 insns)
 *   all 33 pinned, guard + do/while                          27 (length exact)
 *   ... + declaration order `i` before `p`                   27 (inert)
 *   ... + `i = 0` moved to just after __CutsceneStart        27 (inert)
 *   ... + `int i` for `unsigned int i`                       28
 *   ... + f64 / f66 store order swapped                      29
 *   ... + `i = 0` moved to just before the loop                2
 *   ... + `i++` written AFTER __CutsceneWait(1)                0
 *   dropping any one of the 13 survivors                  2 to 260
 *
 * PROTOTYPES are complete per this tree's convention.  `struct Actor *` return
 * on __MapActor_GetActor is what makes the `f5a` read-modify-write a byte op,
 * and `void (*f6c)(struct Actor *)` is what lets 2009838 and 2009d0c store
 * 2009770 into the field without a cast; a `void (*)(void)` field is
 * byte-identical but needs one.
 */
struct Actor {
    unsigned char pad0[8];
    int f8;
    int fc;
    int f10;
    int f14;
    unsigned char pad18[0x28 - 0x18];
    int f28;
    unsigned char pad2c[0x48 - 0x2c];
    int f48;
    unsigned char pad4c[0x55 - 0x4c];
    unsigned char f55;
    unsigned char pad56[0x5a - 0x56];
    unsigned char f5a;
    unsigned char f5b;
    unsigned char pad5c[0x64 - 0x5c];
    short f64;
    short f66;
    int f68;
    void (*f6c)(struct Actor *);
};

extern int gKeyPress;
extern unsigned char gScript_932__0200bd34[];

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __SetFlag(int id);
extern void __PlaySound(int id);
extern struct Actor *__MapActor_GetActor(int slot);
extern void __Actor_SetAnim(struct Actor *a, int anim);
extern void __Actor_SetSpriteFlags(struct Actor *a, int f);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_SetIdle(int slot);
extern void __Func_80933d4(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_8093530(void);
extern void __Func_8092158(int a, int b, int c);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_80925cc(int a, int b);
extern void __Func_8092a1c(int slot, int a, int b);
extern void __Func_8012330(int a, int b, int c);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void OvlFunc_932_2009770(struct Actor *a);
extern void OvlFunc_932_200ad08(void);
extern void OvlFunc_932_200abb0(int a, int b, int c, int d);

#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")
#define PIN4 PIN3; register int q3 __asm__("r3")

void OvlFunc_932_2009770(struct Actor *a)
{
    if (a->f66 != 0) {
        a->f66--;
        if (a->f66 == 1) {
            { PIN3; q0 = -1; q1 = -1; q2 = 0xe666;
              __Func_8012330(q0, q1, q2); }
        }
    }
    if (a->f28 == 0) {
        __Actor_SetAnim(a, 1);
        a->fc += 0xfffe8000;
        if (a->fc < a->f14) {
            if (a->f68 != 0) {
                __PlaySound(0xe5);
                a->f68 = 0;
                a->f66 = 4;
                { PIN3; q0 = 0x80 << 9; q1 = 0; q2 = 0x80 << 9;
                  __Func_8012330(q0, q1, q2); }
            }
            a->fc = a->f14;
        }
        a->f5b = 1;
    } else {
        a->f5b = 0;
    }
    if (a->f64 == 0) {
        __PlaySound(0x98);
        a->f68 = 1;
        __Actor_SetAnim(a, 2);
        a->f28 = 0xc0 << 10;
    }
    a->f64++;
    if (a->f64 == 0x3c) {
        a->f64 = 0;
    }
}

void OvlFunc_932_2009838(void)
{
    struct Actor *p;
    struct Actor *q;
    int sc;

    p = __MapActor_GetActor(0xa);
    __CutsceneStart();
    __Func_80933d4(0x26666, 0x4ccc);
    { PIN4; q0 = 0x95 << 17; q1 = -1; q2 = 0x1510000; q3 = 1;
      __Func_80933f8(q0, q1, q2, q3); }
    __Func_8093530();
    __PlaySound(0x93);
    __Func_80925cc(0xa, 2);
    __CutsceneWait(0x28);
    __Func_80933d4(0xcccc, 0x1999);
    __Func_80933f8(0x1270000, 0x80 << 14, 0xd4 << 16, 1);
    p->f68 = 0;
    p->f64 = 0;
    p->f66 = 0;
    p->f48 = 0x6666;
    p->f6c = OvlFunc_932_2009770;
    { PIN3; q0 = 0xa; q1 = 0x13333; q2 = 0x9999;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0xa; q1 = 0x9a << 1; q2 = 0x123;
      __Func_8092158(q0, q1, q2); }
    __Func_8092158(0xa, 0x137, 0xd7);
    p->f6c = 0;
    p->f5b = 0;
    __CutsceneWait(0x10);
    __MapActor_SetAnim(0xa, 1);
    __PlaySound(0xe5);
    { PIN3; q0 = 0x80 << 9; q1 = 0; q2 = 0x80 << 9;
      __Func_8012330(q0, q1, q2); }
    __CutsceneWait(4);
    { PIN3; q0 = -1; q1 = -1; q2 = 0xe666;
      __Func_8012330(q0, q1, q2); }
    __CutsceneWait(0x28);
    { PIN3; q0 = 0xa; q1 = 0xb0 << 8; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0xa; q1 = 0x80 << 8; q2 = 0x28;
      __Func_8092adc(q0, q1, q2); }
    OvlFunc_932_200ad08();
    __CutsceneWait(0x28);
    { PIN3; q0 = 0xa; q1 = 0xa0 << 7; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0xa; q1 = 0xc0 << 6; q2 = 0x28;
      __Func_8092adc(q0, q1, q2); }
    { PIN4; q0 = 0xa7 << 17; q1 = -1; q2 = 0xf4 << 16; q3 = 1;
      __Func_80933f8(q0, q1, q2, q3); }
    p->f68 = 0;
    p->f64 = 0;
    p->f66 = 0;
    p->f6c = OvlFunc_932_2009770;
    { PIN3; q0 = 0xa; q1 = 0xa0 << 1; q2 = 0xe8;
      __Func_8092158(q0, q1, q2); }
    { PIN3; q0 = 0xa; q1 = 0xaa << 1; q2 = 0x83 << 1;
      __Func_8092158(q0, q1, q2); }
    __Func_8092158(0xa, 0xbb << 1, 0x83 << 1);
    p->f6c = 0;
    __CutsceneWait(0x10);
    __MapActor_SetAnim(0xa, 1);
    __PlaySound(0xe5);
    { PIN3; q0 = 0x80 << 9; q1 = 0; q2 = 0x80 << 9;
      __Func_8012330(q0, q1, q2); }
    __CutsceneWait(4);
    { PIN3; q0 = -1; q1 = -1; q2 = 0xe666;
      __Func_8012330(q0, q1, q2); }
    __CutsceneWait(0x14);
    { PIN3; q0 = 0xa; q1 = 0xf0 << 8; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    __Func_8092adc(0xa, 0xd0 << 8, 0x28);
    __PlaySound(0x99);
    __MapActor_GetActor(0xa)->f28 = 0x80 << 11;
    __MapActor_SetAnim(0xa, 2);
    __Func_8092158(0xa, 0xbe << 1, 0xf8);
    __CutsceneWait(0xa);
    __PlaySound(0xe5);
    { PIN3; q0 = 0x80 << 9; q1 = 0; q2 = 0x80 << 9;
      __Func_8012330(q0, q1, q2); }
    __CutsceneWait(4);
    { PIN3; q0 = -1; q1 = -1; q2 = 0xe666;
      __Func_8012330(q0, q1, q2); }
    __CutsceneWait(6);
    __MapActor_SetAnim(0xa, 1);
    __CutsceneWait(0x28);
    { PIN3; q0 = 0xa; q1 = 0xb0 << 8; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0xa; q1 = 0x80 << 8; q2 = 0x28;
      __Func_8092adc(q0, q1, q2); }
    { PIN4; q0 = 0x98 << 17; q1 = -1; q2 = 0xd7 << 16; q3 = 1;
      __Func_80933f8(q0, q1, q2, q3); }
    p->f68 = 0;
    p->f64 = 0;
    p->f66 = 0;
    p->f6c = OvlFunc_932_2009770;
    { PIN3; q0 = 0xa; q1 = 0x149; q2 = 0xdb;
      __Func_8092158(q0, q1, q2); }
    p->f6c = 0;
    __MapActor_SetAnim(0xa, 1);
    __CutsceneWait(0x10);
    __PlaySound(0xe5);
    { PIN3; q0 = 0x80 << 9; q1 = 0; q2 = 0x80 << 9;
      __Func_8012330(q0, q1, q2); }
    __CutsceneWait(4);
    { PIN3; q0 = -1; q1 = -1; q2 = 0xe666;
      __Func_8012330(q0, q1, q2); }
    __CutsceneWait(0x28);
    __Func_8092adc(0xa, 0x80 << 8, 0x28);
    __MapActor_GetActor(9)->f55 = 0;
    __Func_8010704(3, 0, 1, 1, 0x11, 0xd);
    __Func_8010704(3, 0, 1, 1, 0x12, 0xd);
    __Func_8010704(3, 0, 1, 1, 0x13, 0xd);
    __MapActor_SetSpeed(0xa, 0x16666, 0xb333);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0xa), 0);
    __PlaySound(0x99);
    __MapActor_GetActor(0xa)->f28 = 0xa0 << 11;
    __MapActor_SetAnim(0xa, 3);
    { PIN3; q0 = 0xa; q1 = 0x127; q2 = 0xd7;
      __Func_8092158(q0, q1, q2); }
    __MapActor_SetAnim(0xa, 1);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0xa), 1);
    __PlaySound(0xe5);
    { PIN3; q0 = 0x80 << 9; q1 = 0; q2 = 0x80 << 9;
      __Func_8012330(q0, q1, q2); }
    __CutsceneWait(4);
    { PIN3; q0 = -1; q1 = -1; q2 = 0xe666;
      __Func_8012330(q0, q1, q2); }
    __CutsceneWait(0x28);
    __PlaySound(0x99);
    __MapActor_GetActor(0xa)->f28 = 0xa0 << 11;
    __MapActor_SetAnim(0xa, 3);
    { PIN3; q0 = 0xa; q1 = 0x82 << 1; q2 = 0xd7;
      __Func_8092158(q0, q1, q2); }
    __MapActor_SetAnim(0xa, 1);
    __PlaySound(0xe5);
    { PIN3; q0 = 0x80 << 9; q1 = 0; q2 = 0x80 << 9;
      __Func_8012330(q0, q1, q2); }
    __CutsceneWait(4);
    { PIN3; q0 = -1; q1 = -1; q2 = 0xe666;
      __Func_8012330(q0, q1, q2); }
    __CutsceneWait(0x28);
    { PIN3; q0 = 0xa; q1 = 0xa0 << 7; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    __Func_8092adc(0xa, 0xc0 << 6, 0x14);
    __PlaySound(0x93);
    __Func_80925cc(0xa, 2);
    __CutsceneWait(0x28);
    __Func_8010704(4, 0, 1, 1, 0x11, 0xd);
    __Func_8010704(2, 0, 1, 1, 0x12, 0xd);
    __Func_8010704(4, 0, 1, 1, 0x13, 0xd);
    q = __MapActor_GetActor(0);
    __Func_80933d4(0x4cccc, 0x9999);
    __Func_80933f8(q->f8, q->fc, q->f10, 1);
    __Func_8093530();
    sc = (int)gScript_932__0200bd34;
    __Func_8092a1c(0xa, 0x80 << 9, sc);
    __SetFlag(0x904);
    __CutsceneEnd();
}

void OvlFunc_932_2009d0c(void)
{
    struct Actor *p;
    unsigned int i;

    p = __MapActor_GetActor(0xa);
    __CutsceneStart();
    __MapActor_SetIdle(0xa);
    __Func_80933d4(0x26666, 0x4ccc);
    { PIN4; q0 = 0x1170000; q1 = 0x80 << 15; q2 = 0xd8 << 16; q3 = 1;
      __Func_80933f8(q0, q1, q2, q3); }
    __Func_8093530();
    __PlaySound(0x93);
    __Func_80925cc(0xa, 2);
    __CutsceneWait(0x28);
    { PIN3; q0 = 0xa; q1 = 0xc0 << 6; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0xa; q1 = 0xa0 << 7; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0xa; q1 = 0x80 << 8; q2 = 0x28;
      __Func_8092adc(q0, q1, q2); }
    __Func_80933d4(0xcccc, 0x1999);
    __Func_80933f8(0x80 << 16, 0x80 << 15, 0xca << 16, 1);
    p->f68 = 0;
    p->f64 = 0;
    p->f66 = 0;
    p->f48 = 0x6666;
    p->f6c = OvlFunc_932_2009770;
    { PIN3; q0 = 0xa; q1 = 0x13333; q2 = 0x9999;
      __MapActor_SetSpeed(q0, q1, q2); }
    __Func_8092158(0xa, 0xd4, 0xc8);
    __Func_8092158(0xa, 0x67, 0xc8);
    p->f6c = 0;
    p->f5b = 0;
    __CutsceneWait(0xa);
    __MapActor_SetAnim(0xa, 1);
    __PlaySound(0xe5);
    { PIN3; q0 = 0x80 << 9; q1 = 0; q2 = 0x80 << 9;
      __Func_8012330(q0, q1, q2); }
    __CutsceneWait(4);
    { PIN3; q0 = -1; q1 = -1; q2 = 0xe666;
      __Func_8012330(q0, q1, q2); }
    __CutsceneWait(0x14);
    __Func_8092adc(0xa, 0xa0 << 7, 0x28);
    __MapActor_GetActor(0xa)->f5a &= 0xfe;
    { PIN3; q0 = 0xa; q1 = 0x13333; q2 = 0x9999;
      __MapActor_SetSpeed(q0, q1, q2); }
    __Actor_SetSpriteFlags(__MapActor_GetActor(0xa), 0);
    __PlaySound(0x99);
    __MapActor_GetActor(0xa)->f28 = 0x80 << 11;
    __MapActor_SetAnim(0xa, 3);
    __Func_8092158(0xa, 0x56, 0xd6);
    __MapActor_SetAnim(0xa, 1);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0xa), 1);
    __CutsceneWait(0xa);
    __PlaySound(0xe5);
    { PIN3; q0 = 0x80 << 10; q1 = 0; q2 = 0x80 << 9;
      __Func_8012330(q0, q1, q2); }
    __CutsceneWait(8);
    { PIN3; q0 = -1; q1 = -1; q2 = 0xe666;
      __Func_8012330(q0, q1, q2); }
    __CutsceneWait(0x28);
    __MapActor_GetActor(0xa)->f5a |= 1;
    __Func_8092adc(0xa, 0xc0 << 6, 0x14);
    __Func_8092adc(0xa, 0, 0x28);
    p->f68 = 0;
    p->f64 = 0;
    p->f66 = 0;
    p->f6c = OvlFunc_932_2009770;
    { PIN3; q0 = 0xa; q1 = 0x13333; q2 = 0x9999;
      __MapActor_SetSpeed(q0, q1, q2); }
    __Func_8092158(0xa, 0x78, 0xd7);
    p->f6c = 0;
    p->f5b = 0;
    __MapActor_SetAnim(0xa, 1);
    __CutsceneWait(0x10);
    __PlaySound(0xe5);
    { PIN3; q0 = 0x80 << 9; q1 = 0; q2 = 0x80 << 9;
      __Func_8012330(q0, q1, q2); }
    __CutsceneWait(4);
    { PIN3; q0 = -1; q1 = -1; q2 = 0xe666;
      __Func_8012330(q0, q1, q2); }
    __CutsceneWait(0x28);
    __Func_8092adc(0xa, 0, 0xa);
    __PlaySound(0x93);
    __Func_80925cc(0xa, 2);
    __CutsceneWait(0x50);
    __MapActor_SetAnim(0xa, 3);
    OvlFunc_932_200abb0(0x82 << 16, 0, 0xa8 << 16, 0);
    __CutsceneWait(0x3c);
    i = 0;
    if (gKeyPress == 0) {
        do {
            __CutsceneWait(1);
            i++;
            if (i > 0x3b) {
                break;
            }
        } while (gKeyPress == 0);
    }
    p = __MapActor_GetActor(0);
    __Func_80933d4(0x4cccc, 0x9999);
    __Func_80933f8(p->f8, p->fc, p->f10, 1);
    __Func_8093530();
    __SetFlag(0x905);
    __CutsceneEnd();
}
