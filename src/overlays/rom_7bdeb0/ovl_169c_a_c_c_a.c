/* Cluster OvlFunc_934_2009938..OvlFunc_934_2009938 extracted from goldensun/asm/overlays/rom_7bdeb0/ovl_169c_a_c_c.s.
 *
 * Total .text for this TU = 74 bytes (= 0x4a).
 * Placed in the run in goldensun/overlays/rom_7bdeb0/overlay.ld.
 *
 * Byte-identical to OvlFunc_922_2009004 in overlays/rom_7a8c8c; this C is shared verbatim, with only the symbol changed.
 *
 * Places an actor: fetches it by slot, sets its state byte at +0x22 to 2 and a
 * flag bit at +0x23, and writes two 12.20 coordinates offset by a fixed
 * 0x80000.
 *
 * Matched on the first screen with no levers needed, which is worth recording
 * in itself: the three-argument save into r6/r7/r8, the two-call structure and
 * the walked pointer (`add r1, #0x22` then `add r1, #1` rather than two indexed
 * stores) all fall out of ordinary struct members and ordinary statements.
 */

struct A {
    unsigned char pad00[8];
    int f8;
    unsigned char pad0c[4];
    int f10;
    unsigned char pad14[0xe];
    unsigned char f22;
    unsigned char f23;
};

extern struct A *__MapActor_GetActor(int slot);
extern void __Func_8092b08(int a, int b);

void OvlFunc_934_2009938(int slot, int x, int y)
{
    struct A *a;
    int k;

    a = __MapActor_GetActor(slot);
    if (a == 0)
        return;
    __Func_8092b08(slot, 3);
    a->f22 = 2;
    a->f23 |= 2;
    k = 0x80 << 12;
    a->f8 = (x << 20) + k;
    a->f10 = (y << 20) + k;
}
