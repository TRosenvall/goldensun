/* Func_80b82c4 -- MATCHED, first screen.
 *
 *   objcmp: OK Func_80b82c4 -- 208 bytes, 93 encodings and 10 relocations identical
 *   (against asm/rom_b5000/rom_b8228_a_a.s, and against a trimmed
 *   single-function reference; the two agree, so no Makefile pattern rule is
 *   biting.  Flags: stock GCC296_CFLAGS, no flag group needed.)
 *
 * Twin of Func_80df90c.  They are NOT in the same .s -- Func_80df90c lives in
 * asm/rom_c9000/rom_de974_c_c_c_c_c_c_c_c_c_c.s and this one in
 * asm/rom_b5000/rom_b8228_a_a.s -- so they are two independent landings.  The
 * suggested shared template (src/rom_c9000/rom_de974_c_c_c_c_c_c_c_c_c_b.c)
 * contributed only the actor-access spelling; the twin contributed everything
 * else, and this file was written by transcribing Func_80df90c's solved shape
 * with three edits: k = 0x4b, the +0x28 store takes the fourth argument instead
 * of zero, and the guarded duplicate store.
 *
 * DIFFERENCES FROM THE TWIN, read off this ROM
 *
 *   scale constant 0x4b, not 0x50; divisor is argument 3 (r9 here, r11 there)
 *   no store to +0x44
 *   +0x28 takes argument 4 (r11), stored TWICE
 *   an `if` on bit 2 of the byte at +0x55 guards the first of those stores
 *   four high registers pushed, not three
 *
 * THE DUPLICATE STORE IS REAL.  The ROM is
 *
 *     strb r1, [r3]        @ +0x58 = 1
 *     sub  r3, #3          @ -> +0x55
 *     ldrb r2, [r3]
 *     mov  r3, #4 / and r3, r2 / cmp r3, #0 / beq .Lb8352
 *     mov  r2, r11 / str r2, [r7, #0x28]
 *   .Lb8352:
 *     mov  r3, r11 / str r3, [r7, #0x28]
 *
 * -- the guarded arm falls through into an unconditional store of the same
 * value to the same field.  It is dead in effect but it is in the source, and
 * writing it out plainly reproduces it.  The `sub r3, #3` is gcc's own
 * arithmetic on the +0x58 address, not a source-level offset (same shape as
 * the template's `add r2, #90 / sub r2, #2`), so `a[0x55]` and `a[0x58]` as
 * independent expressions are the right spelling.
 *
 * LEVERS, with mechanisms
 *
 * 1. NAME THE MULTIPLIER: `int k = 0x4b;`.  Same mechanism as the twin --
 *    gcc-2.96 does not constant-propagate a named local into a MULT before
 *    expand, so the local survives as a pseudo and the `mul` pattern is used;
 *    the literal `0x4b * d` goes through synth_mult and comes out as a shift
 *    chain.  MEASURED HERE, not transplanted on faith: see the table.
 * 2. LOAD b's FIELD BEFORE a's, for both +8 and +0x10.  The ROM reuses r6 (`b`)
 *    for `ay`, which needs the two live ranges disjoint.
 * 3. `int (*fp)(int); fp = Func_8000948; fp(mag);` for `bl _call_via_r2`.
 * 4. `GetBattleActor` here has NO leading underscore (same-region call), where
 *    the twin in rom_c9000 calls `_GetBattleActor`.  Read it off the .s.
 *
 * LEVER DELIBERATELY *NOT* TRANSPLANTED
 *
 *   The twin's headline store-block lever -- name BOTH the `1` and the `0` and
 *   assign them before the block -- has nothing to bite on here: this function
 *   has no shared zero, only the shared `1`, so there is no second constant to
 *   force a live-range overlap with.  Bare literals are correct.  Measured
 *   rather than assumed: transplanting a named `one` assigned before the block
 *   is NEUTRAL (still OK), so it costs nothing here, but it would have been an
 *   untested import.  The twin's other two levers were measured and both are
 *   load-bearing.
 *
 * MEASURED WORSE
 *
 *   variant                                              lines    differing
 *   t_c  literal `0x4b *` instead of named k             88 vs 91      86
 *   t_a  as written                                      91 vs 91      OK  <- this file
 *   t_b  as t_a + named `one` (twin's lever imported)    91 vs 91      OK  (inert)
 *
 *   The literal-multiplier variant is THREE LINES SHORT, which under the
 *   length-tell reads like a missing construct; it is not, it is one shift
 *   chain replacing two mov/mul pairs.
 *
 * LANDING
 *
 *   asm/rom_b5000/rom_b8228_a_a.s holds TWO functions --
 *   SetBattleActorKnockback (first) and Func_80b82c4 (target, last) -- and no
 *   data section, no literal-pool labels outside the function bodies.  A split
 *   is required.
 *
 *       python3 tools/split_s.py asm/rom_b5000/rom_b8228_a_a.s Func_80b82c4
 *
 *   The target is last, so no `_c` piece is written: SetBattleActorKnockback
 *   goes to asm/rom_b5000/rom_b8228_a_a_a.s and the target to
 *   asm/rom_b5000/rom_b8228_a_a_b.s.  Both destination suffixes are free -- no
 *   file named rom_b8228_a_a_* exists anywhere under asm/ or src/ (the
 *   existing rom_b8228_a_b.* is a different stem).  This file then lands as
 *   src/rom_b5000/rom_b8228_a_a_b.c, built by the generic `asm/%.o: src/%.c`
 *   rule with stock GCC296_CFLAGS; no Makefile edit, no flag group.
 *
 *   stage1.ld names that .o on exactly ONE line (cited by content, matched on
 *   the full path):
 *
 *       		asm/rom_b5000/rom_b8228_a_a.o(.text)
 *
 *   No .rodata or .data line, and no .ld under overlays/ mentions this object.
 *   The two functions' local labels do not cross the cut (.Lb8266/.Lb826c/
 *   .Lb829c belong to SetBattleActorKnockback, .Lb8352 to the target), and the
 *   `=.Lc59a4`-family pool loads are all in the first function, so nothing new
 *   needs exporting.
 */
