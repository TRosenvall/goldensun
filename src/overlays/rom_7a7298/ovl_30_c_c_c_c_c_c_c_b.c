/* OvlFunc_921_2009f24  --  0x02009f24, cut from goldensun/asm/overlays/rom_7a7298/ovl_30_c_c_c_c_c_c_c.s.
 *
 * Preserves the original ROM layout when slotted between
 * asm/overlays/rom_7a7298/ovl_30_c_c_c_c_c_c_c_a.o and asm/overlays/rom_7a7298/ovl_30_c_c_c_c_c_c_c_c.o in
 * goldensun/overlays/rom_7a7298/overlay.ld.
 *
 * The player-facing turn step. While the counter at +0x64 is non-zero it just
 * ticks down; at zero it reads the d-pad, looks the four direction bits up in a
 * sixteen-entry table of target angles, and either plays the idle animation
 * (table entry -1, no direction held) or slews the facing angle toward the
 * target by at most 0x1000 a frame and plays the walk animation.
 *
 * One of TWO byte-identical copies -- OvlFunc_921_2009f24 and
 * OvlFunc_922_200a014, one per overlay. Found with tools/find_twins.py.
 *
 * THE ONLY WORK WAS NAMING THE TABLE. It sits in this .s as `.L23f0`, a
 * file-local label, and a C file cannot reference one -- local labels do not
 * cross objects and the name is not an identifier. It is renamed to
 * `gTable_921__0200a3f0` and exported, which is what the neighbouring script blobs
 * in the same file already do. **A label emits no bytes**, so renaming one and
 * giving it global binding changes symbol-table metadata and nothing else; the
 * link is byte-identical.
 *
 * The rest fell out on the first screen, at 60 lines against 60 with the only
 * differences being that name and the label numbering behind it:
 *
 *   - the counter at +0x64 is `short`, read SIGNED for `!= 0` and UNSIGNED for
 *     the decrement, which is the ldrsh/ldrh pair the ROM has. Fifth function
 *     on that rule;
 *   - the slew is a `short`, so the clamp compares after `lsl #16 / asr #16`;
 *   - `a->f5a = 0` compiles to `strb r1, ...` reusing the register that already
 *     holds the tested zero -- gcc proves it on that arm, so no separate
 *     `mov #0` appears and none should be written.
 */

struct A {
    unsigned char pad00[6];
    unsigned short f6;
    unsigned char pad08[0x52];
    unsigned char f5a;
    unsigned char pad5b[9];
    short f64;
};

extern unsigned int gKeyHeld;
extern short gTable_921__0200a3f0[];
extern void __Actor_SetAnim(struct A *a, int id);
extern void __Actor_SetAnimSpeed(struct A *a, int s);

void OvlFunc_921_2009f24(struct A *a)
{
    short d;
    int t;

    if (a->f64 != 0) {
        a->f64--;
        return;
    }
    a->f5a = 0;
    t = gTable_921__0200a3f0[(gKeyHeld >> 4) & 0xf];
    if (t == -1) {
        __Actor_SetAnim(a, 9);
        return;
    }
    d = t - a->f6;
    if (d > (0x80 << 5))
        d = 0x80 << 5;
    if (d < -0x1000)
        d = -0x1000;
    a->f6 = a->f6 + d;
    __Actor_SetAnim(a, 2);
    __Actor_SetAnimSpeed(a, 0x30);
}
