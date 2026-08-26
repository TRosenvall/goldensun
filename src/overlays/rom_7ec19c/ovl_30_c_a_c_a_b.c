/* OvlFunc_962_2008100  --  0x02008100
 * OvlFunc_962_200816c  --  0x0200816c
 *
 * The back two thirds of goldensun/asm/overlays/rom_7ec19c/ovl_30_c_a_c_a.s.
 * The first function in that file, OvlFunc_962_200806c, stays as assembly in
 * ovl_30_c_a_c_a_a.s -- see src/non_matching/overlays/200806c.c for why.
 *
 * Two sanctum attendants. Stand inside the facing arc and the counter opens;
 * stand outside it and the attendant speaks, one line before save bit 0x96f is
 * set and another after.
 *
 * The facing test is the QUADRANT spelling settled in
 * src/overlays/rom_7d5838/ovl_30_c_c_c_c_a.c, which is where its two readings
 * are written out: the mask is `~0x3fff` rather than `0xc000` so that gcc pools
 * the 32-bit form in one `ldr`, and the result is an `unsigned short` so that
 * the compare comes out as `lsl #16` against a pre-shifted constant. Both of
 * those are forced -- `& 0xc000` and an `int` result each diverge immediately.
 *
 * The two functions differ only in what the arc opens (a generic counter helper
 * against __UI_Sanctum) and in the two message ids. Both matched on the first
 * screen once the mask was right.
 */
struct A { unsigned char pad00[6]; unsigned short f6; };

extern struct A *__MapActor_GetActor(int slot);
extern void __MessageID(int id);
extern int __GetFlag(int id);
extern void __ActorMessage(int slot, int arg);
extern void __UI_Sanctum(int slot);
extern void __Func_80b3284(int a, int b);

void OvlFunc_962_2008100(int slot)
{
    struct A *a;
    unsigned short d;

    a = __MapActor_GetActor(0);
    d = (a->f6 + 0x2000) & ~0x3fff;
    if (d == 0xc000) {
        __Func_80b3284(0xa, slot);
    } else if (__GetFlag(0x96f)) {
        __MessageID(0x2620);
        __ActorMessage(slot, 0);
    } else {
        __MessageID(0x25d1);
        __ActorMessage(slot, 0);
    }
}

void OvlFunc_962_200816c(int slot)
{
    struct A *a;
    unsigned short d;

    a = __MapActor_GetActor(0);
    d = (a->f6 + 0x2000) & ~0x3fff;
    if (d == 0xc000) {
        __UI_Sanctum(slot);
    } else if (__GetFlag(0x96f)) {
        __MessageID(0x262c);
        __ActorMessage(slot, 0);
    } else {
        __MessageID(0x25d5);
        __ActorMessage(slot, 0);
    }
}
