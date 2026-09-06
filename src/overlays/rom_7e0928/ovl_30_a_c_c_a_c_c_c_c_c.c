/* OvlFunc_956_20084a4 -- 0x020084a4, the WHOLE of
 * asm/overlays/rom_7e0928/ovl_30_a_c_c_a_c_c_c_c_c.s (one .thumb_func_start,
 * one .func_end, 93 instructions, no `.word`/`.byte`/`.section` of any kind --
 * so no data, and a whole-file .c replaces it with NO linker edit).
 *
 * Total .text for this TU = 216 bytes.  The one linker line naming the object,
 * in overlays/rom_7e0928/overlay.ld, is
 *
 *     asm/overlays/rom_7e0928/ovl_30_a_c_c_a_c_c_c_c_c.o(.text)
 *
 * (one TAB-indented line; that is the only line in any .ld naming this .o)
 *
 * and it stays exactly as it is: the `asm/%.o: src/%.c` cross-dir rule builds
 * that same object from this file.  It sits between
 * asm/overlays/rom_7e0928/ovl_30_a_c_c_a_c_c_c_c_b.o and
 * asm/overlays/rom_7e0928/ovl_30_a_c_c_b.o, so ROM layout is preserved.
 *
 * NO FLAG GROUP.  Nothing in the Makefile mentions rom_7e0928; this builds
 * with the default GCC296_CFLAGS through the generic `asm/%.o: src/%.c` rule.
 *
 * What it does: if map actor 0xc is standing on tile (9, 0xc), re-fetch it and
 * send three actors (0xc, 0xb, 0xa) travelling with the same speed/accel pair,
 * set save flag 0x368, and run two CopyMapRectAttributes calls whose last two
 * stack arguments are the two tile coordinates that were just tested.
 *
 *
 * FOUR LEVERS, each re-measured against THIS file (see the table below); every
 * one of them is load-bearing and nothing inert is left in.
 *
 * 1. FOUR POINTER VARIABLES, NOT ONE (`a`, `b`, `c`, `d`).  Only `b` is live
 *    across a call (__Actor_SetSpriteFlags), so only `b` may take a
 *    callee-saved register -- the ROM has it in r5 and reaches actors 0xb and
 *    0xa straight out of r0.  Reusing one variable for all four fetches gives
 *    the union of the live ranges one register, r7 across the whole body, and
 *    costs a fourth high register (r11), two extra prologue moves, a `push
 *    {r7}` and a `mov rN, r7` before every field access: 69 of 94, and 96
 *    lines against 94.  This is docs/elevation.md's "A variable with DISJOINT
 *    live ranges should be two variables", on a four-way split.
 *
 * 2. STRUCT MEMBER TAGS, NOT `unsigned char *` ARITHMETIC.  The two narrow
 *    stores in the first block are `+0x23` and `+0x55`; both offsets are out of
 *    thumb `strb`'s 5-bit immediate range, so each needs its address built.
 *    Written through `unsigned char *` the two stores share ALIAS SET 0, gcc
 *    keeps one address pseudo and reaches the second store with `add r2,#0x32`,
 *    and the scheduler then wedges the second store's zero AFTER the first
 *    store instead of before it -- which changes which scratch register the
 *    `2` gets (r2/r3 instead of the ROM's r1) and cascades through both later
 *    blocks: 33 of 94.  Distinct struct tags put the two stores in distinct
 *    alias sets and all of it lands.  This is the recorded "give EACH store its
 *    own struct tag" rule; what is worth adding is that here the tags also
 *    made THREE other levers inert (see the RETIRED list).
 *
 * 3. `q2` -- ONE NAMED INT FOR THE SHIFTED THIRD ARGUMENT.  The ROM rebuilds
 *    `mov r2,#0x80 / lsl r2,#0xb` at the first and third __Actor_TravelTo, with
 *    no branch between them.  A bare literal 0x40000 is CSE'd into a seventh
 *    long-lived value, which needs r11 and its prologue/epilogue moves: 94 of
 *    94, 97 lines.  Writing the build as `q2 = 0x80; ...; q2 <<= 11;` keeps the
 *    mov and the shift as two separate insns that the argument loads can be
 *    scheduled between, and -- because `q2` is dead at each call -- gcc has to
 *    rebuild it at the third site rather than carry it.  A CALL-CLOBBERED PIN
 *    (`register int q2 __asm__("r2")`) also reaches this and was what first
 *    got the length to 94; it is NOT needed once levers 1, 2 and 4 are in, so
 *    it is not here.  No pin, no fakematch.
 *
 * 4. NAME THE OTHER TWO ARGUMENTS (`x1/z1`, `x2/z2`, `x3/z3`).  The ROM's
 *    argument fill is CROSSED -- `mov r2,#0x80 / ldr r1 / ldr r3 / lsl r2` --
 *    and gcc fuses each mov with its own shift unless something is available to
 *    put between them.  Naming the two loads per call site, assigned between
 *    `q2 = 0x80;` and `q2 <<= N;`, is exactly docs/elevation.md's "The
 *    interleave lever moves the argument you do NOT name": leave the value you
 *    want moved (the shift) alone and name its neighbours.  Without them the
 *    two loads sink below the shift at the first call site: 3 of 94.  Note the
 *    named values here are MEMORY LOADS, not constants, and the site is guarded
 *    (both `if`s dominate all three fills), which is the condition the
 *    "worth re-attacking" section asks for.
 *
 *
 * MEASURED WORSE (all against this file, one lever removed at a time; screened
 * with tools/tryc.py --ref against the original asm/ path):
 *
 *     lever removed                        lines   differing
 *     -----------------------------------  -----   ---------
 *     one pointer variable for all four     96      69
 *     `unsigned char *` instead of struct   94      33
 *     literal 0x40000 instead of `q2`       97      94
 *     inline loads instead of x1/z1/...     94       3
 *     (this file)                           94       0
 *
 * RETIRED SCAFFOLDING -- these three were each necessary at the point they were
 * found and INERT in the finished file, which is why the sweep was re-run at
 * the end rather than trusted from when each was measured:
 *
 *   * `unsigned char *p = b + 0x23; *p = f; p += 0x32; *p = g;`  -- the named
 *     destination pointer.  Worth 65 -> 25 differing at the time.  Superseded
 *     by lever 2.
 *   * `int f = 2;` and `int g = 0;` named ahead of the two narrow stores, with
 *     `g` assigned BEFORE the first store.  Worth 25 -> 20 and 20 -> 0 in the
 *     `unsigned char *` spelling.  Both inert with struct tags.
 *   * the `register int q2 __asm__("r2")` pin (see lever 3).
 *
 * NEW, and grepped for first ("crossed mov", "reload temp", "scratch
 * register", "born earlier", "assignment position"): the docs record
 * "Hoisting a constant's assignment ABOVE an unrelated load flips
 * HIGH-register allocation", explained by allocno priority
 * (floor_log2(n_refs)*n_refs/live_length) moving a value one slot down
 * REG_ALLOC_ORDER.  The SAME lever runs in the CALL-CLOBBERED half of
 * REG_ALLOC_ORDER {3,2,1,0,...} and it works by CONFLICT, not by priority.
 * Measured here in the `unsigned char *` spelling of the two narrow stores:
 * with `g = 0;` assigned after the first store, the `2` is built and reloaded
 * in r2/r3 (20 of 94); hoisting `g = 0;` above the first store makes the zero
 * live across it, r3 and r2 are both taken, and the `2`'s copies drop to the
 * ROM's r1 -- exact.  Four placements of `g` were measured: anywhere BEFORE
 * the first store is exact, anywhere after it is 20 differing.  Useful as a
 * general recogniser: "our low scratch register is one slot too high" means a
 * neighbouring value's live range is too SHORT, not that the allocator is
 * unreachable.  It is not needed in this file (struct tags reach the same
 * schedule) but it is the cheaper fix wherever a struct tag is not available.
 */
