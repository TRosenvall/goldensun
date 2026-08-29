/* OvlFunc_956_2008274  --  0x02008274
 *
 * Cut out of goldensun/asm/overlays/rom_7e0928/ovl_30_a_c_c_a_c_c_c.s.
 *
 * Sends two villagers walking off in different directions and marks the scene
 * done with save bit 0x362.
 *
 * THE TWO POOLED SPEEDS ARE LITERALS, NOT LOCALS -- measured, and it is the
 * fourth time this has come up. The ROM loads 0x6666 into r6 and 0xcccc into r5
 * once and re-stores them into both actors, pushing both registers, which reads
 * as two named locals surviving a call. Written that way the two registers come
 * out EXCHANGED (6 differing of 51) and no ordering of the declarations or the
 * assignments fixes it. Written as plain literals at both sites it matches, and
 * gcc discovers the shared registers itself.
 *
 * This is batch 94's standing non-signal again: a value in a callee-saved
 * register is not evidence the source named it. The refinement batch 95 added
 * -- that a value which must survive a CALL is evidence -- does not rescue the
 * naming here, so treat that as necessary rather than sufficient.
 */
struct A {
    unsigned char pad00[8];
    int f8;
    unsigned char pad0c[4];
    int f10;
    unsigned char pad14[4];
    int f18;
    int f1c;
    unsigned char pad20[0x30 - 0x20];
    int f30;
    int f34;
};

extern struct A *__MapActor_GetActor(int slot);
extern void __Actor_TravelTo(struct A *a, int x, int y, int z);
extern void __SetFlag(int id);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

void OvlFunc_956_2008274(void)
{
    struct A *a;
    int e;

    a = __MapActor_GetActor(9);
    a->f18 = 0x80 << 9;
    a->f1c = 0x80 << 9;
    a = __MapActor_GetActor(0xb);
    a->f34 = 0x6666;
    a->f30 = 0xcccc;
    __Actor_TravelTo(a, a->f8, 0x80 << 14, a->f10);
    a = __MapActor_GetActor(0xa);
    a->f34 = 0x6666;
    a->f30 = 0xcccc;
    __Actor_TravelTo(a, a->f8, 0x80 << 11, a->f10);
    __SetFlag(0x362);
    e = 0xc;
    __Func_8010704(0xf, 0xc, 1, 1, 0xd, e);
    __Func_8010704(0xe, 0xc, 1, 1, 9, e);
}
