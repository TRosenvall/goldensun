/* OvlFunc_956_20082f8 -- 0x020082f8, the WHOLE of
 * asm/overlays/rom_7e0928/ovl_30_a_c_c_a_c_c_c_c_a.s (one .thumb_func_start,
 * one .func_end, 108 instructions; no .section/.data/.bss/.lcomm/.word/.byte
 * anywhere, and the only two `.L` symbols -- .L332 and .L3e0 -- are branch
 * targets DEFINED IN THIS FILE).  tools/asmfacts.py says WHOLE / convert
 * directly, so a whole-file .c replaces it with NO linker edit and NO split.
 *
 * Total .text for this TU = 268 bytes.  The one line naming the object, in
 * overlays/rom_7e0928/overlay.ld:27, is
 *
 *     asm/overlays/rom_7e0928/ovl_30_a_c_c_a_c_c_c_c_a.o(.text)
 *
 * and it stays VERBATIM: `asm/%.o: src/%.c` builds that object from this file.
 *
 * NO FLAG GROUP.  tryc.makefile_flags() on this path is the EMPTY set and no
 * Makefile rule -- explicit or wildcard -- names rom_7e0928, so the generic
 * `asm/%.o: src/%.c` rule fires with the tree default -O2 -mthumb
 * -mthumb-interwork -fcall-used-r4.  NO PINS, so no fakematch entry.
 *
 * OK OvlFunc_956_20082f8 -- 268 bytes, 113 encodings and 17 relocations
 * identical.  Re-measured four times.
 *
 * What it does: if save bit 0x362 is clear, walk actor gState+0x1f4 to actor
 * 0xa's tile, then hand three actors (0xb, 0xa, and gState+0x1f4's own) the
 * same speed/accel pair and send them travelling; repaint a rect, wait two
 * frames, and set save bit 0x367.
 *
 * TEMPLATE.  The two direct siblings in this directory handed the whole thing
 * over and NOTHING had to be re-derived from scratch:
 *
 *   src/overlays/rom_7e0928/ovl_30_a_c_c_a_c_c_c_c_b.c (OvlFunc_956_2008404)
 *     -- the `g = gState; *(int *)(g + 0x1f4)` spelling, the f55/accel/speed
 *        store ORDER, and the `int e1, f1;` pair for __Func_8010704's two
 *        stack arguments.  That function is this one's near-twin: same struct,
 *        same three-store block, and __Func_8010704(0, 0x19, 1, 1, 9, 0xc)
 *        against this one's (0, 0x18, 1, 1, 9, 0xc).
 *   src/overlays/rom_7e0928/ovl_30_a_c_c_a_c_c_c_c_c.c (OvlFunc_956_20084a4)
 *     -- lever 1 (one pointer variable PER FETCH) and lever 3 (`q2 = 0x80;
 *        ...; q2 <<= N;`), both of which reproduce here unchanged.
 *
 * The FIRST plain transcription was 104 of 107 with the function THREE
 * INSTRUCTIONS SHORT, and the entire defect was the gState offset folding to a
 * single `ldr r3, =gState+500` pool load.  Assigning gState to a local first --
 * the sibling's spelling -- restored `mov r2,#0xfa / lsl r2,#1 / add r3,r2` and
 * took it straight to 5 of 110 at the exact size.  Those five were
 * __Func_8010704's `mov r3 / str / mov r3 / str` against the ROM's
 * two-register `mov r3,#9 / mov r2,#0xc / str / str`, and the sibling's named
 * `e1/f1` pair closed them.  Nothing else was needed.
 *
 * MEASURED WORSE / INERT (against 110 lines / 113 encodings), one lever
 * changed at a time from this file:
 *
 *   spelling                                           lines  differing
 *   -------------------------------------------------  -----  ---------
 *   `*(int *)(gState + 0x1f4)` inline, no local `g`      107        104
 *   `slot` not named -- the load repeated at each use    113        104
 *   ONE pointer variable for all four GetActor fetches   113         98
 *   plain literals (0x200000 / 0x40000) for `q2`         113        111  (r11)
 *   __Func_8010704's 9 and 0xc as bare literals          110          5
 *   (this file)                                         110          0
 *   INERT (tie at 0, so the simpler form ships):
 *     x1/z1..x3/z3 named for the two pos loads per call site -- the sibling
 *       _c.c needs them, this file does not; the calls are inline here
 *     `f = __GetFlag(0x362)` named and the three `= 0` stores written `= f`
 *
 * CONFIRMED, and worth the line: THE ROM'S r7 IS NOT IN THE SOURCE.  The ROM
 * emits `mov r7,r0 / cmp r7,#0 / bne` and then `strb r7, [r3]` at all three
 * `interactFlag = 0` stores.  That looks like the source storing the flag
 * variable, and it is not: gcc's cse records the jump equivalence (in the
 * fall-through of `bne`, that register IS 0) and substitutes it for the
 * constant.  Writing `= 0` and DROPPING the flag variable entirely is
 * byte-identical, and it is the honest spelling.  Same mechanism fires in
 * OvlFunc_907_2008ae0 in overlays/rom_79b154 -- two `= 0` stores there reuse
 * two different known-zero flag registers.
 *
 * NEW: the `q2 = 0x80; q2 <<= N;` rebuild lever survives EQUAL SHIFT COUNTS.
 * The sibling _c.c records it for three sites with shifts 11, 14, 11 and
 * argues from `q2` being dead at each call.  Here the second and third sites
 * are BOTH `<< 11` -- the identical value, recomputed with no branch between
 * them -- and gcc still rebuilds rather than commoning.  So the lever is about
 * the mov and the shift being two separate insns, not about the values being
 * distinct; a plain literal at all three sites is 111 of 113 and reaches for
 * r11.
 */