#include "actor.h"

extern struct Actor *__MapActor_GetActor(int slot);
extern void __Actor_SetSpriteFlags(struct Actor *a, int n);
extern void __Actor_TravelTo(struct Actor *a, int x, int y, int z);
extern void __SetFlag(int id);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

void OvlFunc_956_20084a4(void)
{
    struct Actor *a;
    struct Actor *b;
    struct Actor *c;
    struct Actor *d;
    int q2;
    int x1, z1, x2, z2, x3, z3;
    int tx;
    int tz;

    a = __MapActor_GetActor(0xc);
    tx = a->pos.x >> 20;
    if (tx == 9) {
        tz = a->pos.z >> 20;
        if (tz == 0xc) {
            b = __MapActor_GetActor(0xc);
            __Actor_SetSpriteFlags(b, 0);
            b->flags = 2;
            b->interactFlag = 0;
            b->accel = 0x6666;
            b->speed = 0xcccc;
            q2 = 0x80;
            x1 = b->pos.x;
            z1 = b->pos.z;
            q2 <<= 11;
            __Actor_TravelTo(b, x1, q2, z1);

            c = __MapActor_GetActor(0xb);
            c->flags = 2;
            c->accel = 0x6666;
            c->speed = 0xcccc;
            q2 = 0x80;
            x2 = c->pos.x;
            z2 = c->pos.z;
            q2 <<= 14;
            __Actor_TravelTo(c, x2, q2, z2);

            d = __MapActor_GetActor(0xa);
            d->accel = 0x6666;
            d->speed = 0xcccc;
            q2 = 0x80;
            x3 = d->pos.x;
            z3 = d->pos.z;
            q2 <<= 11;
            __Actor_TravelTo(d, x3, q2, z3);

            __SetFlag(0x368);
            __Func_8010704(0xf, 0xc, 1, 1, 0xd, tz);
            __Func_8010704(1, 0x19, 1, 1, tx, tz);
        }
    }
}
