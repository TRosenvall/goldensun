/* OvlFunc_968_200a47c -- 0x0200a47c,
 * asm/overlays/rom_7f2f14/ovl_30_c_c_a_a_a_c.s
 *
 * ELEVEN differing encodings out of 273, at EXACT SIZE (616 bytes), exact
 * instruction count and EXACT RELOCATIONS.
 *      first at index 179: ref 1c03  ours 1c06
 * Candidate at scratch_elev/b248/f200a47c/final.c; d.sh / sweep.sh / pool.sh /
 * shape.sh in that directory rebuild any measurement below.
 *
 * LANDING SHAPE IF CLOSED: src/overlays/rom_7f2f14/ovl_30_c_c_a_a_a_c.c.
 * The .s holds ONE function and emits no .section/.data/.word/.byte, so the
 * whole TU is replaced -- no split, no linker edit. overlay.ld:78 already names
 * asm/overlays/rom_7f2f14/ovl_30_c_c_a_a_a_c.o(.text) and MUST STAY THAT WAY.
 * makefile_flags() is empty, WILDCARD_HITS empty, so default -O2.
 *
 * THE NEIGHBOUR HEURISTIC HELD DECISIVELY. neighbour.py ranked
 * src/overlays/rom_7ed0a0/ovl_30_c_a_a.c first at 11/18 shared symbols, and its
 * OvlFunc_964_2009d04 is a near-twin -- same swap through a stack `struct
 * Actor`, same __Func_80933d4/__Func_8093554/__Func_80933f8/__Func_8093530 run,
 * same f6c callback and SetFlag, differing only in base slot (0xc against 0xa),
 * loop bound (2 against 3) and one extra leading branch. Its struct and idiom
 * carried over verbatim and took the FIRST candidate to 180 differing.
 *
 * BLOCKER: gcc-2.96 THUMB NARROW-CONSTANT REMATERIALISATION, and the mechanism
 * is a HALFWORD STORE POISONING A LATER BYTE CONSTANT ACROSS A CALL.
 *
 * The honest source line is `__Func_8093554()[0x55] = 0;`. gcc commons that zero
 * with the earlier `b->f64 = 0` -- a HALFWORD store -- and, after the two
 * intervening calls, rematerialises it as `ldrh r5, .Ln` out of a MID-FUNCTION
 * POOL (`.word 0` plus a `b` around it, +8 bytes and +3 encodings) where the ROM
 * has a plain `mov r3, #0`.
 *
 * This is NOT the recorded halfword-pooling entry, which is about constants
 * `mov #imm8` cannot build. THE CONSTANT HERE IS ZERO. See the general entry in
 * docs/elevation.md ("A HALFWORD STORE POISONS A LATER NARROW CONSTANT ACROSS A
 * CALL") for the four-way discriminator and the minimal repro.
 *
 * Nothing at the C level breaks the commoning: int locals, unsigned char locals,
 * casts, reordering the stores, bitfields and separate pointer chains were all
 * tried and all still pool. The shipped candidate DODGES it with a `= b->f55`
 * readback, which costs a different thing -- it pins b+0x55 in r6 across both
 * calls -- and that readback is the entire remaining 11.
 *
 * LEVERS THAT ARE LOAD-BEARING (figure = what REMOVING it costs from 11/273):
 *      spilled-local declaration order (e1, e2, tp)             6
 *      separate flag/flag2 and t/t3 variables                   9
 *      inner-loop body hoisted out of line via `goto swap`      44
 *      tgt reusing the loop variable j                          7
 *      `i = 0;` placed before `tp = &tmp;`                      6
 *      pin r1 = 0 at the first OvlFunc_968_2008058              2
 *      pin r0, r1 at the second OvlFunc_968_2008058             2
 *      pin r5 = 0 for `a->f44 = 0`                              7
 *      pin r2 = 2 at the first `|= 2` site                      4
 *      `= b->f55` readback at __Func_8093554 (SCAFFOLD)       121, +8 bytes
 *
 * MEASURED WORSE OR INERT: `while (j<i)` and continue-style loops inert;
 * hand-rolled goto loop 199; `k = i` moved before the field stores inert;
 * literals instead of t3 in the a-block inert; explicit pointer chain in the
 * b-block inert; b-block zero pinned to r2/r1/r3 16-20; the __Func_8093554 zero
 * pinned to r0/r1/r2/r3 13/92/92/92; `= t3` 78; `= b->f14` 126; `= a->f55` 135;
 * `= (unsigned char)e1` 99; pinning __Func_80933d4's arguments inert.
 *
 * NOTE FOR THE RECORDED `orr` NARROW-LOCAL LEVER: it was BEATEN HERE by an
 * eviction pin. `unsigned char m = 2` at the first orr is 2 WORSE than the
 * r2 pin. The lever is not wrong, but it is not automatically the best
 * spelling at an orr site.
 */
