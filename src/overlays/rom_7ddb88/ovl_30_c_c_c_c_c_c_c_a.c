/* OvlFunc_955_2009898 -- asm/overlays/rom_7ddb88/ovl_30_c_c_c_c_c_c_c_a.s
 *
 * VERDICT: BYTE-EXACT.
 *   OK OvlFunc_955_2009898 -- 292 bytes, 125 encodings and 20 relocations identical
 * Verified against a scratch copy of the reference AND against the ORIGINAL
 * asm/ path; the two agree and objcmp printed no "(built with: ...)" line, so
 * no Makefile pattern rule is biting this unit.  FLAG GROUP: none -- plain
 * GCC296_CFLAGS via the generic `asm/%.o: src/%.c` rule at Makefile:146.  There
 * is no rule anywhere in the Makefile mentioning rom_7ddb88 or this stem, so
 * nothing needs adding and nothing needs narrowing.
 *
 * LANDING.  The .s holds EXACTLY ONE function (its only `.thumb_func_start` is
 * this one) and carries NO data -- no `.section`, `.data`, `.word` or `.byte`
 * line appears in its 140 lines; the `ldr r3, =gState` / `ldr r5, =0x3333`
 * pool words are assembler-generated inside .text.  So this is a WHOLE-FILE .c
 * at src/overlays/rom_7ddb88/ovl_30_c_c_c_c_c_c_c_a.c, the hand .s is deleted,
 * and overlays/rom_7ddb88/overlay.ld needs NO EDIT: it already names the same
 * .o, which the generic cross-dir rule now builds from the .c.  Cited by
 * content, the .ld lines naming this object are
 *
 *     asm/overlays/rom_7ddb88/ovl_30_c_c_c_c_c_c_c_a.o(.text)
 *     asm/overlays/rom_7ddb88/ovl_30_c_c_c_c_c_c_c_a.o(.data)
 *     asm/overlays/rom_7ddb88/ovl_30_c_c_c_c_c_c_c_a.o(.data1)
 *
 * all three in overlays/rom_7ddb88/overlay.ld, each first in its group and
 * followed by the _b and _c siblings.  The two `.data`/`.data1` lines are
 * boilerplate for the whole group -- checked against the .s, this object
 * contributes nothing to either, so they stay exactly as they are.
 *
 * WHAT IT IS.  A two-actor cutscene step.  `lead` is the actor id parked at
 * gState+0x1f4 (the party leader); `b` is the map actor named by the caller's
 * slot and `a` is the leader's actor.  The caller passes a destination as two
 * tile-ish coordinates.  The function decides whether the LEADER should trail
 * along the x axis or the z axis -- it compares b's current x tile against
 * x/2, and the axis that already agrees is the one it does not move on -- then
 * walks b to the destination, matches the leader's speed to b's, walks the
 * leader by the same delta on the chosen axis, waits for both, and closes with
 * two sounds and a pause.
 *
 * THE ONE LEVER THAT DECIDED IT: gState's OFFSET MUST BE BUILT, NOT FOLDED.
 * Written `*(int *)(gState + 0x1f4)` gcc folds symbol and displacement into a
 * single pool word and emits `ldr r3, =gState+500 / ldr r3, [r3]`; the ROM
 * spends four instructions building it:
 *
 *     ldr r3, =gState / mov r2, #0xfa / lsl r2, #1 / add r3, r2 / ldr r3, [r3]
 *
 * Assigning the symbol to a local `unsigned char *gs` first makes the local
 * hold an address as a VALUE, so `+ 0x1f4` has to be real arithmetic.  0x1f4
 * is reachable as 0xfa << 1 and 500 is not reachable by `mov` at all, which is
 * why the folded form has to pool it -- the pool entry is the tell.  This is
 * docs/elevation.md "The gState offset must be BUILT, not folded"; the local
 * sibling src/overlays/rom_7ddb88/ovl_30_c_c_c_a_c_a_b.c uses the same idiom.
 * Dropping `gs` is 114 differing of 125 and eight bytes SHORT, i.e. it is not a
 * cosmetic difference, it deletes four instructions.
 *
 * THE FLAG IS AN EXPRESSION-ASSIGNMENT, NOT AN `if`.  The ROM materialises the
 * predicate into r0 and only tests it four instructions later:
 *
 *     mov r0, #0 / cmp r2, r3 / beq .L18dc / mov r0, #1
 *     .L18dc: ... lsl r3, #16 / lsl r6, #16 ... cmp r0, #0 / beq .L18f8
 *
 * The 0 is hoisted ABOVE the compare and the 1 is out of line, which is
 * exactly the `ok = (A != B);` shape of the three in "Branch polarity has a
 * THIRD face: boolean MATERIALISATION".  Spelling the test directly in the
 * `if` collapses the materialisation into a single compare-and-branch and
 * costs 105 of 125 (twelve bytes short).  The mechanism is that the two `<<=`
 * statements sit BETWEEN the flag's assignment and its test, so the flag is
 * live across them and has to occupy a register.
 *
 * POLARITY IS `!=` WITH THE X ARM FIRST.  `==` with the two arms swapped is
 * the same 125 encodings and the same 292 bytes -- and 86 of them differ.  A
 * matching length here proves nothing; per "A large early deficit is NOT
 * diagnosed by branch polarity", read the arms, do not infer them from size.
 *
 * THE `<<= 16` MUST MUTATE THE PARAMETERS.  x and z are shifted in place and
 * the shifted values then live in r6 and r10 across every call to the end of
 * the function -- a carried value, per "REBUILT or CARRIED".  Introducing
 * fresh locals `xx = x << 16; zz = z << 16;` gives gcc a second pseudo to
 * place and it builds them in the wrong block: 94 of 125, and four bytes LONG.
 * The parameter is already the carrier, and naming the value gcc is carrying
 * destroys the carry.
 *
 * NO PINS, NO BARRIERS, NO SCAFFOLDING.  Every argument fill falls out of
 * plain statements; the function needed no `__asm__` register pin at any of
 * the four regions its branches cut it into.
 *
 * ONE NAMED LOCAL WAS MEASURED INERT AND IS NOT HERE.  The first candidate
 * named `bx = *(int *)(b + 8);` because the ROM plainly keeps that load in r1
 * across the compare and the subtraction.  It is byte-identical either way --
 * gcc CSEs the load by itself -- so per "screen the unnamed spelling first"
 * the name is scaffolding and was deleted.  Dropping it re-opened the later
 * sites, and re-measuring `gs`, the flag and `lead` on the reduced base
 * reproduced the same three numbers, so the drop is safe in both directions.
 *
 * MEASURED-WORSE SPELLINGS (differing encodings of 125; shipped is 0)
 *
 *   spelling                                                        differing
 *   ---------------------------------------------------------------  -------
 *   SHIPPED (this file)                                                    0
 *   drop the named `bx` ......................... (inert; dropped)         0
 *   flag typed `unsigned char` instead of `int` . (free; kept int)         0
 *   `lead` unnamed, gState re-read at all four sites ..............      118
 *   `gs` dropped, `gState + 0x1f4` folded .........................      114
 *   `xx`/`zz` fresh locals instead of `x <<= 16` ..................       94
 *   the flag inlined into the `if` ................................      105
 *   the two shifts moved ABOVE the flag's assignment ..............      107
 *   `==` with the two arms swapped ............... (same 292 bytes)        86
 *
 * The flag's type being free and De Morgan being free are both already on
 * record in the boolean-materialisation section; the two measurements here
 * agree with it and add nothing new.  Nothing in this function is a new
 * finding -- every lever is a named rule in docs/elevation.md applied as
 * written.
 */