#include "actor.h"

extern unsigned char gState[];
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern struct Actor *__MapActor_GetActor(int slot);
extern void __MapActor_TravelTo(int slot, int x, int z);
extern void __MapActor_WaitMovement(int slot);
extern void __Actor_TravelTo(struct Actor *a, int x, int y, int z);
extern void __Actor_SetSpriteFlags(struct Actor *a, int n);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __WaitFrames(int n);

void OvlFunc_956_20082f8(void)
{
    unsigned char *g;
    struct Actor *a;
    struct Actor *b;
    struct Actor *c;
    struct Actor *d;
    int slot;
    int q2;
    int e1, f1;

    g = gState;
    slot = *(int *)(g + 0x1f4);
    if (__GetFlag(0x362) != 0)
        return;
    a = __MapActor_GetActor(0xa);
    if (a != 0)
        __MapActor_TravelTo(slot, *(short *)((char *)a + 0xa),
                            *(short *)((char *)a + 0x12));
    __MapActor_WaitMovement(slot);

    b = __MapActor_GetActor(0xb);
    b->interactFlag = 0;
    b->accel = 0x6666;
    b->speed = 0xcccc;
    q2 = 0x80;
    q2 <<= 14;
    __Actor_TravelTo(b, b->pos.x, q2, b->pos.z);

    c = __MapActor_GetActor(0xa);
    c->interactFlag = 0;
    c->accel = 0x6666;
    c->speed = 0xcccc;
    q2 = 0x80;
    q2 <<= 11;
    __Actor_TravelTo(c, c->pos.x, q2, c->pos.z);

    d = __MapActor_GetActor(slot);
    d->interactFlag = 0;
    d->accel = 0x6666;
    d->speed = 0xcccc;
    q2 = 0x80;
    q2 <<= 11;
    __Actor_TravelTo(d, d->pos.x, q2, d->pos.z);

    __Actor_SetSpriteFlags(d, 1);
    __MapActor_WaitMovement(slot);
    e1 = 9;
    f1 = 0xc;
    __Func_8010704(0, 0x18, 1, 1, e1, f1);
    __WaitFrames(2);
    __Actor_SetSpriteFlags(d, 1);
    d->interactFlag = 3;
    d->floorPos = d->pos.y;
    __SetFlag(0x367);
}
