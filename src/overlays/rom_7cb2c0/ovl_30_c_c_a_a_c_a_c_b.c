/* OvlFunc_945_2008e14  --  0x02008e14
 * OvlFunc_945_2008ee0  --  0x02008ee0
 * OvlFunc_945_2008fac  --  0x02008fac
 * OvlFunc_945_2009078  --  0x02009078
 *
 * Cut from goldensun/asm/overlays/rom_7cb2c0/ovl_30_c_c_a_a_c_a_c.s.
 *
 * The shop-visit script: with the party gathered, walk the shopkeeper to the
 * player and speak; otherwise pick one of four lines by save bit. Four of the five members.
 *
 * FOUND BY SHAPE, as a FIVE-member cluster with nothing solved in it.
 * `tools/find_shape.py --clusters` groups every remaining function by its
 * instruction stream with constants, labels and call targets wildcarded; this
 * came back as five functions differing in seven ids each. Solving one and
 * filling the captured values into the template gave the rest, all clean on the
 * first screen.
 *
 * THE FOUR TAIL ARMS ARE WRITTEN OUT IN FULL. The ROM cross-jumps three of them
 * into a shared `mov r0, #n / bl ...9804` and keeps the fourth separate, which
 * is what gcc produces from four independent `else if` arms each ending in
 * their own call. Writing three arms and a `goto` would be transcribing the
 * optimiser.
 *
 * The actor's tile coordinates at +0xa and +0x12 are read as SIGNED halfwords,
 * and the null check on the actor guards only the TravelTo -- the wait and the
 * reposition happen either way.
 */
struct A { unsigned char pad00[0xa]; short fa; unsigned char pad0c[6]; short f12; };

extern int __GetFlag(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __MessageID(int id);
extern struct A *__MapActor_GetActor(int slot);
extern void __MapActor_SetAnim(int slot, int a);
extern void __MapActor_TravelTo(int slot, int x, int z);
extern void __MapActor_WaitMovement(int slot);
extern void __MapActor_SetPos(int slot, int x, int z);
extern int OvlFunc_945_20092dc(void);
extern void OvlFunc_945_2009190(int slot);
extern void OvlFunc_945_200c86c(int n);
extern void OvlFunc_945_2009804(int a, int b, int c);

void OvlFunc_945_2008e14(void)
{
    int s;
    struct A *a;

    if (__GetFlag(0xc0 << 2)) {
        s = OvlFunc_945_20092dc();
        __CutsceneStart();
        OvlFunc_945_2009190(s);
        __MessageID(0x1ea3);
        OvlFunc_945_200c86c(0xd);
        __MapActor_SetAnim(s, 2);
        a = __MapActor_GetActor(0);
        if (a != 0)
            __MapActor_TravelTo(s, a->fa, a->f12);
        __MapActor_WaitMovement(s);
        __MapActor_SetPos(s, 0, 0);
        __CutsceneEnd();
    } else if (__GetFlag(0x92b)) {
        OvlFunc_945_2009804(0xd, 0x1e88, 0x995);
    } else if (__GetFlag(0x92a)) {
        OvlFunc_945_2009804(0xd, 0x1e88, 0x91c);
    } else if (__GetFlag(0x929)) {
        OvlFunc_945_2009804(0xd, 0x1e88, 0x93a);
    } else {
        OvlFunc_945_2009804(0xd, 0x1e88, 0x931);
    }
}
void OvlFunc_945_2008ee0(void)
{
    int s;
    struct A *a;

    if (__GetFlag(0xc0 << 2)) {
        s = OvlFunc_945_20092dc();
        __CutsceneStart();
        OvlFunc_945_2009190(s);
        __MessageID(0x1ea4);
        OvlFunc_945_200c86c(0xe);
        __MapActor_SetAnim(s, 2);
        a = __MapActor_GetActor(0);
        if (a != 0)
            __MapActor_TravelTo(s, a->fa, a->f12);
        __MapActor_WaitMovement(s);
        __MapActor_SetPos(s, 0, 0);
        __CutsceneEnd();
    } else if (__GetFlag(0x92b)) {
        OvlFunc_945_2009804(0xe, 0x1e8b, 0x996);
    } else if (__GetFlag(0x92a)) {
        OvlFunc_945_2009804(0xe, 0x1e8b, 0x91d);
    } else if (__GetFlag(0x929)) {
        OvlFunc_945_2009804(0xe, 0x1e8b, 0x93b);
    } else {
        OvlFunc_945_2009804(0xe, 0x1e8b, 0x932);
    }
}
void OvlFunc_945_2008fac(void)
{
    int s;
    struct A *a;

    if (__GetFlag(0xc0 << 2)) {
        s = OvlFunc_945_20092dc();
        __CutsceneStart();
        OvlFunc_945_2009190(s);
        __MessageID(0x1ea5);
        OvlFunc_945_200c86c(0xf);
        __MapActor_SetAnim(s, 2);
        a = __MapActor_GetActor(0);
        if (a != 0)
            __MapActor_TravelTo(s, a->fa, a->f12);
        __MapActor_WaitMovement(s);
        __MapActor_SetPos(s, 0, 0);
        __CutsceneEnd();
    } else if (__GetFlag(0x92b)) {
        OvlFunc_945_2009804(0xf, 0x1e8e, 0x997);
    } else if (__GetFlag(0x92a)) {
        OvlFunc_945_2009804(0xf, 0x1e8e, 0x91e);
    } else if (__GetFlag(0x929)) {
        OvlFunc_945_2009804(0xf, 0x1e8e, 0x93c);
    } else {
        OvlFunc_945_2009804(0xf, 0x1e8e, 0x933);
    }
}
void OvlFunc_945_2009078(void)
{
    int s;
    struct A *a;

    if (__GetFlag(0xc0 << 2)) {
        s = OvlFunc_945_20092dc();
        __CutsceneStart();
        OvlFunc_945_2009190(s);
        __MessageID(0x1ea6);
        OvlFunc_945_200c86c(0x10);
        __MapActor_SetAnim(s, 2);
        a = __MapActor_GetActor(0);
        if (a != 0)
            __MapActor_TravelTo(s, a->fa, a->f12);
        __MapActor_WaitMovement(s);
        __MapActor_SetPos(s, 0, 0);
        __CutsceneEnd();
    } else if (__GetFlag(0x92b)) {
        OvlFunc_945_2009804(0x10, 0x1e91, 0x998);
    } else if (__GetFlag(0x92a)) {
        OvlFunc_945_2009804(0x10, 0x1e91, 0x91f);
    } else if (__GetFlag(0x929)) {
        OvlFunc_945_2009804(0x10, 0x1e91, 0x93d);
    } else {
        OvlFunc_945_2009804(0x10, 0x1e91, 0x934);
    }
}