extern unsigned char gState[];
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __Actor_SetAnim(void *actor, int anim);
extern void __Actor_TravelTo(void *actor, int x, int y, int z);
extern void __Actor_WaitMovement(void *actor);
extern void __CutsceneWait(int frames);
extern void __PlaySound(int id);

void OvlFunc_955_2009898(int slot, int x, int z)
{
    unsigned char *gs;
    unsigned char *a;
    unsigned char *b;
    int lead;
    int off;
    int dx;
    int dz;

    gs = gState;
    lead = *(int *)(gs + 0x1f4);
    a = __MapActor_GetActor(lead);
    b = __MapActor_GetActor(slot);
    off = (*(int *)(b + 8) >> 20) != x / 2;
    x <<= 16;
    z <<= 16;
    if (off) {
        dx = (x - *(int *)(b + 8)) / 2;
        dz = 0;
    } else {
        dx = 0;
        dz = (z - *(int *)(b + 0x10)) / 2;
    }
    __MapActor_SetAnim(lead, 8);
    __CutsceneWait(6);
    *(int *)(b + 0x30) = 0x8000;
    *(int *)(b + 0x34) = 0x3333;
    __PlaySound(0xef);
    __Actor_SetAnim(b, 3);
    __Actor_TravelTo(b, x, 0, z);
    __CutsceneWait(6);
    __MapActor_SetAnim(lead, 2);
    __MapActor_SetSpeed(lead, 0x8000, 0x3333);
    __Actor_SetAnim(a, 2);
    __Actor_TravelTo(a, *(int *)(a + 8) + dx, 0, *(int *)(a + 0x10) + dz);
    __Actor_WaitMovement(a);
    __Actor_SetAnim(a, 1);
    __Actor_WaitMovement(b);
    __Actor_SetAnim(b, 1);
    __PlaySound(0x120);
    __PlaySound(0xd5);
    __CutsceneWait(0xf);
}
