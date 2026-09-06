/* OvlFunc_936_200b2a4  --  0x0200b2a4
 *   [asm/overlays/rom_7c097c/ovl_30_c_c_c_c_a_a.s, 2nd of 2]
 *
 * 470 instructions of map-redraw cutscene in two independent arms, one per
 * save bit.  Each arm is guarded by `flag == 0 && (actor->x >> 19) <= limit`,
 * opens a cutscene, runs an eight-step __CopyMapTiles wipe in a loop that sets
 * its own save bit at step 0x46, then replays two ten- (or eight-) call tile
 * groups with a __CutsceneWait between them and closes with __Func_8010704 and
 * OvlFunc_936_20095b4.  Reads save bits 0x302 and 0x303; sets both.  75 call
 * sites over 11 callees, 46 of them __CopyMapTiles.
 *
 * VERDICT
 *
 *     OK OvlFunc_936_200b2a4 -- 1108 bytes, 475 encodings and 76 relocations
 *     identical
 *
 * from tools/objcmp.py, against BOTH the single-function extract
 * (scratch_elev/b245/f200b2a4/ref.s) and the ORIGINAL
 * asm/overlays/rom_7c097c/ovl_30_c_c_c_c_a_a.s.  Checked with objcmp rather
 * than tryc because tryc's `!!` warning fires here -- the reference keeps its
 * literal pool INSIDE the function, and there is a `.pool_aligned` in the
 * middle of the second arm's loop.
 *
 * NO FLAG GROUP.  `tryc.makefile_flags("src/overlays/rom_7c097c/
 * ovl_30_c_c_c_c_a_a_b.c")` returns the EMPTY set, and grepping the Makefile
 * for `rom_7c097c` finds ONE line (Makefile:814, for a different stem,
 * ovl_30_c_c_c_a_a_c_a_b).  The TU falls to the default -O2 rule, objcmp
 * prints no `(built with: ...)` line, and the pin set below is minimal with
 * respect to that empty flag group -- which is to say, unconditionally.
 *
 * THE PROLOGUE IS `push {r5, r6, r7, lr} / mov r7, r10 / mov r6, r8 /
 * push {r6, r7}` AND, READ BY CONTENT, IT NAMES FIVE LIVE VALUES, NOT FOUR.
 * r6 is the slot-8 actor then the loop's 6th argument then the tile groups'
 * 5th; r10 is the slot-9 actor, carried across the ENTIRE first arm to reach
 * the second guard; r8 is the slot-0xb actor; r5 walks the flag result, the
 * loop counter and three later constants; r7 is the loop's 5th argument.
 * Plain C holds only FOUR, and that one missing value is the whole difficulty:
 * it costs 469 of 475 encodings, 12 BYTES SHORT, and differing relocations.
 *
 * THE LEVER IS NAMING THE STACK-ARGUMENT CONSTANTS, AND THE RECORDED RULE
 * SAYS IT SHOULD NOT BE NEEDED.  docs/elevation.md:3240 ("A constant hoisted
 * across a call") records the opposite result on two functions:
 *
 *     "That reads as a named local.  It usually is not: gcc hoists a repeated
 *      constant into a callee-saved register on its own, and the literal
 *      spelling compiles to the same instructions."
 *
 * -- measured identical on OvlFunc_948_20099e8 and WORSE by three instructions
 * on OvlFunc_964_200a52c.  Here it runs the other way and it is not close.
 * Written with literals, gcc-2.96 hoists the repeats in the STRAIGHT-LINE tile
 * groups but rematerialises the two constants of each `for` body INSIDE the
 * loop (`mov r3,#5 / str r3,[sp,#4]` and `mov r6,#1` between the argument
 * movs), never allocates a fifth callee-saved register, and comes out six
 * instructions short with a two-high-register prologue where the ROM has
 * three.  Three named locals -- e, f, g, reassigned in the ROM's own order --
 * take it from 469 differing to 11 in one step.
 *
 * NEW (grepped first, see below): THE NAMING RULE HAS A FIRST-USE FORM, EXACTLY
 * LIKE THE PIN RULE.  Ten of the 49 named argument slots are load-bearing and
 * 39 are inert -- and the ten are precisely the FIRST site at which each
 * variable takes each value:
 *
 *     site  7   e = 1, f = 5     (first arm's loop)
 *     site 10   g = 2
 *     site 14   f = 1
 *     site 38   g = 0xa
 *     site 40   g = 0x2c
 *     site 47   e = 1, f = 5     (second arm's loop)
 *     site 50   f = 2
 *     site 52   g = 1
 *     site 70   g = 8
 *     site 72   h = 0x25, k = 0x2b
 *
 * Un-naming any ONE of those ten costs 3 to 469 differing; un-naming any one
 * of the other 39 is an exact TIE, and all 39 revert to literals TOGETHER at
 * zero cost.  Mechanism: once the variable's pseudo dominates, gcc commons a
 * later literal of the same value back into it, so only the assignment that
 * CREATES the pseudo matters.  That is docs/elevation.md's "ONE PIN AT THE
 * FIRST USE COVERS THE LATER ONES" arriving at a second lever.  Grepped as
 * "first use", "named local", "hoist", "stack argument", "PER CALL SITE",
 * "repeated constant" -- :3240, :6981, :8467, :8817 and :9685 all discuss
 * naming stack arguments, and none of them minimises the naming.
 *
 * The FULLY NAMED form ships anyway, and deliberately: an f/g at ten adjacent
 * identical calls and a bare `1, 2` at the eleventh is not source anybody
 * wrote, the fully named form mirrors the ROM's register file one-for-one, and
 * a named local passed as an argument is ordinary C rather than scaffolding.
 * The minimisation is recorded, not shipped.
 *
 * SECOND NEW RESULT, AND IT CONTRADICTS A MEASURED RULE: SHARING ONE SET OF
 * LOCALS ACROSS 46 CALL SITES IS AN EXACT TIE WITH ONE FRESH PAIR PER SITE.
 * docs/elevation.md:8467 "Stack arguments must be named PER CALL SITE" records
 * one shared pair across THREE six-argument calls at 20 differing against 2
 * for separate pairs, and :8514 repeats it as a rule.  Generating this function
 * with 92 locals, `s<N>`/`t<N>` assigned immediately before each of the 46
 * calls, is BYTE-IDENTICAL to the three-shared-local form.  gcc commons them
 * straight back together.  So the per-call-site rule is not general: what it
 * really turns on is whether the values are DISTINCT, and where every site
 * wants the same pair of values the two spellings converge.  This is the same
 * boundary :8817 hit from the other side ("it does NOT work when the uses are
 * far apart") -- there separate locals failed to separate; here they fail to
 * separate too, and that is what is wanted.
 *
 * THE LOOP COUNTER IS INITIALISED BEFORE THE CONSTANTS, NOT IN THE `for`.
 * `for (i = 0x43; i <= 0x4a; i++)` after `e = 1; f = 5;` emits the preheader as
 * e, f, i; the ROM emits i, e, f.  Hoisting the init out of the for-header --
 * `i = 0x43; e = 1; f = 5; for (; i <= 0x4a; i++)` -- is the fix, 11 differing
 * to 5, and it is the only difference between the two spellings.  The counter
 * must be UNSIGNED: the ROM closes the loop with `cmp r5,#0x4a / bls`, and an
 * `int` gives `ble`, 2 differing.
 *
 * ONE PIN, WIDTH ONE, AND THE HOLE TEST FOUND IT IN A TWO-ELEMENT SEARCH.
 * holes.py walks ref.s with a symbolic register file and classifies all 75
 * sites (calls.txt):
 *
 *     HOLE       44   an argument arrives by `mov rLOW, rHIGH` or from a held
 *                     register -- commoned, so a pin can only rematerialise it
 *     ALL-CHEAP  19   every argument is a bare `mov rN, #imm8`
 *     CANDIDATE   6   sites 3, 9, 12, 13, 45, 49
 *
 * Four of the six candidates (3, 9, 45, 49) are the single-argument
 * `ldr r0, =0x302` / `=0x303` flag calls, where there is no ordering to pin.
 * That leaves TWO: __MapActor_SetPos and __MapActor_SetBehavior.  Pinning
 * __MapActor_SetPos is exact; pinning __MapActor_SetBehavior COSTS 2 and
 * pinning both costs 5.  "Nominated minus the one-argument sites" was a
 * two-element search and the first element was the answer.
 *
 * The SetPos pin is minimal in WIDTH as well as in count, and here the width
 * result runs the template's way: PIN1, PIN2 and PIN3 are all three exact, so
 * only r0 is load-bearing and the shipped form names ONE register where the
 * usual PIN3 block would name three.  Mechanism, exactly as
 * src/overlays/rom_7b6668/ovl_314_c_c_a_c_c_a_a.c states it: what the site
 * needs is the slot `mov r0, #0xb` scheduled AHEAD of the expensive argument's
 * build, and naming r0 alone does that --
 *
 *     rom    mov r1,#0x96 / mov r2,#0xb6 / lsl r2,#18 / mov r0,#0xb / lsl r1,#16
 *     plain  mov r1,#0x96 / mov r2,#0xb6 / lsl r2,#18 / lsl r1,#16 / mov r0,#0xb
 *
 * -- a clean two-instruction transposition, the ordering-pin signature with
 * SIZE and RELOCATIONS silent.  THE FILL MUST BE ASCENDING AND THAT IS NOT
 * FREE HERE: a DESCENDING fill and a ROM-ORDER fill (r1, r2, r0 -- the ROM's
 * own emitted order at this site) each leave exactly the residue that dropping
 * the pin leaves, 2 differing.  Transcribing the ROM's argument window is the
 * WRONG move at this site; the recorded caution that ascending is usual but
 * not universal cuts the other way this time.
 *
 * THE PIN IS NECESSARY, NOT MERELY SUFFICIENT.  Four non-pin respellings of
 * the same site were measured (alt.py) and every one is 2 differing, identical
 * to no lever at all: naming both coordinates as locals, naming only the
 * second, naming the slot, and spelling the coordinates as `0x96 << 16` /
 * `0xb6 << 18` instead of whole values.
 *
 * AN ALL-CHEAP SITE NEEDED A LEVER, AND IT WAS NOT A PIN.  Site 72,
 * `__Func_8010704(0x25, 0xd, 0xa, 0xc, 0x25, 0x2b)`, is ALL-CHEAP by the hole
 * test and takes no ordering pin -- pinning it costs 8.  What it wants is the
 * recorded two-named-locals lever for a pair of DISTINCT stack constants
 * (docs/elevation.md:6981, :1858):
 *
 *     rom    mov r3,#0x25 / mov r2,#0x2b / str r3,[sp] / str r2,[sp,#4]
 *     plain  mov r3,#0x25 / str r3,[sp]  / mov r3,#0x2b / str r3,[sp,#4]
 *
 * Both must be named: `h` alone is 3 differing, `k` alone is 2, neither is 0.
 * This is the same distinction :3240 draws -- two DIFFERENT values in the two
 * slots force two registers, where the repeated values elsewhere in this
 * function do not.  So the classification "all-cheap needs no ORDERING PIN"
 * held exactly, and said nothing about whether the site needed a lever.
 *
 * THE ZERO AT SITE 6 IS FREE.  The ROM passes the first `__Func_8010704`'s
 * 5th argument with `str r5, [sp]`, r5 still holding the __GetFlag(0x302)
 * result that the guard has just proved zero.  Writing the literal `0` is
 * exact -- cse substitutes the available pseudo -- and hoisting the flag call
 * into `t = __GetFlag(0x302);` and passing `t` is an exact TIE.  So is
 * splitting both `&&` guards into nested `if`s.  Three spellings, one output.
 *
 * PINNING SITE 6 IS THE WORST THING IN THE SWEEP: 275 of 475 differing with
 * RELOCATIONS DIFFERING, because forcing r0..r3 there destroys exactly that
 * commoning and the whole allocation moves.  It is the loudest instance in
 * this function of "a pin at an expensive non-CSE site can HURT".
 *
 * MINIMAL BY COUNT, CONFIRMED EXHAUSTIVELY.  Every one of the 51 other
 * multi-argument sites was added to the pin set, one at a time (spec6.txt):
 * 37 are exact TIES -- every one of them a HOLE whose four register arguments
 * are bare imm8 -- and 11 are WORSE:
 *
 *     +6    275 differing, relocations differ      +43     6
 *     +10     8                                    +50     7
 *     +13     2                                    +70     8
 *     +38     8                                    +71     7
 *     +39     7                                    +72     8
 *     +40     4
 *
 * The 11 that hurt are exactly the sites where the ROM builds a stack constant
 * into r2/r3 BEFORE filling the register arguments; a PIN4 block forces r3
 * last and breaks that schedule.  Dropping the one surviving pin costs 2.
 *
 * TEARDOWN.  Every lever removed from the finished file and re-measured:
 *
 *     the three constant locals e/f/g   469 of 475 differing, 1096 bytes,
 *                                       12 SHORT, relocations differ
 *     naming only the LOOP constants    405 differing, 1104 bytes, 4 SHORT
 *     naming only the STRAIGHT-LINE     469 differing, 1092 bytes, 16 SHORT
 *       constants
 *     the second arm's `e = 1;`         270 differing, relocations differ
 *       (it looks redundant after the
 *        first arm and is not: the
 *        arms are disjoint)
 *     `i = 0x43;` out of the for-head     6 differing
 *     the SetPos pin                      2 differing
 *     descending fill at the pin          2 differing
 *     ROM-order fill at the pin           2 differing
 *     `h` at site 72                      2 differing
 *     `k` at site 72                      3 differing
 *     both h and k                        3 differing
 *     `int i` instead of `unsigned int`   2 differing
 *     `int *` actor pointers              4 differing
 *
 * And these are exact TIES, so the set is minimal but not unique: PIN2 or PIN3
 * at the SetPos site; nested `if`s for both guards; a named `t` for the flag
 * result; 92 per-call-site locals instead of three shared ones; 39 of the 49
 * named argument slots reverted to literals, individually and all together;
 * and a pin at any of the 37 all-cheap hole sites.
 *
 * MINIMISED TO A FIXPOINT.  Six levers survive -- the constant locals, the
 * loop-counter init placement, the unsigned counter, the pointer type, the
 * one-register SetPos pin, and the h/k pair at site 72.  Each removal fails,
 * and the second pass over the survivors finds every one still required.
 *
 * LANDING NEEDS A SPLIT.  `grep -c thumb_func_start` on
 * asm/overlays/rom_7c097c/ovl_30_c_c_c_c_a_a.s is 2: OvlFunc_936_200b1b8 (100
 * instructions, untouched) and this function, second of the two.  A .o is
 * built from one source file, so tools/split_s.py must cut the .s first:
 *
 *     python3 tools/split_s.py \
 *         asm/overlays/rom_7c097c/ovl_30_c_c_c_c_a_a.s OvlFunc_936_200b2a4
 *
 * leaves OvlFunc_936_200b1b8 in ovl_30_c_c_c_c_a_a_a.s and this function in
 * ovl_30_c_c_c_c_a_a_b.s, and rewrites the linker script.  Neither name
 * collides: `ls asm/overlays/rom_7c097c/` has no ..._a_a_a and no ..._a_a_b.
 * split_s.py is byte-neutral by construction, so `make compare` must be green
 * AFTER the split and BEFORE this .c is added -- check that first, because a
 * layout mistake and a bad decompilation look identical at the end.
 *
 * EXACTLY ONE LINKER LINE NAMES THE OBJECT, matched on FULL PATH across the
 * whole tree (four other overlays have a same-named .o and are NOT this one):
 *
 *     overlays/rom_7c097c/overlay.ld:54
 *         asm/overlays/rom_7c097c/ovl_30_c_c_c_c_a_a.o(.text)
 *
 * `.text` is the object's only section.  The .s emits NO `.section`, `.data`,
 * `.rodata`, `.bss`, `.word`, `.byte`, `.short`, `.hword`, `.align` or
 * `.global` line anywhere -- `grep -oE '^\s*\.[a-z_]+'` over the whole file
 * returns exactly `.include` x1, `.thumb_func_start` x2, `.func_end` x2 and
 * `.pool_aligned` x1, that last one inside this function -- and the `.data` and `.bss` blocks of
 * that same script (overlay.ld:66 and :70) name only
 * asm/overlays/rom_7c097c/ovl_30_c_c_c_c_c_c_b.o, at :67 and :71.  So there is no section to
 * remap and no .data or .rodata line to add; split_s.py rewriting line 54 into
 * the _a and _b pair is the entire linker edit.  The .o PATH DOES NOT MOVE:
 * the tree default `asm/%.o: src/%.c` (Makefile:146) builds
 * asm/overlays/rom_7c097c/ovl_30_c_c_c_c_a_a_b.o from
 * src/overlays/rom_7c097c/ovl_30_c_c_c_c_a_a_b.c, which is why the script keeps
 * an `asm/` path for elevated C -- do not rewrite that line to a src/ path.
 * The only other tree hits for the .o basename are the .text lines of FOUR
 * other overlays' own linker scripts (rom_79dd90, rom_7a1ff0, rom_7a7298,
 * rom_7e0928) and PROSE in EIGHT other overlays' header comments; none is
 * this object and none moves.
 *
 * -- worked in scratch_elev/b245/f200b2a4.  gen.py rebuilds the file from the
 *    site table for any lever combination, sweep.py runs a whole sweep in ONE
 *    container invocation, holes.py prints the hole classification, alt.py the
 *    non-pin respellings of the SetPos site, and verify.py rebuilds the shipped
 *    .c and re-checks it against both reference paths.
 */
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int z);
extern void __MapActor_SetBehavior(int slot, unsigned char *s);
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void OvlFunc_936_20095b4(void);
extern unsigned char gScript_936__0200c268[];

