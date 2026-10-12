/* OvlFunc_964_20090c4  --  0x020090c4
 *
 * NON-MATCHING, 12 differing STREAM encodings of 119.  96/96 16-bit
 * instructions, IN PHASE, so a true DISTANCE.  Was 16 of 119.
 *
 *   *** THIS FIGURE IS MEASURED UNDER -fno-strict-aliasing (ALIAS_CFLAGS,
 *   Makefile:235).  At the production -O2 this same body reads 14 of 119.  The
 *   flag is not a device -- it is a real per-file group applied to ~20 TUs --
 *   but no Makefile rule is needed while this stays parked, since
 *   src/non_matching is never a build input.  Any verify recipe MUST pass it. ***
 *
 *   The relocation set differs ONLY in two `bl` symbol names, `__umodsi3`
 *   against the reference's `_umodsi3_RAM`, and overlays/rom_7ed0a0/overlay.ld:111
 *   already carries `__umodsi3 = _umodsi3_RAM;`.  THOSE TWO COST ZERO ENCODINGS
 *   -- a `bl` with a relocation assembles to the same placeholder bytes either
 *   way -- so the park's old "FOUR OF THE 17 ARE PHANTOM" over-counted by two:
 *   the phantoms are two RELOCATIONS, not two instructions, and the 12 above is
 *   already the real instruction distance.  The park also carried the
 *   "RELOCATIONS DIFFER ... NOT A DISTANCE" banner on top of its own correct
 *   note that they are phantom; both are now stated once, consistently.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     -e OBJCMP_EXTRA=-fno-strict-aliasing goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/ovl_7ed0a0/20090c4.c \
 *     asm/overlays/rom_7ed0a0/ovl_30_a_a_c_c_a_c_a.s --func OvlFunc_964_20090c4
 *
 * PIN-FREE, device-free.  Singleton piece (tools/piece.py), no split needed.
 *
 * WHAT MOVED IT, 16 -> 12, AND THE TWO HALVES COMPOSE:
 *   -fno-strict-aliasing alone                 16 -> 14   (autosweep, flag axis)
 *   the anchor below alone                     16 -> 14   (autosweep, boundary 26)
 *   both together                              16 -> 12
 * The anchor sits after `vz = -32765 + vz;`, i.e. INSIDE the `if (i & 1)` body
 * inside the loop -- a placement the park's own ~100-spelling sweep never
 * generated, and one autosweep could only generate after its depth>=1 fix.
 * It removes two of the six encodings in the tail group below.
 *
 * THE RESIDUE, four groups, re-read this round:
 *   G1 2  the two coordinate loads of __Actor_TravelTo come out in the other
 *         order -- SAME REGISTERS: rom `ldr r1,[r6,#8] / ldr r2,[r6,#0xc]`,
 *         ours the two swapped.
 *   G2 4  `add r3, sp, #0x10 / mov r8, r3` is emitted BEFORE `bl __WaitFrames`;
 *         the ROM emits it after, and in r3 where we use r1.  THIS IS THE PARK'S
 *         "WALL" AND ITS ANALYSIS OF IT STILL HOLDS (see below).
 *   G3 2  `ldr r2, =0xfffe0000` inside the loop, where the ROM uses r3.
 *   G4 4  the tail of the argument build: `ldr r3, =0xffff8003` against our r2,
 *         `add r4, r3` against `add r4, r2`, and `mov r3, #0` swapped with
 *         `ldr r0, [r6, #0x8]`.
 *
 * G3 and G4 are the same single-register shift seen in two places: ours puts
 * each pooled constant one step EARLIER in REG_ALLOC_ORDER ({3,2,1,0,...},
 * arm.h:988-994) than the ROM.  x.c.18.greg says `;; 4 regs to allocate:
 * 33 34 32 40`, so both constants are LOCAL-allocated quantities, which puts
 * them in local-alloc's find_free_reg and specifically under its
 * INSN_SCHEDULING lifetime extension (local-alloc.c:1405-1425: birth-2,
 * death+2, "to avoid using hard registers allocated to qtys which are born
 * immediately after this qty or die immediately before this qty", with a
 * documented fallback to the unadjusted lifetime).  That extension is exactly
 * what pushes a constant off r3 onto r2 when a neighbour is born next to it,
 * and it is schedule-sensitive, which is why it moves with the anchor and the
 * aliasing flag and not with any statement order.  NEXT AGENT: this is the
 * claim to test, and local-alloc.c:1405-1425 is where to test it.
 *
 * G2, CARRIED FORWARD FROM THE PARK BECAUSE IT WAS RIGHT.  The ROM has
 *
 *      add r3, sp, #0x10 / mov r8, r3 / ... / mov r0, r8 / str r3, [r0, #0x24]
 *
 * and gcc emits that second reload of the pointer only when its live range
 * crosses a CALL between definition and store; with the definition adjacent to
 * the store, reload INHERITS the scratch and stores through it directly, one
 * instruction shorter.  Both placements are reachable and neither is the ROM's:
 * `c = &s;` after the WaitFrames rematerialises the address and is 115 lines
 * against 116; before it, the `mov r0, r8` appears but the `add / mov r8` pair
 * lands ahead of the `bl`.  sched2 will not sink the pair past the call because
 * the `add` writes a call-clobbered register.  GENERALISATION WORTH KEEPING:
 * `mov rLow, rHigh` immediately before a store whose base is that high register
 * is a RE-READ of a pointer already committed to a callee-saved register -- it
 * is evidence that a CALL sits between the pointer's definition and its use, not
 * evidence about statement order.
 *
 * THREE LEVERS FROM THE PARK, ALL STILL IN THE BODY AND ALL STILL EARNING:
 *   - A NEGATIVE CONSTANT MULTIPLIER IS WHAT SELECTS THE SHIFT CHAIN.
 *     expand_mult calls synth_mult on the ABSOLUTE value and negates after, but
 *     the cost budget comes from the negative MULT -- so `x * 6553` gives
 *     `ldr =0x1999 / mul` and only `x * -6553` gives the seven-instruction
 *     chain.  Read the sign off the ROM: a `neg` after a shift chain means the
 *     source multiplier was negative.  This function multiplies twice and the
 *     ROM spells the two differently, which is a clean internal control.
 *   - A STATEMENT BREAK STOPS THE DISTRIBUTION.  `(r - 5) * 0x3332` folds to
 *     `r * 0x3332 - 65530`; splitting it into `t = r; t -= 5; vx = t * 0x3332;`
 *     gives the ROM's `sub #5 / mov / mul`.  Worth 4.
 *   - DELETING THE BYTE-POINTER LOCAL WAS WORTH 3.  The ROM holds `&actor->f55`
 *     in r10 across the function and writes through it three times, which reads
 *     like the case for a named `unsigned char *p`; it is not -- gcc CSEs the
 *     field reference into that same pseudo on its own.
 *   Also confirmed by the park: `C - x*k` and `x*-k + C` are indistinguishable
 *   in an isolated probe and only differ under this function's real register
 *   pressure.  Isolated probes are not safe for sign questions.
 *
 * MEASURED INERT THIS ROUND -- 115 variants on top of the flag, all 12:
 *   - `c = &s;` at all 14 boundaries from the top of the function to the store,
 *     under the flag AND without it.  (The park's 34-way sweep was at default
 *     flags only; crossing it with the flag changes nothing.)
 *   - all 29 compiling anchor placements as a SECOND anchor on top of this one.
 *   - naming the TravelTo coordinates (both x/y orders, and the z sum) -- G1
 *     does not move.
 *   - naming the loop's -0x20000 step as a local; spelling it `a->y = a->y - k`.
 *   - four spellings of the `-32765 + vz` fold (`vz -= 32765`, `vz = vz - 32765`,
 *     one statement, and the random value through its own local).
 *   - splitting the reused `vz` and the reused `t` into two locals each.
 *   - naming the loop's `0` argument as a local.
 *   - the other seven flag groups: -O1 112, -fno-gcse 114, --no-sched2 51,
 *     -ffixed-r7 121, and three inert at 16 on the unchanged body.
 *   - TWO of autosweep's 31 anchor variants DO NOT COMPILE (boundaries 17 and
 *     18, which land among the loop's declarations); that part of the axis is
 *     untested, not inert.
 */
