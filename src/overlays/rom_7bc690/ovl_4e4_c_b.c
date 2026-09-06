// fakematch
/* OvlFunc_933_2009180  --  0x02009180
 *   [asm/overlays/rom_7bc690/ovl_4e4_c.s, 3rd of 7]
 *   lands as src/overlays/rom_7bc690/ovl_4e4_c_b.c
 *
 * The Tolbi-spring "nearest NPC reacts" beat.  Slots 9..12 are scanned for the
 * one closest to the party leader in Manhattan distance, measured in whole
 * tiles (a 16.16 fixed-point coordinate divided by 0x10000), the leader turns
 * and is handed a callback, and the winner is walked over and emoted at.
 *
 * VERDICT
 *   OK OvlFunc_933_2009180 -- 380 bytes, 157 encodings and 28 relocations identical
 * against BOTH scratch_elev/b244/f2009180/ref.s and the original
 * asm/overlays/rom_7bc690/ovl_4e4_c.s.
 *
 * NO FLAG GROUP.  `tryc.makefile_flags()` returns the EMPTY set for
 * src/overlays/rom_7bc690/ovl_4e4_c_b.c (and for _c.c and _a.c), no Makefile
 * line mentions this stem, and objcmp against the ORIGINAL asm path prints no
 * `(built with: ...)` line.  The TU falls to the tree default `asm/%.o: src/%.c`
 * at -O2.  The match does not depend on any flag.
 *
 * LANDING NEEDS AN EXPORT, THEN A SPLIT, THEN THE .c -- THREE GATED STEPS.
 * The .s holds SEVEN functions (2008e2c, 2009054, 2009180, 20092fc, 20094b0,
 * 2009638, 2009874) plus a trailing `.section .data` of `.incbin` blobs, so a
 * whole-file .c is impossible: C cannot carry the blobs.  Every linker line
 * naming the .o, matched on FULL PATH across the tree and quoted by content
 * (both in overlays/rom_7bc690/overlay.ld; the only other hits are in the
 * generated overlay.map, which is output, not input):
 *
 *     		asm/overlays/rom_7bc690/ovl_4e4_c.o(.text)     <- in `.text : {`
 *     		asm/overlays/rom_7bc690/ovl_4e4_c.o(.data)     <- in `.data : {`
 *
 * `split_s.py` REFUSES this cut as it stands, and the refusal is real: running
 * its own parse + cross_references over the file reports exactly two labels,
 *
 *     ovl_4e4_c_a.s references .L1f48, defined in ovl_4e4_c_c.s
 *     ovl_4e4_c_a.s references .L1f70, defined in ovl_4e4_c_c.s
 *
 * -- the two data tables OvlFunc_933_2008e2c selects between, which live in the
 * trailing `.data` and would land in _c while their only user lands in _a.  So:
 *
 *   1. add `.global .L1f48` and `.global .L1f70` immediately above their
 *      definitions in asm/overlays/rom_7bc690/ovl_4e4_c.s.  A `.global` emits
 *      no bytes; gate `make compare` on this ALONE so an export mistake and a
 *      layout mistake stay separable.
 *   2. python3 tools/split_s.py asm/overlays/rom_7bc690/ovl_4e4_c.s OvlFunc_933_2009180
 *      giving _a = {2008e2c, 2009054}, _b = {2009180} (nothing trails it -- the
 *      cut after `.func_end` carries zero lines), _c = {20092fc, 20094b0,
 *      2009638, 2009874} + the `.data`.  rewrite_ld repoints BOTH sections, one
 *      line per piece; _a and _b get `.data` lines for a section they do not
 *      have, which is harmless and is the documented behaviour.  Gate
 *      `make compare` again with NO .c in the tree.
 *   3. add this file as src/overlays/rom_7bc690/ovl_4e4_c_b.c and delete
 *      asm/overlays/rom_7bc690/ovl_4e4_c_b.s.
 *
 * The suffixes are FREE: `ovl_4e4_c_a`, `ovl_4e4_c_b` and `ovl_4e4_c_c` appear
 * nowhere in the tree, in asm/, src/ or any .ld.
 *
 * ----------------------------------------------------------------- LEVERS --
 *
 * THE PROLOGUE IS A PIN PROLOGUE READ BY CONTENT.  `push {r5,r6,r7,lr}` +
 * `mov r7,r8 / push {r7}` keeps four registers, and the epilogue's
 * `pop {r0} / bx r0` is the positive evidence of `void`.  What the wide push
 * KEEPS is the whole story: r6 = the leader pointer, r7 = the running minimum,
 * r5 = the loop counter, r8 = the winning slot.  r8 is not a wall -- it is one
 * value (`nearest`) that has to survive `__SetFlag` and thirteen later calls,
 * and plain C put it there unasked.  Seven r8 references across ONE high
 * register is exactly a deliberately-placed value, not pressure.
 *
 * FIVE LEVERS, EACH MEASURED ALONE FROM THE FINISHED FILE (objcmp encodings):
 *
 *   1. `p += 0x5a; *p &= 0xfe;`  NOT  `p[0x5a] &= 0xfe;`     both sites: 102
 *      differing and +8 bytes; one site alone: 100 and +4.  0x5a is outside
 *      thumb's `ldrb` imm5 range so the address must be computed either way;
 *      the ROM computes it INTO the returned pointer (`add r0, #0x5a`), and
 *      only a pointer that is MUTATED gets to reuse its own register.  The
 *      subscript spelling spends `mov r1, r0` first.
 *
 *   2. `ax = dx;` before the abs -- the copy-then-modify tell.  Removing it: 76
 *      differing.  The ROM emits `mov r2, r1` before `cmp r2,#0 / neg r2,r2`,
 *      i.e. it copies the difference before negating it, while
 *      `if (dx < 0) dx = -dx;` negates in place and is one instruction SHORT.
 *      Two live names, one expression, exactly as recorded.  ONLY dx wants it:
 *      adding the mirror `az = dz;` is a TIE, so the second copy is inert
 *      scaffolding and does not ship.  And the copy has to sit AFTER the dz
 *      subtraction -- hoisting `ax = dx;` above it is 76 differing.
 *
 *   3. THE MASK'S SIGNEDNESS, `& ~0xfff` NOT `& 0xfffff000`.  One encoding, and
 *      it is a pool WORD, not an instruction: see the NEW FINDING below.
 *
 *   4. `g = gState;` -- the recorded gState lever, unchanged.  Dropping the
 *      local base folds symbol+offset into `ldr r3, =gState+500`: 142 differing
 *      and 4 bytes SHORT.  Spelling the offset `0x1f4` instead of `(0xfa << 1)`
 *      is a TIE, which is the recorded reading -- the BASE wants the name, the
 *      offset must stay an expression.
 *
 *   5. THREE REGISTER PINS, and each is load-bearing:
 *          __MapActor_Surprise(0, 0x102)   dropped -> 2 differing
 *          __MapActor_SetSpeed(0, ...)     dropped -> 3 differing
 *          __MapActor_Emote(nearest, ...)  dropped -> 2 differing
 *      All three are the argument-interleave shape -- the ROM lands one cheap
 *      argument INSIDE another's split build (`mov r1,#0x81 / mov r0,#0 /
 *      lsl r1,#1`) or ahead of two pool loads (`mov r0,#0 / ldr r1,=0x1999 /
 *      ldr r2,=0xccc`), and unpinned gcc finishes each argument before starting
 *      the next.
 *
 * THE TEMPLATE'S CHEAPNESS TEST HOLDS HERE, AND IT PRUNED THE SWEEP BY 70%.
 * src/overlays/rom_798dc4/ovl_314_c_a_c_a_c.c records that an ALL-CHEAP call
 * site never needs an ordering pin.  This function has 26 call sites.
 * EIGHTEEN are all-cheap -- every argument a bare `mov rN, #imm8` or a plain
 * register move -- and NOT ONE of them is pinned, which is the template's
 * result reproduced on a second overlay family.  EIGHT carry an expensive
 * argument (a `mov`+`lsl` split build, a pool load, or a memory operand), and
 * THREE of those eight are pinned.  So the test is NECESSARY and not
 * sufficient, exactly as the template states it -- five expensive sites come
 * out bare here, against 11 of 53 there -- and its whole value is the prune: it
 * took the sweep from 26 candidates to 8, and the three pins were found in
 * three compiles.
 *
 * THE FIVE HOLES ARE READABLE, NOT SEARCHABLE, AND ONE OF THEM IS INSTRUCTIVE.
 * `__MapActor_Surprise(0, 0x100)` is the same `mov`+`lsl` shape as the PINNED
 * 0x102 site twenty instructions earlier, and it is already RIGHT unpinned:
 * pinning it is a TIE, so it is inert scaffolding and does not ship.  Which way
 * an interleave site falls is a SITE property, and here the discriminator is
 * visible in the ROM without compiling anything -- at 0x102 the zero sits
 * BETWEEN `mov r1,#0x81` and `lsl r1,#1`, which is not an order gcc reaches; at
 * 0x100 it sits AFTER the shift, which is gcc's own default.  Read the zero's
 * position against the shift before spending a compile.  The other four holes
 * are the two memory-operand sites and the `=0x101` pool load, none of which
 * has a second argument to be ordered against, and `__SetFlag(0x80 << 2)`,
 * whose shifted build is the only argument there is.
 *
 * NEW (measured here): A LARGE HEX MASK IS UNSIGNED, AND THAT IS WHAT LETS gcc
 * NARROW IT.  The ROM builds a 32-bit mask for a HALFWORD store:
 *
 *     ldrh r3,[r0,#6] / mov r2,#0x80 / lsl r2,#8 / add r3,r2
 *     ldr r2, =0xfffff000 / and r3,r2 / strh r3,[r5,#6]
 *
 * Written `& 0xfffff000` gcc pools `0xf000` instead -- one encoding wrong, in
 * the POOL rather than in the instruction stream, so tryc's `=value`
 * normalisation cannot see it and only objcmp catches it.  Written `& ~0xfff`
 * the pool word is the ROM's.  So is `& -0x1000`, and so is
 * `& (int)0xfffff000`; `& 0xfffff000u` is wrong again.  The lever is therefore
 * the mask operand's SIGNEDNESS, not its magnitude and not the spelling:
 * `0xfffff000` exceeds INT_MAX so C89 types it `unsigned int`, the `&` is done
 * unsigned, and gcc's combine then narrows it to the store's HImode.  A signed
 * mask survives.
 *
 * This SHARPENS "A mask applied to a byte gets NARROWED unless it is named" and
 * corrects the scope of "gcc narrows a mask that the ROM kept wide", which says
 * "`& -13` instead of `& ~0xc` is byte-identical: the narrowing happens on the
 * value's mode, not on how the constant is spelled".  That is true WITHIN the
 * signed spellings -- confirmed again here, `~0xfff` and `-0x1000` and
 * `(int)0xfffff000` are three ways of writing one match -- but it does not
 * cross the signed/unsigned line, and above 0x7fffffff the spelling chooses the
 * type.  It is the same C89 rule already recorded for comparisons at "A large
 * unsigned literal makes the COMPARISON unsigned", reaching a different pass:
 * there it changed `bge` to `bcs`, here it changes a pool word.  The named-int
 * cure the byte entry prescribes is ACTIVELY WRONG on this function -- `int
 * mask = 0xfffff000;` costs a register and scores 14 differing regions, and
 * reusing any of five dead locals for it scores 10 to 36.  Try the signed
 * spelling FIRST; it is free.
 *
 * MEASURED WORSE (objcmp encodings differing, from the finished file, one
 * change at a time):
 *
 *     `p[0x5a] &= / |=` for both mutated pointers ....  102, +8 bytes
 *     the same at one site only ......................  100, +4 bytes
 *     `ax = dx;` copy dropped ........................   76
 *     `ax = dx;` hoisted above the dz subtraction ....   76
 *     `g = gState;` dropped ..........................  142, -4 bytes
 *     one GetActor(0) reused for both q and p ........   75, -8 bytes
 *     `nearest = 9;` moved after __SetFlag ...........    5
 *     `best = d;` before `nearest = i;` ..............    2
 *     the SetSpeed pin dropped .......................    3
 *     the Surprise/0x102 pin dropped .................    2
 *     the Emote pin dropped ..........................    2
 *     `& 0xfffff000` (or `& 0xfffff000u`) ............    1  (pool word)
 *
 * TIES -- none of these is a lever, and the inert ones are not shipped:
 *     `& -0x1000`, `& (int)0xfffff000`, `& ~0xfffL`
 *     `0x1f4` for `(0xfa << 1)`
 *     `if (a)` / `if (p)` for `if (a != 0)` / `if (p != 0)`
 *     `extern void OvlFunc_933_2008344(void);` for the int/no-prototype form
 *     adding `az = dz;` beside `ax = dx;`
 *     adding a pin at __MapActor_Surprise(0, 0x100)
 *     ROM-ORDER fill inside a pin (`q1 = 0x82; q2 = 0; q0 = nearest;
 *       q1 <<= 1;`) against the uniform ASCENDING fill shipped here.  Worth
 *       recording against "INSIDE A PINNED FILL, THE SHIFT'S POSITION IS SOURCE
 *       ORDER": at all three sites here the fill order is a TIE and only the
 *       PIN does work, so that rule is a fallback to reach for when the uniform
 *       fill misses, not a transcription duty.
 *
 * THE OTHER SIX FUNCTIONS IN THIS .s WERE ASSESSED AND NOT ATTEMPTED.
 * 2009874 (19 insns) is the cheapest-looking, but it is the LAST function and
 * the trailing `.data` would travel with it into _b, which is precisely the
 * hazard split_s.py documents -- deleting _b.s would destroy the tables
 * 2008e2c references.  2009054 (133) and 2008e2c (237) both build a 0x28-byte
 * stack struct for OvlFunc_common0_10c and 2008e2c additionally reaches the two
 * `.L` data tables from C.  20092fc / 20094b0 / 2009638 are 158-248 instruction
 * scripts of the same family as this one and are the natural next targets; the
 * levers above should transfer, subject to the usual re-measurement.
 *
 * -- worked in scratch_elev/b244/f2009180/; ref.s is lines 1 + 393-558 of the
 *    original .s, and every number above is `tools/objcmp.py` against it.
 */
