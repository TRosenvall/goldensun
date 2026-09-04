/* OvlFunc_888_200a750 -- 0x0200a750,
 * asm/overlays/rom_7892c8/ovl_30_c_c_a_a_a_c_c_a_c_a.s
 *
 * Spawns actor 0x11d at the queried actor's position raised by 0x2d0000,
 * attaches a script, seeds a few fields including the caller's second argument
 * at +0x66, points +0x6c at OvlFunc_888_200a6f0 and +0x68 back at the queried
 * actor, and finally copies bits 2-3 of the parent's sub-object flag byte into
 * the child's.
 *
 * 12 of 55 by the screen, but the honest figure is about SIX: the residue
 * includes a `b L0` against our `b L1 / L1:`, which is the recorded
 * jump-over-a-join false negative and not a real disagreement. (It is not the
 * WHOLE residue, so the usual assemble-and-cmp shortcut does not apply -- the
 * function still does not match.)
 *
 * BLOCKER, and it is two small things that resist together:
 *
 * 1. THE MASK IS BUILT AT INT WIDTH IN THE ROM AND AT BYTE WIDTH BY US. The
 *    ROM spends TWO instructions -- `mov r3, #0xd / neg r3, r3` -- to make -13,
 *    where one instruction, `mov r3, #0xf3`, expresses the same mask for a byte
 *    result. gcc takes the cheap one because it knows the value is stored back
 *    to an unsigned char, so it narrows ~0xc before selecting the constant.
 *    Nothing tried keeps it at int width.
 *
 * 2. THE OR ACCUMULATES INTO THE OTHER OPERAND. The ROM ends with `orr r3, r2`,
 *    where r3 holds `s->f9 & ~0xc` and r2 holds `parent & 0xc`; we produce
 *    `orr r2, r3`. Which operand becomes the destination is the two-address
 *    question, and here it does not follow the source order of the `|`.
 *
 * TRIED -- SEVEN spellings, five of them tying at EXACTLY 12:
 *   a  `(s->f9 & ~0xc) | (parent & 0xc)`                              14
 *   b  operands flipped: `(parent & 0xc) | (s->f9 & ~0xc)`            12
 *   c  parent part hoisted to an int local, or-ed on the left         12
 *   f  parent part hoisted, or-ed on the RIGHT of the masked field    12
 *   g  the mask written as the literal -13 rather than ~0xc           12
 *   h  the mask written as ~0xcu                                      12
 *   j  the field value ALSO hoisted to an int local                   23
 *   k  both halves hoisted to int locals                              24
 * Writing -13 or ~0xcu directly (g, h) does NOT stop the narrowing, which is
 * the useful negative: the width is chosen from the STORE's type, not from how
 * the constant is spelled. And hoisting the masked value to an int local (j, k)
 * is much worse, so this is not the recorded value-in-destination case either.
 *
 * The remaining third of the residue is an argument-setup ordering difference:
 * the ROM emits `ldr r0, =0x11d` LAST of the four CreateActor arguments, after
 * the two coordinate loads and the +0x2d0000 add, where we emit it first. That
 * did not move with any of the above and was not attacked separately.
 *
 * WHAT WAS WON: flipping the `|` operands is worth two instructions (14 -> 12)
 * and gets the ROM's computation ORDER -- the parent's masked bits are computed
 * before the field's -- even though it does not fix which register accumulates.
 * The struct layout, the 0xb4 << 14 offset, the pooled zero for +0x26 and the
 * field types are all reproduced; the first 12 instructions and instructions
 * 15-38 are exact.
 */

struct Sub {
    unsigned char pad00[9];
    unsigned char f9;
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
        b = __CreateActor(0x11d, a->f8, a->fc + (0xb4 << 14), a->f10);
        if (b != 0) {
            s = b->f50;
            __Actor_SetScript(b, &gScript_888__0200c15c);
            b->f55 = 0;
            b->f64 = 0;
            b->f66 = n;
            b->f6c = OvlFunc_888_200a6f0;
            s->f26 = 0;
            s->f9 = (a->f50->f9 & 0xc) | (s->f9 & ~0xc);
            b->f68 = a;
        }
    }
}
