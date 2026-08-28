/* Cluster OvlFunc_921_2009794..OvlFunc_921_2009794 extracted from goldensun/asm/overlays/rom_7a7298/ovl_30_c_c_c_c_c_a_c_c.s.
 *
 * Total .text for this TU = 304 bytes (= 0x0130).
 * Slotted before asm/overlays/rom_7a7298/ovl_30_c_c_c_c_c_a_c_c_c.o in
 * goldensun/overlays/rom_7a7298/overlay.ld.
 *
 * Spawns up to five props, each on its own phase of a 0x3c-frame cycle.
 *
 * REQUIRED A LINKER ALIAS, and this is the case where adding one pays.  The
 * five `%` operations emit `bl __umodsi3`; the ROM calls `_umodsi3_RAM`.
 * Three sibling overlays already carry `__umodsi3 = _umodsi3_RAM;` and
 * rom_7a7298 did not, so it was added -- with the five alias lines being the
 * ONLY remaining difference, which is the test the earlier deferral on
 * OvlFunc_932_200abe0 failed (there the alias would have left three real lines
 * behind and so did not pay for itself).
 *
 * Checked before adding it: no elevated C in this overlay uses `%`, and the
 * three asm files that mention umodsi3 name the ROM symbol directly, so the
 * alias on the gcc-emitted name reaches nothing else.  The byte-neutral compare
 * was then run WITH the alias and the function still in assembly.
 *
 * b1..d5 -- every shifted and pooled CreateActor argument -- are named before
 * the first `if`, which places the ROM's `mov r0, #0xde` ahead of the shifts at
 * all five sites.  Inline, those five sites plus the alias are the whole diff,
 * 25 of 128.
 *
 * The modulus operands are UNSIGNED, read off the helper name per the
 * signedness-oracle note in docs/elevation.md.
 */
struct Actor {
    unsigned char pad0[0x68];
    int f68;
    void *f6c;
};

extern unsigned int iwram_3001e40;

extern struct Actor *__CreateActor(int a, int b, int c, int d);
extern void __Actor_SetAnim(struct Actor *a, int n);
extern void OvlFunc_921_2009704(void);
extern void OvlFunc_921_200974c(void);

void OvlFunc_921_2009794(void)
{
    struct Actor *a;
    int b1, d1, b2, c2, d2, b3, d3, b4, d4, b5, d5;

    b1 = 0x1cf0000;
    d1 = 0x92 << 17;
    b2 = 0xa0 << 17;
    c2 = 0x80 << 14;
    d2 = 0xb2 << 17;
    b3 = 0xec << 15;
    d3 = 0x8c << 15;
    b4 = 0xab << 17;
    d4 = 0xf8 << 15;
    b5 = 0x1af0000;
    d5 = 0xab << 16;
    if (iwram_3001e40 % 0x3c == 0) {
        a = __CreateActor(0xde, b1, 0, d1);
        if (a != 0) {
            OvlFunc_921_2009704();
            a->f68 = 0x3c;
            a->f6c = (void *)OvlFunc_921_200974c;
            __Actor_SetAnim(a, 5);
        }
    }
    if ((iwram_3001e40 + 0x1e) % 0x3c == 0) {
        a = __CreateActor(0xde, b2, c2, d2);
        if (a != 0) {
            OvlFunc_921_2009704();
            a->f68 = 0x3c;
            a->f6c = (void *)OvlFunc_921_200974c;
            __Actor_SetAnim(a, 5);
        }
    }
    if ((iwram_3001e40 + 0xa) % 0x3c == 0) {
        a = __CreateActor(0xde, b3, 0, d3);
        if (a != 0) {
            OvlFunc_921_2009704();
            a->f68 = 0x3c;
            a->f6c = (void *)OvlFunc_921_200974c;
            __Actor_SetAnim(a, 5);
        }
    }
    if ((iwram_3001e40 + 0x32) % 0x3c == 0) {
        a = __CreateActor(0xde, b4, 0, d4);
        if (a != 0) {
            OvlFunc_921_2009704();
            a->f68 = 0x3c;
            a->f6c = (void *)OvlFunc_921_200974c;
            __Actor_SetAnim(a, 5);
        }
    }
    if ((iwram_3001e40 + 0x50) % 0x3c == 0) {
        a = __CreateActor(0xde, b5, 0, d5);
        if (a != 0) {
            OvlFunc_921_2009704();
            a->f68 = 0x3c;
            a->f6c = (void *)OvlFunc_921_200974c;
            __Actor_SetAnim(a, 5);
        }
    }
}
