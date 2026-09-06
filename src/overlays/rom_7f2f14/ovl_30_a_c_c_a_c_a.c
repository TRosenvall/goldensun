/* OvlFunc_968_20089c8
 *   [asm/overlays/rom_7f2f14/ovl_30_a_c_c_a_c_a.s, lines 13-142 of 142.  THE
 *   FILE HOLDS EXACTLY ONE FUNCTION -- `grep -c thumb_func_start` returns 1 --
 *   AND HAS NO `.section` DIRECTIVE OF ANY KIND: `grep -n "\.section\|\.global\|
 *   incbin"` on it returns NOTHING.  So there is no .data/.rodata/.bss blob to
 *   strand and NO SPLIT IS NEEDED; tools/split_s.py is not involved.
 *
 *   EXACTLY ONE .ld LINE IN THE WHOLE TREE NAMES THIS .o, and it is on the
 *   FULL PATH (`grep -rn "ovl_30_a_c_c_a_c_a\.o" --include="*.ld" .` returns
 *   this and nothing else):
 *
 *     overlays/rom_7f2f14/overlay.ld:40  asm/overlays/rom_7f2f14/ovl_30_a_c_c_a_c_a.o(.text)
 *
 *   That line becomes `src/overlays/rom_7f2f14/ovl_30_a_c_c_a_c_a.o(.text)` and
 *   the .s is deleted.  THE .data LIST OF THAT SAME .ld (lines 101-104) NAMES
 *   ONLY ovl_30_c_c_c_c_{a,b,c}.o and must not be touched, and the file has no
 *   .rodata and no .bss list at all -- so no section named for this .o is left
 *   unaccounted, which is the hazard the sibling headers flag ("an .ld entry
 *   matching no input section is NOT an error, so aiming it wrong drops the
 *   blob silently").  The eight `ovl_30_a_c_c_a_c_a*` lines in
 *   overlays/rom_7ed0a0/overlay.ld are a DIFFERENT OVERLAY and different stems
 *   (`_a_b`, `_a_c_b`, ...); matching on the full path is what separates them.
 *
 *   NO NAME COLLISION: asm/overlays/rom_7f2f14 already holds
 *   ovl_30_a_c_c_a_c_{a,b,c}, and only the `_a` stem moves.]
 *
 * EXACT, measured as a single-function extract against the ORIGINAL asm/ path
 * (the .s holds one function, so --func isolates it on the reference side):
 *
 *   OK OvlFunc_968_20089c8 -- 320 bytes, 133 encodings and 23 relocations identical
 *
 * Reproduced on two re-runs and against a scratch copy of the same .s.  objcmp
 * prints NO `(built with: ...)` line -- adjust=set(), the tree default
 * -O2 -mthumb -mthumb-interwork -fcall-used-r4.  NO FLAG GROUP IS NEEDED AND
 * NONE CAN ARRIVE BY ACCIDENT: every rom_7f2f14 pattern rule in the Makefile is
 * scoped to `ovl_30_c_a_c_a_c_a%` or `ovl_30_c_a_c_a_c_c%`, and this stem is
 * `ovl_30_a_c_c_a_c_a` -- it diverges at the first component after `ovl_30_`.
 * The only rules that can reach it are the generic `%.o: %.s`, `%.o: %.c` and
 * `asm/%.o: src/%.c`.
 *
 * 127 instructions: a cutscene opener, a 17-iteration cos/sin ring loop, and a
 * camera/fade tail.  It is the THIRD member of the ring-loop family solved from
 * src/overlays/rom_7a5214/ovl_17ec_c_c_b.c (OvlFunc_918_20098b8), whose script
 * it repeats with one extra `__CutsceneWait(0x1e)` before `__PlaySound(0xcc)`,
 * one extra `__Func_809202c()` in the tail, and -- the one structural
 * difference -- A CACHED ACTOR POINTER.  `tools/solved_twins.py` reports ZERO
 * twins (0 across 0 templates); the family was reached from the template's own
 * header rather than from the twin script.
 *
 * ---------------------------------------------------------- CONFIRMATION ----
 * "A REPEATED ACCESSOR CALL IS A SOURCE-LEVEL REPEAT -- AND IT IS PER-FUNCTION"
 * READ FROM THE OTHER SIDE, AND THE WRONG DIRECTION IS **LONG**.  The recorded
 * entry (docs/elevation.md, "COUNT THE `bl`s BETWEEN THE STORES") is written
 * from functions that DO NOT cache, where the cached spelling is 117 differing
 * and eight instructions SHORT.  This function caches -- one
 * `bl __MapActor_GetActor` at the very top, r7 held across the entire body,
 * ZERO further accessor calls -- and the mis-spelling therefore runs the other
 * way:
 *
 *     a = __MapActor_GetActor(0); a->f48 = ...          0
 *     __MapActor_GetActor(0)->f48 = ... at every store  131 differing, 36 bytes LONG
 *     accessor at the stores, `a` named only for loop   132 differing, 28 bytes LONG
 *
 * So the recogniser is symmetric and worth stating as a pair: an uncached
 * candidate against a caching ROM is LONG by one `mov r0,#0`/`bl` per store
 * with EXTRA `__MapActor_GetActor` relocations, exactly as a cached candidate
 * against a repeating ROM is SHORT with MISSING ones.  Either way the count of
 * `bl`s in the ROM decides, and it decided differently here than in the
 * template three files away in the same family.
 *
 * The ROM ALSO fetches the pointer BEFORE `__CutsceneStart()`, not after, and
 * that is load-bearing on its own: moving `a = __MapActor_GetActor(0);` one
 * statement later costs 8.
 *
 * ------------------------------------------------------------------- NEW ----
 * A `void f(void)` PROTOTYPE HAS NO ARGUMENTS TO ORDER, IS STILL LOAD-BEARING,
 * AND REACHES **BACKWARD** TO THE PRECEDING CALL.
 *
 * Grepped docs/elevation.md by concept first, in the corpus's vocabulary:
 * "return type" (18 hits), "preceding call", "no-argument", "zero-argument",
 * "call_value", "implicit int".  What is recorded is the FORWARD rule --
 * "Declare every callee" (line 1050): "the deferral is caused by the
 * *preceding* call's return type", i.e. callee C's return type decides how the
 * call AFTER C fills its argument registers -- plus its refinement "There are
 * TWO declaration levers" (the mismatching call's OWN declaration reorders its
 * OWN fill) and the table at line 3186 proving the PARAMETER LIST is
 * irrelevant.  Nothing covers a callee that takes NO ARGUMENTS acting on a call
 * that comes BEFORE it, which is what three of this function's declarations do.
 *
 * Three tail callees take no arguments and return nothing.  Dropping any one of
 * them costs 2, and the two differing encodings are never at its own site:
 *
 *   declaration dropped        residue (objdump index)            site it breaks
 *   -------------------------  ---------------------------------  -----------------------------
 *   __Func_8012350(void)       104/105 negs r0,r0 / negs r1,r1    __Func_8012330(-1,-1,0xe666)
 *                                                                 -- the call IMMEDIATELY BEFORE it
 *   __Func_809202c(void)       109/110 movs r0,#0 / lsls r1,#1    __MapActor_Surprise(0, 0x80<<1)
 *                                                                 -- the call IMMEDIATELY BEFORE it
 *   __CutsceneEnd(void)        109/110, the same two              __MapActor_Surprise(0, 0x80<<1)
 *                                                                 -- TWO calls further back
 *
 * IT IS THE RETURN TYPE, NOT THE MISSING LINE.  Isolated exactly as the
 * recorded table isolates the forward lever:
 *
 *   extern void __CutsceneEnd(void);    0   <- what ships
 *   extern void __CutsceneEnd();        0       (empty parameter list, still void)
 *   extern int  __CutsceneEnd(void);    2       same residue as deleting it
 *   no declaration at all               2
 *
 * The effects are INDEPENDENT AND ADDITIVE: dropping all three at once is 4,
 * the union of the two residues, not 2 and not 6.
 *
 * MECHANISM.  Declared `int`, the call becomes a `call_value` insn carrying an
 * explicit SET of r0 instead of a bare CLOBBER.  A SET is a real definition, so
 * sched2 sees an output dependence on r0 that a CLOBBER does not create, and it
 * loses the freedom to swap the `negs r0` / `negs r1` (or `movs r0,#0` /
 * `lsls r1`) pair of the argument fill that PRECEDES the call.  The forward
 * lever is about r0 being LIVE ACROSS a call; this is about r0 being DEFINED AT
 * one, and liveness runs forward while a scheduling dependence constrains both
 * ends of the edge.
 *
 * TWO PRACTICAL CONSEQUENCES.  (1) A PIN DOES NOT IMMUNISE A SITE AGAINST THIS:
 * both broken sites are already pinned PIN3/PIN2 ascending and break anyway --
 * a pin constrains which pseudo lands in which hard register, not the schedule
 * of a later call's r0 definition.  (2) The same shape of declaration is inert
 * elsewhere in the SAME function: `__CutsceneStart`, `__Func_800fe9c`,
 * `__MapTransitionIn` and `__WaitMapTransition` are all `void f(void)` in the
 * opening block and all four are inert, as is retyping `__CutsceneStart` to
 * `int`.  That is "PROTOTYPES ARE PER-SITE, NOT PER-FILE" sharpened to its
 * strongest form -- per-site for ONE declaration shape inside ONE function.
 *
 * PROTOTYPES, 6 OF 18 LOAD-BEARING: __Func_8012330 5, __MapActor_Surprise 4,
 * __MapActor_SetAnim 2, __CutsceneEnd 2, __Func_8012350 2, __Func_809202c 2.
 * The other twelve are inert and stay declared because a complete prototype
 * list is this tree's convention; OvlFunc_968_200896c's cannot be dropped at
 * all (it types the function-pointer field store).
 *
 * ------------------------------------------------------ CONFIRMATION --------
 * FIVE PINS, ALL FIVE LOAD-BEARING, MINIMAL BY MEASUREMENT.  Every pinnable
 * constant-argument site was pinned, stripped greedily with a re-test after
 * each drop, then re-grown with an ADD pass, to a fixpoint from both ends.  No
 * site drops:
 *
 *   site  call                                        cost of dropping it
 *   ----  ------------------------------------------  -------------------
 *      0  __Func_80933f8(-1, -1, -1, 0)                     120  (4 bytes short)
 *      3  __Func_8012330(-1, -1, 0xe666)                     34  (4 bytes short)
 *      2  __Func_8012330(0xa0<<11, 0xa0<<11, 0x80<<9)        29  (4 bytes short)
 *      1  __MapActor_Surprise(0, 0x101)                       2
 *      4  __MapActor_Surprise(0, 0x80 << 1)                   2
 *
 * SITE 4 IS THE ONE THE TEMPLATE SAYS DOES NOT EXIST, and it is why the first
 * draft was 2 differing.  On OvlFunc_918_20098b8 a pin at
 * __MapActor_Surprise(0, 0x80<<1) is INERT and explicitly listed under "ADD
 * PASS, all inert"; on OvlFunc_968_200ca2c it is worth 2; here it is worth 2 in
 * a function that otherwise transcribes the template's pin set exactly.  Treat
 * every template lever as SUFFICIENT NOT NECESSARY, in the ADD direction too:
 * the site the template proved needs nothing is the site this one needs.
 *
 * The residue it cures is the recorded "PINNING r0 ALONE ORDERS A POOLED r1"
 * shape with the pool side replaced by a two-instruction constant, and the
 * lever survives the substitution:
 *
 *     ROM   movs r0,#0    / movs r1,#0x80 / lsls r1,#1
 *     ours  movs r1,#0x80 / lsls r1,#1    / movs r0,#0
 *
 *     PIN1, q0 = 0 only, 0x80<<1 passed bare      0   <- the minimal cure
 *     PIN2, both filled ascending                 0   <- what ships (uniform fill)
 *     PIN2 filled descending (q1 then q0)         2   -- unchanged
 *     no pin                                      2
 *
 * So the entry's claim generalises past its own wording: what matters is that
 * the OTHER argument has no bare constant left for CSE to re-home, and a
 * `mov`+`lsl` pair qualifies as surely as a pool load does.  Pinning the cheap
 * r0 alone is again a COMPLETE cure.
 *
 * UNIFORM ASCENDING FILL, one statement per argument, whole value per
 * statement, at all five.  NO SITE WANTS DESCENDING and every one of them is
 * measurably worse for it: 80933f8 2, Surprise(0x101) 2, 8012330(0xa0<<11) 4,
 * 8012330(-1,-1,0xe666) 4, Surprise(0x80<<1) 2.  Three of the five fills are
 * WIDER THAN THE MINIMUM and that is deliberate uniformity -- PIN2 suffices at
 * __Func_80933f8 (PIN1 is 120), PIN1 at __Func_8012330(-1,-1,0xe666) and at
 * both __MapActor_Surprise sites.  ONLY __Func_8012330(0xa0<<11, ...) NEEDS ITS
 * FULL PIN3: PIN1 and PIN2 there are both 44.
 *
 * ADD PASS, all inert (tie at 0, so they do not ship): __MapActor_SetAnim(0,
 * 0x16), __PlaySound(0xbc), __CutsceneWait(0x1e), __WaitFrames(1).  The
 * recorded "AN ALL-CHEAP CALL SITE NEEDS NO ORDERING PIN".
 *
 * ------------------------------------------------------ CONFIRMATION --------
 * DECLARATION ORDER IS LOAD-BEARING, AND THE CONSTRAINT IS EXACTLY ONE PAIR.
 * The sibling OvlFunc_968_200ca2c records all 5040 permutations of its seven
 * locals measuring the same.  Here twelve random permutations of the six split
 * cleanly, and the rule is "Frame layout follows DECLARATION ORDER,
 * last-declared lowest" with nothing else in it:
 *
 *   `struct Cfg s` declared before `int v[3]`   0   (5 of 12, every one)
 *   `int v[3]` declared before `struct Cfg s`   2   (7 of 12, every one)
 *
 * The positions of `a`, `i`, `ang` and `z` are completely free -- they live in
 * registers.  The frame is `sub sp, #0x44` = 16 bytes of outgoing argument area
 * + 12 for `v` at sp+0x10 + 40 for `s` at sp+0x1c, so `s` must be declared
 * FIRST to land higher.  Declaration order is worth trying here and was inert
 * on the sibling; what separates the two cases is whether any local is
 * addressable at all.
 *
 * WHAT NEEDED NOTHING.  The 17-iteration loop as a plain
 * `for (i = 0; i <= 0x10; i++)` over an `unsigned int` -- the ROM's `bls` is
 * what makes it unsigned, and `int` costs exactly 1 encoding (the branch);
 * `v[0] + v[0] / 2` written out, with gcc's own `lsr #31 / add / asr #1`
 * rounding falling out of the signed division by two; the four
 * `0x82<<16`-style two-instruction constants as plain shifted literals;
 * `0x1090001` and `0xcccc` inline; and the THREE HIGH REGISTERS gcc reaches on
 * its own with no pin at all -- r8 for the loop counter, r10 for `&s`, plus the
 * r6 that holds `&v` -- confirming again that r8/r10 are not a wall.
 *
 * MEASURED WORSE (against 320 bytes / 133 encodings):
 *
 *   spelling                                                   differing
 *   --------------------------------------------------------  ---------
 *   `a` uncached: __MapActor_GetActor(0) at every store          131 (36 bytes long)
 *   accessor at the stores, `a` named only for the loop          132 (28 bytes long)
 *   `int v0, v1, v2;` instead of `int v[3]`                      122 (4 bytes short)
 *   drop the __Func_80933f8 pin                                  120 (4 bytes short)
 *   PIN1 (q0 only) at __Func_80933f8                             120 (4 bytes short)
 *   `s.f00 = 1;` added (the sibling's shape)                     116 (4 bytes long)
 *   `v[0] = v[0] * 3 / 2` for `v[0] + v[0] / 2`                   73
 *   PIN1 or PIN2 instead of PIN3 at __Func_8012330(0xa0<<11,...)  44 (4 bytes short)
 *   one local `t0` for both 0xa0<<11 arguments                    40 (4 bytes short)
 *   drop the __Func_8012330(-1, -1, 0xe666) pin                   34 (4 bytes short)
 *   drop the __Func_8012330(0xa0<<11, ...) pin                    29 (4 bytes short)
 *   `a = __MapActor_GetActor(0)` moved after __CutsceneStart()     8
 *   `s.f24 = ...` written after the two 0xcccc stores              7
 *   `v[1] = 0;` before `v[0] = __cos(ang);`                        5
 *   drop the __Func_8012330 prototype                              5
 *   drop the __MapActor_Surprise prototype                         4
 *   descending fill at either __Func_8012330 site                  4
 *   `int v[3];` declared before `struct Cfg s;`                    2
 *   drop either __MapActor_Surprise pin                            2
 *   descending fill at either __MapActor_Surprise or __Func_80933f8  2
 *   drop the __MapActor_SetAnim / __CutsceneEnd / __Func_8012350
 *     / __Func_809202c prototype (each, alone)                     2
 *   loop counter typed `int`                                       1
 *
 *   INERT (tie at 0, so the plainer or more uniform form ships):
 *     `for (i = 0; i < 0x11; i++)`;  `v[0] += v[0] / 2`
 *     `ang` inlined into both calls (no `ang` local)
 *     `z = 0` dropped, literal 0 at both f44 and f55; or `f44 = 0` + `f55 = z`
 *     `unsigned char *q = &a->f55` for the two byte stores
 *     the f55 stores through an explicit `(unsigned char *)((char *)a + 0x55)`
 *     `int f24;` + a cast instead of the function-pointer field
 *     `0x50000` / `0x10000` for `0xa0 << 11` / `0x80 << 9`
 *     PIN1/PIN2 reductions at four of the five pinned sites (see above)
 *     adding a pin at __MapActor_SetAnim, __PlaySound, __CutsceneWait or
 *       __WaitFrames
 *     `extern void __CutsceneEnd();` (empty parameter list, void return)
 *     every declaration order with `s` before `v` (5 of 12 sampled)
 *
 * FLAGS: NO FLAG GROUP -- and none is reachable, see the Makefile paragraph
 * above.  Two passes are live and both are ON by default at -O2:
 * -fno-schedule-insns2 is 49 differing and -fno-gcse is 130 (8 bytes short,
 * relocations differ).  -fno-schedule-insns, -fno-cse-follow-jumps,
 * -fno-rerun-cse-after-loop, -fno-expensive-optimizations, -fno-strict-aliasing
 * and -fno-caller-saves are ALL BYTE-IDENTICAL, so like its rom_7f2f14 sibling
 * this TU turns on no alias set and needs no caller-save spill.
 */
