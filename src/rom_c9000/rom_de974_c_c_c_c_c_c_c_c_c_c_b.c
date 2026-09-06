/* Func_80df90c -- MATCHED.
 *
 *   objcmp: OK Func_80df90c -- 196 bytes, 87 encodings and 10 relocations identical
 *   (against asm/rom_c9000/rom_de974_c_c_c_c_c_c_c_c_c_c.s, and against a
 *   trimmed single-function reference; the two agree, so no Makefile pattern
 *   rule is biting.  Flags: stock GCC296_CFLAGS, no flag group needed.)
 *
 * Twin of Func_80b82c4 (asm/rom_b5000/rom_b8228_a_a.s).  Same routine with a
 * different scale constant: aim the actor at another combatant, set its speed
 * from the distance, and start it travelling.  The two are in DIFFERENT .s
 * files, so they are two separate landings.
 *
 * The suggested template (src/rom_c9000/rom_de974_c_c_c_c_c_c_c_c_c_b.c) gave
 * the actor-access spelling -- `int *p = GetBattleActor(id); unsigned char *a =
 * (unsigned char *)p[0];` with `*(int *)(a + 0x34)` field writes -- and nothing
 * else: it has no arithmetic, no call through a pointer and no second actor.
 * Everything below was re-measured on this function.
 *
 * LEVERS, with mechanisms
 *
 * 1. NAME THE MULTIPLIER.  `int k = 0x50;` ... `(k * d) / 100`.
 *    The ROM has `mov r2, #0x50 / mov r10, r2 / ... / mov r0, r10 / mul r0, r3`
 *    -- a real `mul` against a register.  Written as the literal `0x50 * d`,
 *    gcc-2.96 runs synth_mult at expand time and emits the shift chain
 *    `lsl r0, r3, #2 / add r0, r3 / lsl r0, #4` instead.  There is no constant
 *    propagation into a MULT for a named local, so the local survives to expand
 *    as a pseudo, the pattern needs two registers, and CSE then commons the one
 *    live constant into a callee-saved register across both call sites -- which
 *    is exactly the ROM's r10.  See the NEW FINDING note at the bottom.
 *    Cost of getting this wrong: 68 of 87 encodings.
 *
 * 2. LOAD b's FIELD BEFORE a's.  `int bx = *(int *)(b + 8); int ax = *(int *)(a
 *    + 8);` in that order, and again for +0x10.  The ROM reuses r8 for `b` and
 *    then for `ay`: those live ranges are disjoint only if `ay` is born after
 *    `b` dies.  Naming `ax`/`ay` first (the reading-order spelling) starts their
 *    live ranges while `b` is still live, the allocator needs a seventh
 *    callee-saved register, and the prologue grows a `push {r7}` the ROM does
 *    not have.  77 of 87 encodings, and the tell was the push list.
 *
 * 3. NAME *BOTH* STORE-BLOCK CONSTANTS, ASSIGNED BEFORE THE BLOCK.
 *    `one = 1; zero = 0;` ahead of the +0x34 store.  The ROM's store block
 *    carries the `1` in r1 and the `0` in r2 simultaneously while r3 cycles
 *    through the two byte-store addresses.  Written with bare literals, gcc
 *    gives the `1` r2 and lets the `0` share r3 with the first address, so the
 *    second address is pushed to r1 and three registers rotate.  Making both
 *    constants named locals born before the block forces their live ranges to
 *    overlap, which is what puts them in separate registers and frees r3 for
 *    both addresses.  This is docs/elevation.md "Two constants in DIFFERENT
 *    registers means they are simultaneously live" -- confirmed here for LOW
 *    registers, where that note was written about callee-saved ones -- and it
 *    is a direct counter-example to "The same function can want a named zero
 *    and a bare one", which says the `1` should stay a literal.  Read it off
 *    the ROM; this function wants both named.
 *
 * 4. The call through Func_8000948 is ordinary C: `int (*fp)(int); fp =
 *    Func_8000948; fp(mag);` gives `ldr r2, =Func_8000948 / bl _call_via_r2`.
 *    Spelling lifted from the solved corpus (src/overlays/rom_799abc/
 *    ovl_30_a_a_a_b.c and 14 siblings), not invented.  `int` return, matching
 *    the ROM's `mov r0, r3` before the call and the `lsl r0, #8` after.
 *
 * 5. `pop {r0} / bx r0` closes the function: void return, confirmed.  The
 *    prologue's four pushed high registers are pins, not carries.
 *
 * MEASURED WORSE (all at the same 84-line length unless noted)
 *
 *   variant                                                    differing
 *   a  literal `0x50 *`, ax/ay named first                          68
 *   b  named k, ax/ay named first                                   77   (+push {r7})
 *   c  named k, loads in ROM order, bare 1 and 0                     9
 *   d  as c + `q = a + 0x5a` named, assigned before the block        9
 *   e  as c + `q` assigned in the ROM's position                     9
 *   f  as e + `p = a + 0x58` named as well                           9
 *   g  as c + named `zero` only, assigned early                      6
 *   h  as c + named `one` only, assigned early                       9
 *   j  as c + named `zero` assigned late (at first use)              9
 *   l  both named but assigned at first use, not before the block    9
 *   i  BOTH named, assigned before the block                        OK  <- this file
 *   k  as i with `zero = 0;` before `one = 1;`                      OK  (order is free)
 *
 * The named-destination-pointer lever (d/e/f) is inert here in all three
 * placements: it moves the `add` but not the rotation.  It was the obvious
 * reading of "the ROM computes the byte-store address first" and it is not
 * what this function is stuck on.
 *
 * NEW FINDING (grepped by concept first: "synth_mult", "expand_mult",
 * "constant multiplier", "shift chain", "shifts instead of", "naming an
 * intermediate", "a local holding the constant" -- the only hit is
 * "## A NEGATIVE multiplier is what selects the shift chain", which is about
 * the opposite tell and does not cover this)
 *
 *   A POSITIVE constant multiplier also selects the shift chain, and a named
 *   local is what buys the ROM's `mul` back.  gcc-2.96 does not constant-
 *   propagate a local into a MULT before expand, so the choice is made purely
 *   on how the source spells the multiplier.  Isolated probe under production
 *   flags (scratch_elev/b239/f80df90c/probe_mul.c):
 *
 *     int lit1(int d) { return 0x50 * d; }
 *       -> mov r3, r0 / lsl r0, r3, #2 / add r0, r0, r3 / lsl r0, r0, #4
 *     int nam1(int d) { int k = 0x50; return k * d; }
 *       -> mov r3, #80 / mul r0, r0, r3
 *     int nam2(int d, int e) { int k = 0x50; return (k*d)/3 + (k*e)/3; }
 *       -> mov r6, #80 / mul / bl __divsi3 / mul / bl __divsi3
 *          with the 80 held in a pushed callee-saved register across both
 *
 *   The third case is this function's exact shape, including the pushed
 *   register.  So `mov rN, #K` into a callee-saved register followed by two
 *   `mul`s is not a CSE curiosity to be reverse-engineered -- it reports a
 *   named local in the source, and it is confirmed independently on the twin
 *   Func_80b82c4 (literal 0x4b: 86 differing and three lines SHORT; named k:
 *   exact on the first screen).
 *
 * LANDING
 *
 *   asm/rom_c9000/rom_de974_c_c_c_c_c_c_c_c_c_c.s holds TWO functions --
 *   Func_80df90c (target, first) and Func_80df9d0 -- plus a trailing
 *   `.section .rodata` with eight already-`.global` `.incrom` blobs.  A split
 *   is required; the whole file cannot land as one .c.
 *
 *       python3 tools/split_s.py \
 *           asm/rom_c9000/rom_de974_c_c_c_c_c_c_c_c_c_c.s Func_80df90c
 *
 *   The target is first, so no `_a` piece is written: the target goes to
 *   asm/rom_c9000/rom_de974_c_c_c_c_c_c_c_c_c_c_b.s and Func_80df9d0 plus the
 *   .rodata to ..._c_c.s.  Both destination suffixes are free -- no file named
 *   rom_de974_c_c_c_c_c_c_c_c_c_c_* exists anywhere under asm/ or src/.  This
 *   file then lands as src/rom_c9000/rom_de974_c_c_c_c_c_c_c_c_c_c_b.c and is
 *   built by the generic `asm/%.o: src/%.c` rule with stock GCC296_CFLAGS; no
 *   Makefile edit is needed and no flag group is involved.
 *
 *   stage1.ld names that .o on TWO lines, both of which must be remapped
 *   (cited by content, matched on the full path):
 *
 *       		asm/rom_c9000/rom_de974_c_c_c_c_c_c_c_c_c_c.o(.text)
 *       		asm/rom_c9000/rom_de974_c_c_c_c_c_c_c_c_c_c.o(.rodata)
 *
 *   split_s.py's rewrite_ld replaces EVERY section line with one line per
 *   written part, so the _b object acquires a `(.rodata)` line for a section it
 *   does not have.  That is the documented harmless case, not a defect.
 *   No other .ld under overlays/ mentions this object.
 */
extern void *_GetBattleActor(int id);
extern void _Actor_Stop(void *actor);
extern void _Actor_TravelTo(void *actor, int x, int a2, int y);
extern void _Actor_SetAnim(void *actor, int anim);
extern int Func_8000948(int);

void Func_80df90c(int self, int target, int speed)
{
    int *p0 = (int *)_GetBattleActor(self);
    int *p1 = (int *)_GetBattleActor(target);
    unsigned char *a = (unsigned char *)p0[0];
    unsigned char *b = (unsigned char *)p1[0];
    int k = 0x50;
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
    int one;
    int zero;

    u >>= 8;
    w >>= 8;
    mag = u * u + w * w;
    fp = Func_8000948;
    v = (fp(mag) << 8) / speed;

    one = 1;
    zero = 0;
    *(int *)(a + 0x34) = v;
    *(int *)(a + 0x30) = v;
    *(a + 0x58) = one;
    *(int *)(a + 0x48) = 0xab85;
    *(int *)(a + 0x28) = zero;
    *(int *)(a + 0x44) = zero;
    *(a + 0x5a) = one;
    _Actor_Stop(a);
    _Actor_TravelTo(a, x, 0, y);
    _Actor_SetAnim(a, 2);
}
