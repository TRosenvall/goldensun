/* OvlFunc_928_20085f4  --  0x020085f4
 *   [asm/overlays/rom_7b6668/ovl_314_c_c_a_c_c_a_a.s, 1st of 1]
 *
 * 331 instructions of straight-line cutscene: 74 call sites, one save-flag
 * fork at the top whose short arm re-arms the actor callback and ends the
 * scene early, and a long arm with three position guards, a fade loop and a
 * conditional walk-off.  Message bases 0x17fa / 0x17fb; reads and sets save
 * bit 0x200.
 *
 * LANDING NEEDS NOTHING BUT THE FILE, AND NO LINKER EDIT AT ALL.  The .s holds
 * exactly ONE function (`grep -c thumb_func_start` = 1).  Grepped across the
 * whole tree on FULL PATH, exactly one linker line names the object:
 *
 *     overlays/rom_7b6668/overlay.ld:34
 *         asm/overlays/rom_7b6668/ovl_314_c_c_a_c_c_a_a.o(.text)
 *
 * `.text` is the object's only section -- the .s emits no `.section`, `.data`,
 * `.rodata`, `.bss`, `.word`, `.byte` or `.align` line anywhere -- and the
 * `.data` block of that same script names only `asm/overlays/common/
 * common0_c.o` and `asm/overlays/rom_7b6668/ovl_314_c_c_c.o`, so there is no
 * section to remap and no line to add.  The .o PATH DOES NOT MOVE either: the
 * tree default `asm/%.o: src/%.c` (Makefile:146) builds `asm/.../X.o` from
 * `src/.../X.c`, which is why the already-landed sibling
 * src/overlays/rom_7b6668/ovl_314_c_c_a_c_c_a_b.c leaves overlay.ld:21 saying
 * `asm/...`.  Landing is: add this .c, delete the .s.  The only other tree
 * hits for the .o name are PROSE inside two files' header comments.
 *
 * NO FLAG GROUP.  `tryc.makefile_flags("src/overlays/rom_7b6668/
 * ovl_314_c_c_a_c_c_a_a.c")` returns the EMPTY set; no Makefile line and no
 * wildcard reaches this stem, so the TU falls to the default rule at -O2.
 * objcmp run against the ORIGINAL asm/ path prints no `(built with: ...)`
 * line.  The match does NOT depend on any flag, so the pin set below is
 * minimal with respect to the flags it ships under.
 *
 * THE PROLOGUE IS `push {r5, r6, r7, lr}` AND, READ BY CONTENT, ALL THREE
 * CALLEE-SAVED REGISTERS ARE REAL VARIABLES -- r6 the actor(0x14) object held
 * end to end, r7 a `moved` flag whose zero initialiser gcc also spends on two
 * stores of 0, r5 three different call-crossing values in three disjoint
 * ranges.  That is the whole difficulty.  Plain C has FOUR things wanting a
 * callee-saved register, because gcc also hoists the save-bit id 0x200
 * (`mov r0,#0x80 / lsl r0,#2`, three sites) into one; `moved` then loses and
 * lands in r8, buying a `mov r7, r8 / push {r7}` prologue and 323 of 343
 * encodings differing at 876 bytes.  There is NO high-register traffic in the
 * reference and that absence is what has to be engineered.
 *
 * ONE PIN AT THE FIRST FLAG SITE DEFEATS THE HOIST.  Pinning r0 at the FIRST
 * `__GetFlag(0x200)` is exact; the pins at the other two sites (the short
 * arm's `__GetFlag` and the closing `__SetFlag`) are each individually inert
 * and drop together at zero cost.  Dropping the first instead -- with the
 * other two kept -- is the 323-differing wide-push failure above.  This is
 * docs/elevation.md's "ONE PIN AT THE FIRST USE COVERS THE LATER ONES"
 * confirmed on a THREE-site class, with the recorded precondition satisfied:
 * branches and many clobbering calls separate the sites, so nothing here
 * touches the "ADJACENT SITES" boundary.  It is the ordinary direction of that
 * rule, not the asymmetric case at "a first-use pin CAN be the redundant one".
 *
 * SIX ORDERING PINS, ASCENDING FILL, AND THE FILL IS NOT THE ROM'S ORDER.
 * Every pinned site fills in ARGUMENT order.  Measured against the all-three
 * -register form of the same six pins, which also matches: a uniform
 * DESCENDING fill costs 18 encodings, and transcribing the ROM's own emitted
 * register order per site (r1 r0 r2 at the `__Func_8092adc` site, r1 r2 r0 at
 * the first `__MapActor_SetSpeed`, r2 r0 r1 at the `0x103` emote) costs 5.
 * Ascending is CORRECT here, not merely cheaper; the mid-group `mov r0` the
 * reference shows is sched2 rearranging a correctly allocated group.
 *
 * NEW: A PIN SET IS MINIMAL IN WIDTH AS WELL AS IN COUNT -- MINIMISE WHICH
 * ARGUMENTS ARE PINNED, NOT ONLY WHICH SITES ARE.  Every recorded minimisation
 * in docs/elevation.md strips whole SITES ("six pins are inert one at a time",
 * "greedy stripping to fixpoint from BOTH ENDS", "minimisation from the 53
 * expensive-argument sites"), and every template ships a site as an
 * all-arguments PIN3 block.  Nothing there asks whether a pinned site needs
 * all of its arguments named.  Here NONE of the six does.  All six started as
 * three-register blocks; all six drop q2 at zero cost, individually and
 * together, and three of them (`__Func_8092adc(0x12,0x8000,0x14)` and both
 * `__MapActor_Emote`s) drop q1 as well and need only r0 -- 9 named registers
 * where the template form would ship 18.  Dropping a site's pin ENTIRELY costs
 * 2-3 encodings with SIZE and RELOCATIONS silent, the ordering-pin signature,
 * so the sites are required and only the width was slack.  Mechanism: what
 * these sites need is the SLOT `mov` scheduled ahead of the expensive
 * argument's build, and naming r0 alone fixes that; the expensive argument's
 * own register was never in doubt.  Grepped first as "PIN3", "register int
 * q2", "how many registers", "partial pin", "all three arguments" -- no hits.
 * Practical effect: a width pass after the count pass, ~2 extra compiles per
 * surviving pin, and half the scaffolding ships.
 *
 * THE HOLE IS THE `__MapActor_SetPos` SITE, IT IS READABLE BY CONSTRUCTION,
 * AND PINNING IT IS THE WORST THING YOU CAN DO.  The reference supplies that
 * call's second argument with `mov r1, r5` -- the value is built IN PLACE in
 * the callee-saved register that carried it across the intervening
 * `__MapActor_GetActor`:
 *
 *     rom   ldr r5,[r0,#8] / bl GetActor / ldr r2,[r0,#0x10]
 *           asr r5,#20 / mov r3,#0x80 / lsl r3,#12 / asr r2,#20
 *           lsl r5,#20 / add r5,r3 / lsl r2,#20 / add r2,r3 / mov r1,r5
 *
 * A `mov rD, rS` supplying an argument is a hole BY CONSTRUCTION: the value
 * was commoned, so a pin there can only rematerialise it.  Measured: pinning
 * this one site costs 166 encodings AND COMES OUT FOUR BYTES SHORT -- the
 * "a pin that shortens the output has dropped something that should be live"
 * tell, here the whole in-place chain collapsing.  What the site needs instead
 * is the recorded mutate-in-place spelling (`x >>= 20; x <<= 20; x += 0x80000;`
 * on a named local) rather than one expression; the expression form renames
 * into r1 early (`asr r1, r5, #20`), loses r5's live range, and costs 169
 * encodings at 852 bytes.  THE HOLE TEST NOMINATED THE SITE; A DIFFERENT LEVER
 * DECIDED IT.
 *
 * THE ALL-CHEAP RULE HELD EXACTLY, IN THE NECESSARY DIRECTION.  Of the 74 call
 * sites, 18 carry an expensive argument (a shifted build, a pool load, a
 * memory operand or a stack word) and 56 carry only bare `mov rN, #imm8`.  All
 * seven pins are among the 18; NOT ONE of the 56 is pinned, and pinning one of
 * them (`__Func_809280c(0, 0x12, 0)`) is an exact TIE.  The converse fails as
 * the template says it should: `__Func_8092adc(0, 0xc000, 0xa)` and
 * `__Func_80921c4(0x12, 0x118, 0xe8)` both build a shifted constant and both
 * come out bare, and pinning either is also a TIE.  The set is minimal along
 * this path, not unique.
 *
 * TWO NON-PIN LEVERS AT THE `__Func_8010704` CALL, BOTH ALREADY IN THE DOCS.
 * The ROM stores `[sp, #4]` BEFORE `[sp]`, with the 6th argument's register
 * (r3) reused for the 4th immediately after -- which is exactly the recorded
 * shape "where the shared value goes into `[sp]` last, immediately before the
 * `bl`, and the per-call value into `[sp, #4]` early, no reordering of the C
 * is needed".  It falls out of two source facts: the 5th argument must be
 * shifted IN PLACE (`x >>= 20;` -- as an inline `x >> 20` it is 8 differing),
 * and the 6th needs its OWN local.  Reusing the `y` that the preceding
 * `__MapActor_SetPos` chain used puts the value in r2 and flips the
 * `mov r2,#1 / mov r3,#1` pair, 4 differing; a fresh `z` is exact.  That is
 * "Do NOT reuse an earlier pair's local for a later shared value" arriving
 * from the other side -- the same collision, with the perturbation landing on
 * the LATER call rather than the earlier one.
 *
 * THE FADE LOOP READS BOTH WORDS BEFORE STORING EITHER.  `*(p+0x1c) += k;
 * *(p+0x18) += k;` emits load/add/store twice; the ROM emits both `ldr`s
 * first.  Two fresh locals restore it (7 differing without).  They must be
 * FRESH: recycling `x` and `y` here re-crosses their earlier live ranges and
 * puts `moved` back in r8: 324 of 343
 * differing at 876 bytes, with relocations differing -- a source-level slip
 * inside the loop reading, at a glance, exactly like the whole-function CSE
 * loss it in fact is.
 *
 * THE TWO `orr` SITES NEED THE NARROW-LOCAL LEVER AND THE TWO `and` SITES MUST
 * NOT HAVE IT.  docs/elevation.md's "The constant-as-destination lever: `orr`
 * wants a NARROW local, `and` wants an `int`" is confirmed here on one
 * function carrying both shapes:
 *
 *     rom   ldrb r2,[r1] / mov r3,#2 / orr r3,r2      CONSTANT is rd
 *     plain ldrb r3,[r1] / mov r2,#2 / orr r3,r2      VALUE is rd
 *
 * `unsigned char m = 2; *p = m | *p;` is exact at both `|=` sites (2 encodings
 * each if dropped); `int m` at both is 4 differing -- byte-identical to the
 * plain literal, i.e. NO LEVER AT ALL, exactly as the note predicts.  The two
 * `*(p + 0x5a) &= 0xfe` sites are already exact written plain and were left
 * alone.  The 0x5a `|= 1` site additionally needs its ADDRESS named
 * (`bp = __MapActor_GetActor(0) + 0x5a`); the narrow local alone leaves the
 * single call and the byte load in the wrong roles.
 *
 * TEARDOWN.  Every lever was removed from the finished file and re-measured:
 *
 *     the first `__GetFlag` pin           323 of 343 differing, 876 bytes,
 *                                         relocations differ, wide push
 *     the in-place x/y SetPos chain       169 differing, 852 bytes, 4 SHORT
 *     `x >>= 20;` before __Func_8010704     8 differing
 *     the fade loop's two fresh locals      7 differing
 *     a separate local for the 6th arg      4 differing
 *     each of the six ordering pins       2-3 differing, relocations silent
 *     each of the two `unsigned char m`     2 differing
 *     pinning the SetPos hole             166 differing, 852 bytes, 4 SHORT
 *     descending fill at the six sites     18 differing
 *     ROM-order fill at the six sites       5 differing
 *     `int m` at both orr sites             4 differing
 *
 * And three additions are exact TIES, so the set is minimal but not unique:
 * pins on `__Func_8092adc(0, 0xc000, 0xa)`, on
 * `__Func_80921c4(0x12, 0x118, 0xe8)`, and on the all-cheap
 * `__Func_809280c(0, 0x12, 0)`.
 *
 * MINIMISED TO A FIXPOINT.  Thirteen levers survive; each removal fails, and a
 * second full pass over the survivors finds every one still required.  Two
 * pins were dropped on the way, individually and together at zero cost, and
 * every surviving pin was then narrowed to its minimum width.
 *
 * -- worked in scratch_elev/b244/f20085f4; gen.py there rebuilds any single
 *    lever-drop from cand.c and objsweep.sh runs a whole sweep in one
 *    container invocation.
 */
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __MessageID(int id);
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_SetBehavior(int slot, int b);
extern void __MapActor_Emote(int slot, int id, int n);
extern void __MapActor_WaitMovement(int slot);
extern void __ActorMessage(int slot, int a);
extern void __PlaySound(int id);
extern void __WaitFrames(int n);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __Func_809202c(void);
extern void __Func_809218c(int a, int b, int c);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_809228c(int a, int b, int c);
extern void __Func_809259c(int a, int b);
extern void __Func_80925cc(int a, int b);
extern void __Func_809280c(int a, int b, int c);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8092b08(int a, int b);
extern void __Func_8093040(int a, int b, int c);
extern unsigned char *iwram_3001ebc;
extern void OvlFunc_928_2008500(void);

