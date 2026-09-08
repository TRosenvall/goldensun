/* OvlFunc_907_2008ae0 -- 0x02008ae0, the WHOLE of
 * asm/overlays/rom_79b154/ovl_30_c_a_c_c_a_a_c_c.s (one .thumb_func_start, one
 * .func_end, 186 instructions; no .section/.data/.bss/.lcomm/.word/.byte, and
 * all six `.L` symbols -- .Lb68 .Lb72 .Lb8c .Lc5c .Lc80 .Lc92 -- are branch
 * targets DEFINED IN THIS FILE).  tools/asmfacts.py says WHOLE / convert
 * directly: one .c, no split, no linker edit.
 *
 * Total .text for this TU = 468 bytes.  The one line naming the object, in
 * overlays/rom_79b154/overlay.ld:41, is
 *
 *     asm/overlays/rom_79b154/ovl_30_c_a_c_c_a_a_c_c.o(.text)
 *
 * and it stays VERBATIM.  NO FLAG GROUP: tryc.makefile_flags() is the empty
 * set and every rom_79b154 rule in the Makefile names
 * ovl_30_c_a_a_c_a_c_c_c.o literally, so there is no wildcard hazard and the
 * generic `asm/%.o: src/%.c` rule fires at the tree default -O2 -mthumb
 * -mthumb-interwork -fcall-used-r4.
 *
 * FAKEMATCH: register-pin idiom, so OvlFunc_907_2008ae0 goes in fakematch.txt.
 *
 * OK OvlFunc_907_2008ae0 -- 468 bytes, 192 encodings and 37 relocations
 * identical.  Re-measured four times.
 *
 * The only external symbol it defines nothing for is OvlFunc_907_2008cb4,
 * already reachable from asm/overlays/rom_79b154/ovl_30_c_a_c_c_a_b.s.
 *
 * ------------------------------------------------------------- THE PATH -----
 * The neighbour src/overlays/rom_79b154/ovl_30_c_a_c_c_a_a_c_b.c
 * (OvlFunc_907_20089cc) is the previous function out of the same original .s
 * and it named the blocker in advance: "the ROM puts `mov r0, #slot` BEFORE the
 * shift that builds the second argument, and gcc emits it after".  Six sites
 * here do exactly that.  What that file could NOT transfer is its fix -- it
 * names the shifted arguments in the DOMINATING BLOCK, and this function's ROM
 * REBUILDS every one of them at its own call site, so a dominating-block name
 * is precisely wrong here.  Same family, opposite lever.
 *
 * Ordered by mechanism size, as recorded:
 *
 *   plain C, struct tags, one `p`, goto for the .Lc92 skip   198 of 200  (+8)
 *   + `x = 0xd0; x <<= 8;` two-statement builds at all six    91 of 195  (+3)
 *   + the three `|= 0x10` sites written inline                17 of 192  (exact size)
 *   + the 0x8e/0x9c fill written x<<=,z<<= not z<<=,x<<=      13 of 192
 *   + PIN at all six call sites                                6 of 192
 *   + 0x8e<<16 / 0x9c<<16 / 0xb0<<15 / 0xc4<<16 as LITERALS    4 of 192
 *   + q0 assigned FIRST at the two remaining sites             0
 *
 * 1. THE PUSH MASK WAS THE FIRST DIAGNOSTIC AND IT PAID IMMEDIATELY.  The ROM
 *    pushes {r5, r6, r7, lr}; the first draft pushed r8 as well and ran 8
 *    instructions long.  That is the recorded "a push mask wider than the ROM's
 *    is a COMMONING tell" -- gcc had commoned 0x8e0000 and 0x9c0000 across the
 *    two __MapActor_SetPos sites that share them and needed two more
 *    callee-saved registers to carry them over the intervening
 *    __MapActor_GetActor / __Actor_SetSpriteFlags pair.  The ROM rebuilds both.
 *    Writing every shifted argument as `x = base; x <<= n;` -- the
 *    rom_7e0928 sibling's `q2` lever -- killed the commoning outright: 198 -> 91
 *    and r8 gone.  ONE LEVER, and it was found by counting registers in the
 *    prologue rather than by reading the body.
 *
 * 2. THE `|= 0x10` SITES WERE THE WHOLE OF THE LENGTH.  The ROM does
 *    `bl __MapActor_GetActor / add r0,#0x59 / ldrb / orr / strb` -- the address
 *    build DESTROYS the return register.  A named `struct Actor *p` reused
 *    across the three sites gives `mov r2,r0 / add r2,#0x59` instead, one extra
 *    instruction three times over, which is exactly the three the function was
 *    long.  Writing the fetch INLINE (`__MapActor_GetActor(9)->f59 |= 0x10;`)
 *    is exact.  So are `unsigned char *q = (unsigned char *)... + 0x59; *q |=`
 *    and three separate pointer variables p1/p2/p3 -- all three spellings tie
 *    at 0, so the inline one ships.  NEW, and cheap to recognise: A CANDIDATE
 *    EXACTLY N INSTRUCTIONS LONG WITH N `mov rX, r0` BEFORE N ADDRESS BUILDS IS
 *    A NAMED POINTER THAT SHOULD NOT BE NAMED -- the ROM let the call's return
 *    register die into the address.
 *
 * 3. sched2 OWNS THE FILL ORDER HERE AND THE SIGN SAID SO.  At 17 differing,
 *    `-fno-schedule-insns2` is 30 -- a REGRESSION, which per docs/elevation.md
 *    means sched2 is already producing the ROM's order and alias is the wrong
 *    axis.  That verdict was worth its one compile twice over: it ruled out the
 *    aliasing levers and it predicted, correctly, that permuting source
 *    statement order would be INERT.  It was: all twelve permutations of the
 *    two constant assignments and the two shifts at sites 4/5/6 report 8
 *    differing, byte-for-byte the same 8.  sched2 normalises source order at a
 *    site completely; the only thing that reaches it is WHICH INSNS EXIST.
 *
 * 4. WHAT REACHED IT WAS THE PIN, AND THEN THE PIN'S SHAPE.  The ROM shifts the
 *    arguments IN PLACE IN r1/r2 (`mov r1,#0x84 / mov r2,#0xba / mov r0,#0xb /
 *    lsl r1,#16 / lsl r2,#16`), which is what a PIN2/PIN3 block with the shift
 *    written on the pinned variable produces.  All six pin blocks are
 *    load-bearing: dropping them one at a time from the minimised file costs
 *    2, 2, 195, 2, 3 and 8 differing.  The 195 is site 3 -- its pin is the
 *    EVICTION device that keeps the 0x8e/0x9c pair out of the commoning pool,
 *    and without it the r8 push comes straight back.  The other five are pure
 *    ORDERING.  Widths were narrowed to a fixpoint and re-confirmed by a strict
 *    re-drop: PIN1, PIN1, PIN3, PIN2, PIN2, PIN2.
 *
 * 5. A LITERAL BEAT THE TWO-STATEMENT BUILD AT THREE OF THE SIX SITES, WHICH IS
 *    THE OPPOSITE OF LEVER 1.  Once the pins are in, r1 and r2 are
 *    call-clobbered, so the commoning that lever 1 was fighting cannot happen
 *    and the two-statement build is no longer buying anything -- it is only
 *    constraining the schedule.  `q1 = 0x8e << 16;` lets gcc's own constant
 *    synthesis emit the mov/lsl pair and sched2 then lands the ROM's CROSSED
 *    fill (movs r1,r2 -- shifts r2,r1), which no hand-written statement order
 *    could reach: 6 -> 4.  Applying literals at the other three sites is 11, so
 *    this is per-site, not a rule.  NEW: THE REBUILD LEVER AND THE PIN ARE
 *    ALTERNATIVES, NOT A LADDER.  Both defeat commoning; where the pin already
 *    does it, the two-statement build is dead weight that costs schedule
 *    freedom.  The discriminator is whether the value's register is
 *    call-clobbered.
 *
 * 6. THE LAST FOUR WERE PIN ORDER, AND THE POSITION IS PER SITE.  `q0` assigned
 *    LAST at site 3 (`q1, q2, q0`) and FIRST at sites 4 and 6 (`q0, q1, q2`) is
 *    exact; each is 2 differing with the other spelling, and the two sites were
 *    diagnosed independently and combined.  Site 5 wants `q1, q2, q0`.  There
 *    is no single fill order for this callee -- __MapActor_SetPos takes all
 *    three spellings in one function.
 *
 * MEASURED WORSE / INERT (against 192 lines / 192 encodings / 468 bytes):
 *
 *   spelling                                                    differing
 *   ----------------------------------------------------------  ---------
 *   plain C, no pins, no two-statement builds                    198 (+8, pushes r8)
 *   two-statement builds, named `p` at the three |= sites         91 (+3)
 *   `void *sprite` + a cast for the 0x1e store                    50 (1 SHORT)
 *   no pins, best non-pin fill order                              13
 *   drop the site-3 pin                                          195 (+5, pushes r8)
 *   drop the site-6 pin                                            8
 *   drop the site-5 pin                                            3
 *   drop the site-1 / site-2 / site-4 pin                          2 each
 *   PIN1 at site 3 / site 4 / site 5 / site 6                    195 / 2 / 4 / 8
 *   PIN2 at site 3                                                15
 *   literals at all six sites instead of three                    11
 *   __CopyMapTiles/__Func_8010704 stack args as bare literals      6
 *   the 0x8e/0x9c fill spelled z<<=,x<<= instead of x<<=,z<<=      17
 *   INERT (tie at 0, so the simpler form ships):
 *     `a->y -= 0x80000` for `a->y += 0xfff80000`
 *     the .Lc92 skip written as a DUPLICATED tail in an if/else instead of
 *       `goto done` -- cross-jumping merges it; the goto is shorter and ships
 *     all twelve source-order permutations at sites 4/5/6 (see lever 3)
 *     `unsigned char *q` or three pointer variables at the |= sites
 *
 * THE UNIFIED STRUCT TAG IS THE ALIASING LEVER, third sighting.  `sprite` is
 * declared `struct Actor *` -- the SAME tag -- and `f1e` lives in that tag, so
 * `a->sprite->f1e = ...` and the `a->y` read-modify-write that follows sit in
 * one alias set.  Split into `void *` plus a cast, the store and the load
 * reorder and the function comes out ONE INSTRUCTION SHORT at 50 differing.
 * src/overlays/rom_798dc4/ovl_314_c_c_c_a.c records the same thing at the same
 * two offsets (0x50 and 0x1e); this is the second file to need it and the
 * first where the neighbour of the store is an `+=` rather than a reload.
 */