struct Actor {
    unsigned char pad00[8];
    int x;
    int y;
    int z;
    unsigned char pad14[0x30];
    int f44;
    int f48;
    unsigned char pad4c[9];
    unsigned char f55;
};

struct Cfg {
    int f00;
    int f04;
    int f08;
    int f0c;
    unsigned char pad10[0x14];
    void (*f24)(void);
};

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __WaitFrames(int n);
extern void __MapTransitionIn(void);
extern void __WaitMapTransition(void);
extern void __PlaySound(int id);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_800fe9c(void);
extern struct Actor *__MapActor_GetActor(int slot);
extern void __MapActor_Surprise(int a, int b);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __Func_8012330(int a, int b, int c);
extern void __Func_8012350(void);
extern void __Func_809202c(void);
extern int __cos(int a);
extern int __sin(int a);
extern void OvlFunc_968_200896c(void);
extern void OvlFunc_968_2008118(int x, int y, int z, int a, int b, int c, int d, struct Cfg *s);

#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")
#define PIN4 PIN3; register int q3 __asm__("r3")

void OvlFunc_968_20089c8(void)
{
    struct Cfg s;
    int v[3];
    struct Actor *a;
    unsigned int i;
    int ang;
    int z;

    a = __MapActor_GetActor(0);
    __CutsceneStart();
    { PIN4; q0 = -1; q1 = -1; q2 = -1; q3 = 0;
      __Func_80933f8(q0, q1, q2, q3); }
    __Func_800fe9c();
    __WaitFrames(1);
    a->y = 0x82 << 16;
    a->f48 = 0x80 << 8;
    z = 0;
    a->f44 = z;
    a->f55 = z;
    __MapTransitionIn();
    __WaitMapTransition();
    __CutsceneWait(0x1e);
    __PlaySound(0xcc);
    a->f55 = 3;
    __CutsceneWait(0x18);
    s.f04 = 7;
    s.f24 = OvlFunc_968_200896c;
    s.f08 = 0xcccc;
    s.f0c = 0xcccc;
    for (i = 0; i <= 0x10; i++) {
        ang = i << 12;
        v[0] = __cos(ang);
        v[1] = 0;
        v[2] = __sin(ang);
        v[0] = v[0] + v[0] / 2;
        OvlFunc_968_2008118(a->x, a->y, a->z, v[0], v[1], v[2], 0x1090001, &s);
    }
    __PlaySound(0xbc);
    { PIN2; q0 = 0; q1 = 0x101;
      __MapActor_Surprise(q0, q1); }
    __MapActor_SetAnim(0, 0x16);
    { PIN3; q0 = 0xa0 << 11; q1 = 0xa0 << 11; q2 = 0x80 << 9;
      __Func_8012330(q0, q1, q2); }
    { PIN3; q0 = -1; q1 = -1; q2 = 0xe666;
      __Func_8012330(q0, q1, q2); }
    __Func_8012350();
    { PIN2; q0 = 0; q1 = 0x80 << 1;
      __MapActor_Surprise(q0, q1); }
    __Func_809202c();
    a->f48 = 0x80 << 9;
    a->f44 = 0x80 << 7;
    __CutsceneEnd();
}