extern void *GetBattleActor(int id);
extern void _Actor_Stop(void *actor);
extern void _Actor_TravelTo(void *actor, int x, int a2, int y);
extern void _Actor_SetAnim(void *actor, int anim);
extern int Func_8000948(int);

void Func_80b82c4(int self, int target, int speed, int val)
{
    int *p0 = (int *)GetBattleActor(self);
    int *p1 = (int *)GetBattleActor(target);
    unsigned char *a = (unsigned char *)p0[0];
    unsigned char *b = (unsigned char *)p1[0];
    int k = 0x4b;
    int bx = *(int *)(b + 8);
    int ax = *(int *)(a + 8);
    int u = (k * (bx - ax)) / 100;
    int by = *(int *)(b + 0x10);
    int ay = *(int *)(a + 0x10);
    int w = (k * (by - ay)) / 100;
    int x = ax + u;
    int y = ay + w;
    int mag;
    int v;
    int (*fp)(int);

    u >>= 8;
    w >>= 8;
    mag = u * u + w * w;
    fp = Func_8000948;
    v = (fp(mag) << 8) / speed;

    *(int *)(a + 0x34) = v;
    *(int *)(a + 0x30) = v;
    *(a + 0x58) = 1;
    if (*(a + 0x55) & 4)
        *(int *)(a + 0x28) = val;
    *(int *)(a + 0x28) = val;
    *(int *)(a + 0x48) = 0xab85;
    *(a + 0x5a) = 1;
    _Actor_Stop(a);
    _Actor_TravelTo(a, x, 0, y);
    _Actor_SetAnim(a, 2);
}
