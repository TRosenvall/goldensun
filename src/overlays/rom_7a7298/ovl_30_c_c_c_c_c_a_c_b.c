/* Cluster OvlFunc_921_200974c..OvlFunc_921_200974c extracted from
 * goldensun/asm/overlays/rom_7a7298/ovl_30_c_c_c_c_c_a_c.s.
 *
 * Total .text for this TU = 66 bytes (= 0x42).
 * Placed in the run in goldensun/overlays/rom_7a7298/overlay.ld.
 *
 * One frame of a rising particle: horizontal position takes the angle word at
 * +0x64 scaled by 256, vertical takes a fixed 0x8000, two more words advance by
 * 0x7ae, the angle steps by 2, and a lifetime word at +0x68 counts down to
 * deletion.
 *
 * THE ANGLE IS READ BOTH WAYS FROM ONE `short` MEMBER. `ldrsh` where it is
 * scaled into a position -- the sign matters there -- and `ldrh` where it is
 * stepped by 2 and stored straight back, because the sign cannot affect an add
 * that is truncated to sixteen bits. Declaring it `short` gives both; declaring
 * it `unsigned short` to match the `ldrh` breaks the `ldrsh`. Same trap as the
 * ActorAttrOp family in batch 75.
 *
 * THE LIFETIME IS AN `int`, so `if (--a->f68 == 0)` is correct here -- gcc keeps
 * the decremented value in the register and tests it directly. The int-width
 * dance that OvlFunc_911_20080cc and OvlFunc_923_2009c20 needed applies only
 * when the counter is a halfword and the store truncates it.
 */

struct A {
    unsigned char pad00[8];
    int f8;
    int fc;
    unsigned char pad10[8];
    int f18;
    int f1c;
    unsigned char pad20[0x44];
    short f64;
    unsigned char pad66[2];
    int f68;
};

extern void __DeleteActor(struct A *a);

void OvlFunc_921_200974c(struct A *a)
{
    a->f8 += a->f64 << 8;
    a->fc += 0x80 << 8;
    a->f18 += 0x7ae;
    a->f1c += 0x7ae;
    a->f64 += 2;
    if (--a->f68 == 0)
        __DeleteActor(a);
}