struct Actor {
    unsigned char pad00[0xc];
    int y;
    unsigned char pad10[0xe];
    unsigned short f1e;
    unsigned char pad20[0x30];
    struct Actor *sprite;
    unsigned char pad54;
    unsigned char f55;
    unsigned char pad56[3];
    unsigned char f59;
};

#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")

extern int __GetFlag(int id);
extern struct Actor *__MapActor_GetActor(int slot);
extern void __Actor_SetSpriteFlags(struct Actor *a, int n);
extern void __MapActor_SetPos(int slot, int x, int z);
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8092b08(int a, int b);
extern void OvlFunc_907_2008cb4(void);

void OvlFunc_907_2008ae0(void)
{
    struct Actor *a;
    struct Actor *b;
    int x, z;
    int e, f;

    a = __MapActor_GetActor(0xa);
    b = __MapActor_GetActor(0xb);
    __Actor_SetSpriteFlags(__MapActor_GetActor(8), 0);
    if (__GetFlag(0x845) != 0) {
        __MapActor_SetPos(9, 0, 0);
        __MapActor_SetPos(0xa, 0, 0);
        __MapActor_SetPos(0xb, 0, 0);
        e = 1;
        f = 2;
        __CopyMapTiles(0x38, 0xf, 0x28, 0xf, e, f);
        e = 0xa;
        f = 0xf;
        __Func_8010704(0x1a, 0xf, 1, 3, e, f);
        if (__GetFlag(0x849) == 0) {
            if (__GetFlag(0x848) != 0)
                goto done;
            __MapActor_SetPos(0xe, 0, 0);
        }
        { PIN1; x = 0xd0; q0 = 0xc; x <<= 8;
          __Func_8092adc(q0, x, 0); }
        { PIN1; x = 0xb0; q0 = 0xd; x <<= 8;
          __Func_8092adc(q0, x, 0); }
    } else {
        __MapActor_SetPos(0xc, 0, 0);
        __MapActor_SetPos(0xd, 0, 0);
        __MapActor_SetPos(0xe, 0, 0);
        __Actor_SetSpriteFlags(__MapActor_GetActor(9), 0);
        __Actor_SetSpriteFlags(__MapActor_GetActor(0xa), 0);
        __Actor_SetSpriteFlags(__MapActor_GetActor(0xb), 0);
        a->f55 = 0;
        if (__GetFlag(0x881) != 0) {
            __MapActor_GetActor(9)->f59 |= 0x10;
            __MapActor_GetActor(0x10)->f59 |= 0x10;
            __MapActor_GetActor(0xb)->f59 |= 0x10;
            { PIN3; q1 = 0x8e << 16; q2 = 0x9c << 16; q0 = 0x10;
              __MapActor_SetPos(q0, q1, q2); }
            __Actor_SetSpriteFlags(__MapActor_GetActor(0x10), 0);
            { PIN2; q0 = 0xa; q1 = 0x8e << 16; z = 0x9c << 16;
              __MapActor_SetPos(q0, q1, z); }
            a->sprite->f1e = 0x80 << 7;
            a->y += 0xfff80000;
            if (__GetFlag(0x848) != 0) {
                { PIN2; q1 = 0x84; z = 0xba; q0 = 0xb; q1 <<= 16; z <<= 16;
                  __MapActor_SetPos(q0, q1, z); }
            } else {
                { PIN2; q0 = 0xb; q1 = 0xb0 << 15; z = 0xc4 << 16;
                  __MapActor_SetPos(q0, q1, z); }
                __Func_8092b08(0xb, 3);
                b->f59 |= 4;
            }
        } else {
            a->y = 0x80 << 14;
            b->f55 = 0;
            b->y = 0xc0 << 14;
        }
    }
done:
    OvlFunc_907_2008cb4();
}
