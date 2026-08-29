/* OvlFunc_965_2008a4c, the whole of goldensun/asm/overlays/rom_7ef4f4/ovl_30_a_a_a_c_c_c_c_c_a.s.
 *
 * Total .text for this TU = 96 bytes (= 0x60). The .s is replaced outright, so
 * no linker-script change was needed.
 *
 * Spawns an actor, sets its sprite's two-bit selector to 1, initialises two
 * bytes at +0x55 and +0x59, runs the two setup calls, and finally sets bit 1
 * of the flag byte at +0x23 while clearing bit 0. Returns the actor, or 0 if
 * __CreateActor refused.
 *
 * FOUR BYTE-IDENTICAL COPIES exist, one per area overlay; they are listed in
 * reports/batch-71.md and share this C verbatim.
 *
 * THE ARGUMENT ROTATION IS FREE. __CreateActor is called with this function's
 * arguments rotated -- (d, a, b, c) -- and gcc reproduces the ROM's seven-move
 * shuffle through r4/r5/r6 exactly, with no help. Worth knowing before treating
 * an argument permutation as the argument-precompute blocker; that class is
 * about cheap constants mixed with expensive values, not about permutation.
 *
 * THE TWO MASKED WRITES NEED OPPOSITE SPELLINGS, and that is the whole content
 * of this file.
 *
 *   Sprite +9, a two-bit field:  the ROM builds the mask at 32-bit width,
 *     `mov r3, #0xd / neg r3, r3`. That is the bitfield form -- gcc's
 *     store_bit_field works in int width.
 *
 *   Actor +0x23, one bit cleared and one set:  the ROM builds the mask at BYTE
 *     width, `mov r3, #0xfe`. Declared as a bitfield this comes out
 *     `mov r3, #2 / neg r3, r3` instead, one instruction too many. It has to be
 *     hand-written masking on a plain `unsigned char`.
 *
 * So "use a bitfield" is not a blanket rule: the ROM's mask WIDTH says which
 * spelling the original used, and this one function uses both. `mov rN, #K /
 * neg` means a bitfield; a bare `mov rN, #0xNN` byte mask means hand-written.
 *
 * The `| 2` rather than `& ~2 | 2` is gcc simplifying a one-bit set, not
 * evidence either way.
 */

struct Sprite {
    unsigned char pad[9];
    unsigned char lo : 2;
    unsigned char sel : 2;
    unsigned char hi : 4;
};

struct Actor {
    unsigned char pad00[0x23];
    unsigned char f23;
    unsigned char pad24[0x2c];
    struct Sprite *spr;
    unsigned char pad54;
    unsigned char f55;
    unsigned char pad56[3];
    unsigned char f59;
};

extern struct Actor *__CreateActor(int a, int b, int c, int d);
extern void __Actor_SetSpriteFlags(struct Actor *a, int f);
extern void __Func_80929d8(struct Actor *a, int n);

void *OvlFunc_965_2008a4c(int a, int b, int c, int d)
{
    struct Actor *act;

    act = __CreateActor(d, a, b, c);
    if (act != 0) {
        act->spr->sel = 1;
        act->f55 = 0;
        act->f59 = 8;
        __Actor_SetSpriteFlags(act, 0);
        __Func_80929d8(act, 0xf);
        act->f23 = (0xfe & act->f23) | 2;
        return act;
    }
    return 0;
}
