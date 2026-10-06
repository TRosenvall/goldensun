/* OvlFunc_888_200a750  --  0x0200a750, MATCHING.
 *
 * Was src/non_matching/overlays/200a750.c at a positional figure of twelve
 * encodings of fifty-nine, which was PHASE: the two streams were 51 against
 * 50 sixteen-bit instructions, so the figure counted the offset.  Batch 332.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/overlays/rom_7892c8/ovl_30_c_c_a_a_a_c_c_a_c_a_a_b.c \
 *     asm/overlays/rom_7892c8/ovl_30_c_c_a_a_a_c_c_a_c_a_a_b.s --whole
 *
 * SPLIT.  The piece asm/overlays/rom_7892c8/ovl_30_c_c_a_a_a_c_c_a_c_a_a.s
 * holds three functions -- OvlFunc_888_200a6f0, this one, OvlFunc_888_200a7d4 --
 * so it splits three ways and this is the _b part:
 *   python3 tools/split_s.py \
 *     asm/overlays/rom_7892c8/ovl_30_c_c_a_a_a_c_c_a_c_a_a.s OvlFunc_888_200a750
 *   -> _a.s (53 lines), _b.s (69 lines), _c.s (141 lines);
 *      overlays/rom_7892c8/overlay.ld rewritten.
 * tools/datacheck.py on the piece: clean, no data section goes with the split.
 * PINS: 0 (tools/shimcount.py), no inline asm, no device.
 *
 * THE DIRECTION WAS "REFERENCE LONGER": we were MISSING one instruction, and
 * it was the second half of the mask.
 *
 * FOUR LEVERS, ALL NEEDED, EACH WORTH 1-3 ENCODINGS.  12 -> 9 -> 6 -> 3 -> 0.
 *
 * 1. `f9` IS A SIGNED char, NOT AN UNSIGNED ONE.  That is the whole missing
 *    instruction.  With `unsigned char f9` the C integer promotion is to a
 *    value fold knows fits in 0xff, so fold distributes the narrowing convert
 *    of the store over the AND and picks `mov r3, #0xf3` -- one instruction.
 *    With `signed char f9` it cannot, the mask stays int-wide at -13, and -13
 *    costs `mov r3, #0xd / neg r3, r3` -- two.  The ROM has two.  The load
 *    stays `ldrb` either way because only the low byte is live.
 *    This is already written down in the tree: the LANDED
 *    src/rom_c9000/rom_dbb24_a.c header says "a signed-char f9 gives the #-13
 *    mask where unsigned char gives #243", and the park's own TRIED table had
 *    varied only the SPELLING of the constant (~0xc, -13, ~0xcu) and the
 *    PLACE of the operands, never the DECLARED SIGNEDNESS OF THE FIELD.
 *    (The named-int-mask route of src/non_matching/ovl_7a8c8c/2008ed8.c and
 *    Anim_Gaia lever 4 -- `int mask = ~0xc; t = mask & s->f9;` -- buys the
 *    same two instructions here and measured six; it is the same mechanism
 *    reached from the other side and it is NOT needed once f9 is signed.)
 *
 * 2. THE OR IS WRITTEN FIELD-HALF FIRST.  `(s->f9 & ~0xc) | u` gives the ROM's
 *    `orr r3, r2`; `u | (s->f9 & ~0xc)` gives `orr r2, r3`.  Thumb's orr is
 *    two-address and gcc ties the destination to operand 1, which for a
 *    commutative op with two non-constant operands is the one written FIRST.
 *    The park had measured this pair and recorded the WRONG member as better,
 *    because at the time the mask was still narrowed and the comparison was
 *    being made on an out-of-phase stream.
 *
 * 3. `b->f68 = a;` GOES BEFORE THE FLAG STORE.  The ROM's tail is
 *    `orr r3, r2 / str r7, [r6, #0x68] / strb r3, [r5, #9]`.
 *
 * 4. THE THREE COORDINATE ARGUMENTS ARE NAMED LOCALS.  That is what puts
 *    `ldr r0, =0x11d` LAST of the four argument registers instead of first.
 *    Inline, the one argument that needs arithmetic is precomputed and the
 *    pooled constant is then emitted at its parameter index; with all three
 *    coordinates already in pseudos nothing is precomputed and the pool load
 *    sinks to the end.  The LANDED src/overlays/rom_7a4370/ovl_30_c_c_c_c_a_a_c_c.c
 *    shows the same shape from the other side -- three plain loads then
 *    `ldr r0, .L18+12 / bl __CreateActor`.
 *
 * MEASURED AND INERT (do not respend): flipping the operands inside either
 * AND (`~0xc & s->f9`, `0xc & a->f50->f9`) is byte-identical to not flipping
 * them -- fold canonicalises a literal to the right, so an AND's operand order
 * is NOT reachable from C when one side is a literal.  Hoisting only the y
 * coordinate, or only the id, or spelling 0xb4 << 14 as 0x2d0000, are all
 * byte-identical.  Hoisting the FIELD LOAD to its own local (`int f = s->f9;`)
 * costs four bytes and dirties the relocations.
 *
 * Spawns actor 0x11d at the queried actor's position raised by 0x2d0000,
 * attaches a script, seeds a few fields including the caller's second argument
 * at +0x66, points +0x6c at OvlFunc_888_200a6f0 and +0x68 back at the queried
 * actor, and finally copies bits 2-3 of the parent's sub-object flag byte into
 * the child's.
 */

struct Sub {
    unsigned char pad00[9];
    signed char f9;
    unsigned char pad0a[0x26 - 0xa];
    unsigned char f26;
};

struct Actor {
    unsigned char pad00[8];
    int f8;
    int fc;
    int f10;
    unsigned char pad14[0x50 - 0x14];
    struct Sub *f50;
    unsigned char pad54[1];
    unsigned char f55;
    unsigned char pad56[0x64 - 0x56];
    unsigned short f64;
    unsigned short f66;
    struct Actor *f68;
    void *f6c;
};

extern struct Actor *__MapActor_GetActor(int slot);
extern struct Actor *__CreateActor(int id, int x, int y, int z);
extern void __Actor_SetScript(struct Actor *a, void *script);
extern void gScript_888__0200c15c;
extern void OvlFunc_888_200a6f0(void);

void OvlFunc_888_200a750(int slot, int n)
{
    struct Actor *a;
    struct Actor *b;
    struct Sub *s;

    a = __MapActor_GetActor(slot);
    if (a != 0) {
        int x = a->f8;
        int y = a->fc + (0xb4 << 14);
        int z = a->f10;
        b = __CreateActor(0x11d, x, y, z);
        if (b != 0) {
            s = b->f50;
            __Actor_SetScript(b, &gScript_888__0200c15c);
            b->f55 = 0;
            b->f64 = 0;
            b->f66 = n;
            b->f6c = OvlFunc_888_200a6f0;
            s->f26 = 0;
            {
                int u = a->f50->f9 & 0xc;

                b->f68 = a;
                s->f9 = (s->f9 & ~0xc) | u;
            }
        }
    }
}
