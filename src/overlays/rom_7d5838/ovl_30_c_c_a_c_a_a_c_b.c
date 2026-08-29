/* OvlFunc_950_200866c  --  0x0200866c
 *
 * The last function of goldensun/asm/overlays/rom_7d5838/ovl_30_c_c_a_c_a_a_c.s;
 * the other six stay as assembly in ovl_30_c_c_a_c_a_a_c_a.s.
 *
 * A third sanctum attendant in the same overlay as
 * src/overlays/rom_7d5838/ovl_30_c_c_c_c_a.c, and structurally the same
 * function: quadrant test, then one of three lines chosen by two save bits.
 * Only the counter it opens and the three ids differ.
 *
 * The facing test is the QUADRANT spelling; ovl_30_c_c_c_c_a.c carries the
 * reasoning for `~0x3fff` and for the `unsigned short` result. Matched on the
 * first screen.
 */
struct A { unsigned char pad00[6]; unsigned short f6; };

extern struct A *__MapActor_GetActor(int slot);
extern void __MessageID(int id);
extern int __GetFlag(int id);
extern void __ActorMessage(int slot, int arg);
extern void __Func_80b0278(int a, int b);

void OvlFunc_950_200866c(int slot)
{
    struct A *a;
    unsigned short d;

    a = __MapActor_GetActor(0);
    d = (a->f6 + 0x2000) & ~0x3fff;
    if (d == 0xc000) {
        __Func_80b0278(0x1b, slot);
    } else if (__GetFlag(0x95 << 4)) {
        __MessageID(0x238f);
        __ActorMessage(slot, 0);
    } else if (__GetFlag(0x962)) {
        __MessageID(0x221d);
        __ActorMessage(slot, 0);
    } else {
        __MessageID(0x1fd9);
        __ActorMessage(slot, 0);
    }
}