#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")
#define PIN4 PIN3; register int q3 __asm__("r3")

void OvlFunc_936_200b2a4(void)
{
    unsigned char *actor8;
    unsigned char *actor9;
    unsigned char *actorb;
    unsigned int i;
    int e;
    int f;
    int g;
    int h;
    int k;

    actor8 = __MapActor_GetActor(8);
    actor9 = __MapActor_GetActor(9);
    if (__GetFlag(0x302) == 0 && (*(int *)(actor8 + 8) >> 19) <= 0x1d) {
        actorb = __MapActor_GetActor(0xb);
        __CutsceneStart();
        __Func_8010704(7, 0x2c, 1, 1, 0, 1);
        i = 0x43;
        e = 1;
        f = 5;
        for (; i <= 0x4a; i++) {
            __CopyMapTiles(i, 0x3a, 0x4e, 0x29, e, f);
            __CutsceneWait(4);
            if (i == 0x46)
                __SetFlag(0x302);
        }
        g = 2;
        __CopyMapTiles(0x10, 0x6d, 0xd, 0x6d, 3, g);
        __CutsceneWait(0x28);
        *(int *)(actorb + 0x18) = 0x1999;
        *(int *)(actorb + 0x1c) = 0x1999;
        { PIN1; q0 = 0xb;
          __MapActor_SetPos(q0, 0x960000, 0x2d80000); }
        __MapActor_SetBehavior(0xb, gScript_936__0200c268);
        f = 1;
        __CopyMapTiles(0x43, 0x40, 0x47, 0x2c, f, g);
        __CopyMapTiles(0x43, 0x40, 0x48, 0x2c, f, g);
        __CopyMapTiles(0x43, 0x44, 0x49, 0x2b, f, g);
        __CopyMapTiles(0x43, 0x44, 0x4a, 0x2b, f, g);
        __CopyMapTiles(0x43, 0x40, 0x4b, 0x2c, f, g);
        __CopyMapTiles(0x43, 0x42, 0x4c, 0x2c, f, g);
        __CopyMapTiles(0x43, 0x40, 0x4d, 0x2c, f, g);
        __CopyMapTiles(0x43, 0x40, 0x4e, 0x2c, f, g);
        __CopyMapTiles(0x43, 0x40, 0x4f, 0x2c, f, g);
        __CopyMapTiles(0x43, 0x42, 0x50, 0x2c, f, g);
        __CopyMapTiles(2, 0, 9, 0x2a, g, g);
        __CutsceneWait(0x28);
        __CopyMapTiles(0x44, 0x40, 0x47, 0x2c, f, g);
        __CopyMapTiles(0x44, 0x40, 0x48, 0x2c, f, g);
        __CopyMapTiles(0x44, 0x44, 0x49, 0x2b, f, g);
        __CopyMapTiles(0x44, 0x44, 0x4a, 0x2b, f, g);
        __CopyMapTiles(0x44, 0x40, 0x4b, 0x2c, f, g);
        __CopyMapTiles(0x44, 0x42, 0x4c, 0x2c, f, g);
        __CopyMapTiles(0x44, 0x40, 0x4d, 0x2c, f, g);
        __CopyMapTiles(0x44, 0x40, 0x4e, 0x2c, f, g);
        __CopyMapTiles(0x44, 0x40, 0x4f, 0x2c, f, g);
        __CopyMapTiles(0x44, 0x42, 0x50, 0x2c, f, g);
        __CopyMapTiles(4, 0, 9, 0x2a, g, g);
        __CutsceneWait(0x28);
        g = 0xa;
        __CopyMapTiles(7, 0xb, 7, 0x2a, g, 8);
        __CopyMapTiles(0x47, 0xc, 0x47, 0x2b, g, 0xd);
        g = 0x2c;
        __Func_8010704(6, 0xd, 0xc, 0xc, 6, g);
        __CutsceneWait(0x28);
        OvlFunc_936_20095b4();
        __Func_8010704(0, 1, 1, 1, 7, g);
        __CutsceneEnd();
    }
    if (__GetFlag(0x303) == 0 && (*(int *)(actor9 + 8) >> 19) <= 0x57) {
        __CutsceneStart();
        i = 0x43;
        e = 1;
        f = 5;
        for (; i <= 0x4a; i++) {
            __CopyMapTiles(i, 0x3a, 0x6b, 0x29, e, f);
            __CutsceneWait(4);
            if (i == 0x46)
                __SetFlag(0x303);
        }
        f = 2;
        __CopyMapTiles(0x2d, 0x6d, 0x2a, 0x6d, 3, f);
        __CutsceneWait(0x28);
        g = 1;
        __CopyMapTiles(0x43, 0x40, 0x66, 0x2c, g, f);
        __CopyMapTiles(0x43, 0x40, 0x67, 0x2c, g, f);
        __CopyMapTiles(0x43, 0x40, 0x68, 0x2c, g, f);
        __CopyMapTiles(0x43, 0x42, 0x69, 0x2c, g, f);
        __CopyMapTiles(0x43, 0x40, 0x6a, 0x2c, g, f);
        __CopyMapTiles(0x43, 0x40, 0x6b, 0x2c, g, f);
        __CopyMapTiles(0x43, 0x40, 0x6c, 0x2c, g, f);
        __CopyMapTiles(0x43, 0x42, 0x6d, 0x2c, g, f);
        __CutsceneWait(0x28);
        __CopyMapTiles(0x44, 0x40, 0x66, 0x2c, g, f);
        __CopyMapTiles(0x44, 0x40, 0x67, 0x2c, g, f);
        __CopyMapTiles(0x44, 0x40, 0x68, 0x2c, g, f);
        __CopyMapTiles(0x44, 0x42, 0x69, 0x2c, g, f);
        __CopyMapTiles(0x44, 0x40, 0x6a, 0x2c, g, f);
        __CopyMapTiles(0x44, 0x40, 0x6b, 0x2c, g, f);
        __CopyMapTiles(0x44, 0x40, 0x6c, 0x2c, g, f);
        __CopyMapTiles(0x44, 0x42, 0x6d, 0x2c, g, f);
        __CutsceneWait(0x28);
        g = 8;
        __CopyMapTiles(0x26, 0xe, 0x26, 0x2c, g, 4);
        __CopyMapTiles(0x66, 0xe, 0x66, 0x2c, g, 0xc);
        h = 0x25;
        k = 0x2b;
        __Func_8010704(0x25, 0xd, 0xa, 0xc, h, k);
        __CutsceneWait(0x28);
        OvlFunc_936_20095b4();
        __CutsceneEnd();
    }
}