struct Actor {
    unsigned char pad00[8];
    int x;
    int y;
    int z;
    unsigned char pad14[0x14];
    int f28;
    unsigned char pad2c[0x10];
    int f3c;
    unsigned char pad40[0x15];
    unsigned char f55;
};

struct Cfg {
    unsigned char pad00[0x24];
    void (*f24)(void);
};

extern struct Actor *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __Actor_SetAnim(struct Actor *a, int n);
extern void __Func_8092504(int a);
extern void __Actor_SetSpriteFlags(struct Actor *a, int f);
extern void __PlaySound(int id);
extern void __Actor_TravelTo(struct Actor *a, int x, int y, int z);
extern void __WaitFrames(int n);
extern unsigned int __Random(void);
extern void OvlFunc_964_2009068(void);
extern void OvlFunc_964_2008ae8(int x, int y, int z, int a, int b, int c, int d, struct Cfg *s);

void OvlFunc_964_20090c4(void)
{
    struct Cfg s;
    struct Cfg *c;
    struct Actor *a;
    unsigned int i;
    int vx;
    int vz;
    int t;
    unsigned char two;

    a = __MapActor_GetActor(0);
    __CutsceneStart();
    __Actor_SetAnim(a, 6);
    __Func_8092504(0);
    __Actor_SetAnim(a, 1);
    __Actor_SetSpriteFlags(a, 0);
    two = 2;
    a->f55 = two | a->f55;
    __PlaySound(0x98);
    a->f28 = 0x80 << 11;
    __Actor_TravelTo(a, a->x, a->y, a->z + (0xc0 << 12));
    c = &s;
    __WaitFrames(6);
    a->f55 = 0;
    c->f24 = OvlFunc_964_2009068;
    __PlaySound(0x7f);
    for (i = 0; i < 8; i++) {
        a->y -= 0x20000;
        a->f3c = a->y;
        __WaitFrames(1);
        if (i & 1) {
            t = (int)(__Random() % 10);
            t -= 5;
            vx = t * 0x3332;
            vz = (int)(__Random() % 10) * -6553;
            vz = -32765 + vz; do { } while (0);
            OvlFunc_964_2008ae8(a->x, a->y, a->z, vx, 0, vz, 0x1000001, c);
        }
    }
    __Actor_SetSpriteFlags(a, 1);
    a->f55 = 3;
    __CutsceneEnd();
}