#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")

void OvlFunc_928_20085f4(void)
{
    unsigned char *obj;
    unsigned char *p;
    int moved;
    int face;
    int x;
    int y;
    int z;
    int lo;
    int hi;
    int t;

    obj = __MapActor_GetActor(0x14);
    __CutsceneStart();
    moved = 0;
    *(int *)(__MapActor_GetActor(0x12) + 0x6c) = 0;
    { PIN1; q0 = 0x200; t = __GetFlag(q0); }
    if (t != 0
        || (*(int *)(__MapActor_GetActor(0x12) + 8) >> 20) <= 0x13) {
        face = *(unsigned short *)(__MapActor_GetActor(0x12) + 6);
        __Func_809280c(0x12, 0, 0);
        __CutsceneWait(0xa);
        __MessageID(0x17fb);
        if (__GetFlag(0x200) == 0) {
            *(unsigned short *)(iwram_3001ebc + (0xec << 1)) += 1;
            __ActorMessage(0x12, 0);
            *(unsigned short *)(__MapActor_GetActor(0x12) + 0x64) = 0;
            *(unsigned short *)(__MapActor_GetActor(0x12) + 6) = face;
        } else {
            __ActorMessage(0x12, 0);
            { PIN1; q0 = 0x12;
              __Func_8092adc(q0, 0x8000, 0x14); }
        }
        *(int *)(__MapActor_GetActor(0x12) + 0x6c) = (int)OvlFunc_928_2008500;
        __CutsceneEnd();
        return;
    }
    if ((*(int *)(__MapActor_GetActor(0) + 0x10) >> 19) > 0x1b
        && (*(int *)(__MapActor_GetActor(0) + 0x10) >> 19) <= 0x1d
        && (*(int *)(__MapActor_GetActor(0) + 8) >> 20) != 0x1a) {
        { PIN2; q0 = 0; q1 = 0x8000;
          __MapActor_SetSpeed(q0, q1, 0x4000); }
        __Func_809280c(0, 0x12, 0);
        __CutsceneWait(5);
        p = __MapActor_GetActor(0);
        if (*(int *)(p + 8) < *(int *)(__MapActor_GetActor(0x12) + 8)) {
            *(__MapActor_GetActor(0) + 0x5a) &= 0xfe;
            __Func_809218c(0,
                ((*(int *)(__MapActor_GetActor(0x12) + 8) >> 20) << 4) - 8,
                0xe8);
            moved = 1;
        } else {
            *(__MapActor_GetActor(0) + 0x5a) &= 0xfe;
            __Func_809218c(0,
                ((*(int *)(__MapActor_GetActor(0x12) + 8) >> 20) << 4) + 0x18,
                0xe8);
        }
        __MapActor_WaitMovement(0);
    }
    *(int *)(__MapActor_GetActor(0x12) + 0x38) = 0x80000000;
    *(int *)(__MapActor_GetActor(0x12) + 0x3c) = 0x80000000;
    *(int *)(__MapActor_GetActor(0x12) + 0x40) = 0x80000000;
    __MapActor_SetBehavior(0x12, 1);
    __MapActor_SetAnim(0x12, 1);
    __Func_80925cc(0x12, 2);
    __CutsceneWait(0xa);
    __PlaySound(0xe4);
    *(int *)(obj + 0x18) = 0x4ccc;
    *(int *)(obj + 0x1c) = 0x4ccc;
    x = *(int *)(__MapActor_GetActor(0x12) + 8);
    y = *(int *)(__MapActor_GetActor(0x12) + 0x10);
    x >>= 20;
    y >>= 20;
    x <<= 20;
    x += 0x80000;
    y <<= 20;
    y += 0x80000;
    __MapActor_SetPos(0x14, x, y);
    x = *(int *)(__MapActor_GetActor(0x12) + 8);
    z = *(int *)(__MapActor_GetActor(0x12) + 0x10) >> 20;
    x >>= 20;
    __Func_8010704(0x10, 0x10, 1, 1, x, z);
    __Func_8092b08(0x14, 2);
    { unsigned char m = 2;
      *(obj + 0x23) = m | *(obj + 0x23); }
    do {
        __WaitFrames(3);
        hi = *(int *)(obj + 0x1c);
        lo = *(int *)(obj + 0x18);
        *(int *)(obj + 0x1c) = hi + 0x1999;
        lo += 0x1999;
        *(int *)(obj + 0x18) = lo;
    } while (lo <= 0xffff);
    { PIN1; q0 = 0x12;
      __MapActor_Emote(q0, 0x105, 0x46); }
    __Func_809280c(0x12, 0, 0);
    __CutsceneWait(0x14);
    { PIN1; q0 = 0x12;
      __MapActor_Emote(q0, 0x103, 0); }
    __Func_809259c(0x12, 2);
    __CutsceneWait(0x46);
    __MessageID(0x17fa);
    __Func_8093040(0x12, 0, 0x14);
    __Func_809202c();
    if ((*(int *)(__MapActor_GetActor(0) + 8) >> 20) == 0x1a
        && (*(int *)(__MapActor_GetActor(0) + 0x10) >> 20) > 0xd)
        moved = 1;
    if (moved != 0) {
        { PIN2; q0 = 0; q1 = 0xcccc;
          __MapActor_SetSpeed(q0, q1, 0x6666); }
        __Func_8092adc(0, 0xc000, 0xa);
        *(__MapActor_GetActor(0) + 0x5a) &= 0xfe;
        __MapActor_SetAnim(0, 2);
        __Func_809228c(0, 0, 0x10);
        __MapActor_WaitMovement(0);
        __MapActor_SetAnim(0, 1);
    }
    { PIN2; q0 = 0x12; q1 = 0xcccc;
      __MapActor_SetSpeed(q0, q1, 0x6666); }
    if ((*(int *)(__MapActor_GetActor(0x12) + 0x10) >> 20) != 0xe)
        __Func_80921c4(0x12, *(short *)(__MapActor_GetActor(0x12) + 0xa), 0xe8);
    __Func_80921c4(0x12, 0x118, 0xe8);
    __SetFlag(0x200);
    { unsigned char *bp = __MapActor_GetActor(0) + 0x5a;
      unsigned char m = 1;
      *bp = m | *bp; }
    __CutsceneEnd();
}