extern unsigned char gState[];
extern unsigned char *__MapActor_GetActor(int slot);
extern void __SetFlag(int id);
extern void __CutsceneWait(int n);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_SetAnimSpeed(int slot, int n);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_Surprise(int slot, int a);
extern void __MapActor_Emote(int slot, int a, int b);
extern void __MapActor_TravelTo(int slot, int x, int z);
extern void __Func_809280c(int a, int b, int c);
extern void __Func_809259c(int a, int b);
extern void __Func_8091eb0(int a, int b);
extern int OvlFunc_933_2008344();

#define PIN2 register int q0 __asm__("r0"); \
             register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")

void OvlFunc_933_2009180(void)
{
    unsigned char *g;
    unsigned char *me;
    unsigned char *a;
    unsigned char *p;
    unsigned char *q;
    int best;
    int nearest;
    int i;
    int dx;
    int dz;
    int d;
    int ax;

    g = gState;
    me = __MapActor_GetActor(*(int *)(g + (0xfa << 1)));
    nearest = 9;
    __SetFlag(0x80 << 2);
    best = 0x80 << 13;
    for (i = 9; i <= 12; i++) {
        a = __MapActor_GetActor(i);
        if (a != 0) {
            dx = (*(int *)(me + 8) - *(int *)(a + 8)) / 0x10000;
            dz = (*(int *)(me + 0x10) - *(int *)(a + 0x10)) / 0x10000;
            ax = dx;
            if (ax < 0)
                ax = -ax;
            if (dz < 0)
                dz = -dz;
            d = ax + dz;
            if (d < best) {
                nearest = i;
                best = d;
            }
        }
    }
    __MapActor_SetAnim(0, 1);
    p = __MapActor_GetActor(0);
    p += 0x5a;
    *p &= 0xfe;
    __Func_809280c(0, nearest, 0);
    __CutsceneWait(0x14);
    { PIN2; q0 = 0; q1 = 0x81 << 1; __MapActor_Surprise(q0, q1); }
    __Func_809259c(0, 2);
    __CutsceneWait(0x3c);
    __MapActor_Surprise(0, 0x101);
    q = __MapActor_GetActor(0);
    p = __MapActor_GetActor(0);
    *(short *)(q + 6) = (*(unsigned short *)(p + 6) + (0x80 << 8)) & ~0xfff;
    __MapActor_SetAnim(0, 5);
    __MapActor_SetAnimSpeed(0, 0x18);
    { PIN3; q0 = 0; q1 = 0x1999; q2 = 0xccc; __MapActor_SetSpeed(q0, q1, q2); }
    p = __MapActor_GetActor(0);
    *(int *)(p + 0x6c) = (int)OvlFunc_933_2008344;
    p = __MapActor_GetActor(nearest);
    if (p != 0)
        __MapActor_TravelTo(0, *(short *)(p + 0xa), *(short *)(p + 0x12));
    __CutsceneWait(0x3c);
    { PIN3; q0 = nearest; q1 = 0x82 << 1; q2 = 0; __MapActor_Emote(q0, q1, q2); }
    __CutsceneWait(0x3c);
    __MapActor_Surprise(0, 0x80 << 1);
    p = __MapActor_GetActor(0);
    p += 0x5a;
    *p |= 1;
    p = __MapActor_GetActor(0);
    *(int *)(p + 0x6c) = 0;
    __Func_8091eb0(0x35, 4);
}
