/* OvlFunc_922_2008ed8 -- NOT MATCHING
 *
 * NON-MATCHING, 8 of 41 encodings  (MEASURED, batch 319 recipe backfill).
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/ovl_7a8c8c/2008ed8.c \
 *     asm/overlays/rom_7a8c8c/ovl_30_c_a_c_c_c_a_c_c_c_c_a_a.s --func OvlFunc_922_2008ed8
 *
 * This recipe was ADDED by the batch-319 backfill: the park had none, so
 * parkcheck.py could not report its figure and nothing had ever checked it.
 *
 * Source asm: goldensun/asm/overlays/rom_7a8c8c/ovl_30_c_a_c_c_c_a_c_c_c_c_a_a.s
 * Best screen: 8 differing of 43, streams the same length.
 *
 * BLOCKER CLASS: register allocation -- the sprite pointer lands in r0 where
 * the ROM has r1, and the +0x55 address is computed after the flag store rather
 * than before it.
 *
 * THREE LEVERS FROM THE TREE ALL APPLY HERE AND ALL WERE NEEDED to get this
 * far, which is why the file is worth keeping even unmatched:
 *
 *   the mask is a named `int` (`mask = ~0xc`), so it is built 32-bit with
 *   `mov r3, #0xd / neg r3, r3` rather than narrowed to `mov r3, #0xf3`
 *   -- batch 92, src/rom_8a000/rom_8d9a4_c_a_c_c_c_c_c_c.c
 *
 *   the OR'd 2 is a named `unsigned char`, which puts the CONSTANT in the
 *   destination of the `orr` -- batch 97,
 *   src/overlays/rom_7ced6c/ovl_30_c_c_c_c_c_a_a_c_b.c
 *
 *   the null test is written positive, `if (n != 0) { ... return n; }
 *   return 0;`, so the return constant is not hoisted above it -- batch 96,
 *   src/non_matching/ovl_common/common0_18.c
 *
 * WHAT WAS TRIED AGAINST THE RESIDUE:
 *   - the +0x55 write through a named pointer computed before the flag store
 *     (the batch-97 two-pointer lever): WORSE, 45 lines and 32 differing,
 *     because it forces r12 into use
 *   - the sprite dereferenced inline instead of through a local (44 lines, 30)
 *   - the +0x55 store moved ahead of the flag store in the source (43 lines, 11)
 *
 * The plain field write below is the best of the four. This is the same family
 * as src/non_matching/ovl_common/common0_18.c, which has the identical masked
 * byte and is parked on the identical exchange -- except that THIS one also has
 * the `| 4` that makes src/rom_8a000/rom_8d9a4_c_a_c_c_c_c_c_c.c match, so the
 * "does the and feed an orr" theory recorded in docs/elevation.md does NOT
 * explain the split. That theory should be treated as refuted.
 *
 * BATCH 329 (brief H).  FIGURE RE-DERIVED AND HELD: 8 differing encodings of 41.
 * Exact length -- objcmp prints no SIZE, no INSTRUCTION COUNT and no POOL WORD
 * COUNT line, so size, instruction count and pool words all agree.  Pin-free.
 * Not a member of any tools/dupfuncs.py group (7 groups / 14 functions, none
 * here).
 *
 * THE DIAGNOSIS IS NARROWED TO ONE QUANTITY.  The park calls this register
 * allocation plus an ordering ("the +0x55 address is computed after the flag
 * store").  The ordering is not independent: ALL EIGHT differences follow from
 * ONE choice, which hard register the sprite pointer gets.  The ROM gives it
 * r1 and we give it r0, and the ROM's reason is that r0 IS STILL HOLDING `n`
 * from `bl __CreateActor`, so reload_cse deletes the `mov r0, r5` that
 * __Actor_SetSpriteFlags' first argument would otherwise need -- and the ROM
 * then has to materialise `mov r1, #0x0` fresh for the second argument.  We put
 * the sprite pointer in r0, so we must emit `mov r0, r5`, and we pay for it by
 * reusing the r1 zero left over from the `f55` store as the second argument.
 * The two instruction counts balance at 43 lines for exactly that reason.
 *
 *     rom    ldr r1, [r5, #0x50] ... mov r2, r5 / add r2, #0x55 /
 *            strb r3, [r1, #0x9] / mov r3, #0x0 / strb r3, [r2] / mov r1, #0x0
 *     ours   ldr r0, [r5, #0x50] ... strb r3, [r0, #0x9] / mov r3, r5 /
 *            add r3, #0x55 / mov r1, #0x0 / strb r1, [r3] / mov r0, r5
 *
 * So the question to ask of this park is ONLY: how does a pseudo come to occupy
 * r0 across the sprite-pointer live range?  The +0x55 ordering, the `orr`
 * destination and the two zeros are all downstream of it and should not be
 * probed separately.
 *
 * AND ONE THING THAT READING DOES NOT EXPLAIN, which is where the next round
 * should start.  Our sprite pointer takes r0 -- ARM's REG_ALLOC_ORDER opens
 * 3, 2, 1, 0 (VERIFIED verbatim, config/arm/arm.h), so r0 is the LAST of the four low registers gcc reaches for.
 * Over the sprite pointer's whole live range (the `ldr` to the `strb`) r2 and
 * r3 are busy with the byte, the mask and the 4, but **r1 is free** -- the zero
 * that ends up in r1 is not defined until after the `strb`.  A plain
 * first-free-in-REG_ALLOC_ORDER walk would therefore have given it r1, which is
 * the ROM's answer.  It did not, so something is EXCLUDING r1 from that qty,
 * or the qty carries a suggestion toward r0 (local-alloc records a suggested
 * hard register from a copy; the __CreateActor return in r0 is the only
 * candidate).  READ `.17.lreg` AND `.18.greg` FOR THIS QTY BEFORE SPELLING
 * ANYTHING: the sprite pointer is block-local, so per the standing note it will
 * be ABSENT from .18.greg's allocno list and the decider is find_free_reg's
 * exclusion set, which is the thing to print.
 *
 * MEASURED THIS BATCH, all 8 unless noted, all at exact length unless noted:
 *   a second variable `m = n` inside the test, passed to
 *     __Actor_SetSpriteFlags, so the argument has its own pseudo   8 (inert)
 *   `m` taken from the call and `n` copied from `m`                8 (inert)
 *   the above plus the batch-328 two-zero split
 *     (`zi = 0; z = zi; n->f55 = z;`)                              8 (inert)
 *   the two-zero split alone                                       8 (inert)
 *   the sprite pointer in a nested block, shortest range           8 (inert)
 *   the `n->f55 = 0` store moved ahead of the masked byte write
 *     (37 instructions against 38, a pad absorbing it)             30 (WORSE)
 *
 * The four inert rows are one hypothesis, not four: that a source-level second
 * reference to `n` can keep a pseudo in r0.  It cannot -- gcc coalesces the copy
 * away in every spelling, so DO NOT re-run that family.  The last row restates
 * the park's own "+0x55 store moved ahead" result from the other side: moving a
 * store across the masked read changes the instruction COUNT, so any row in
 * this park that reorders the two stores is measuring misalignment.
 */
struct Spr { unsigned char pad00[9]; unsigned char f9; };

struct A {
    unsigned char pad00[0x23];
    unsigned char f23;
    unsigned char pad24[0x50 - 0x24];
    struct Spr *f50;
    unsigned char pad54[1];
    unsigned char f55;
};

extern struct A *__CreateActor(int a, int b, int c, int d);
extern void __Actor_SetSpriteFlags(struct A *a, int f);
extern void __Func_80929d8(struct A *a, int n);

struct A *OvlFunc_922_2008ed8(int a, int b, int c, int d)
{
    struct A *n;
    struct Spr *s;
    unsigned char *q23;
    unsigned char two;
    int mask;

    n = __CreateActor(d, a, b, c);
    if (n != 0) {
        s = n->f50;
        mask = ~0xc;
        s->f9 = (mask & s->f9) | 4;
        n->f55 = 0;
        __Actor_SetSpriteFlags(n, 0);
        __Func_80929d8(n, 0xf);
        q23 = &n->f23;
        two = 2;
        *q23 = two | *q23;
        return n;
    }
    return 0;
}
