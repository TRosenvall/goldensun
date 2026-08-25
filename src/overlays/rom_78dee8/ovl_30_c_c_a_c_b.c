/* Cluster OvlFunc_895_200856c..OvlFunc_895_2008754 extracted from goldensun/asm/overlays/rom_78dee8/ovl_30_c_c_a_c.s.
 *
 * Slotted between ovl_30_c_c_a_c_a.o and the rest of the overlay.
 *
 * EIGHT NEAR-TWINS IN ONE OBJECT, AND THAT IS DELIBERATE. All eight need
 * -fno-rerun-cse-after-loop and they are contiguous in the .s, so splitting
 * once puts them in a single .o under a single Makefile rule instead of eight.
 * The tree already carries fourteen of these rules; adding one instead of eight
 * matters. See CSE_CFLAGS in the Makefile and the standing item in HANDOFF.md.
 *
 * Each reads a map actor's X, shifts it down 20 to a tile coordinate, clears
 * two adjacent flags, and sets one of them back if the tile matches. Both flag
 * ids appear on both sides of a call, which is the constant-CSE shape named in
 * batch 50.
 *
 * WHAT VARIES, AND IT IS NOT UNIFORM. Six of the eight end with a call to
 * OvlFunc_895_20097c0(0); the first two do not, branching straight to the
 * epilogue. And OvlFunc_895_20085ac INVERTS the pairing: everywhere else the
 * first tile value sets the SECOND flag, and there it sets the first. Neither
 * is guessable from the family -- both were read off each listing.
 *
 * The flag ids that are a multiple of four are written as shifts (`0xc0 << 2`)
 * because the ROM builds them with mov/lsl; the others are pooled and written
 * as literals. That is just what fits an eight-bit immediate.
 */
extern void *__MapActor_GetActor(int slot);
extern void __SetFlag(int id);
extern void __ClearFlag(int id);
extern void OvlFunc_895_20097c0(int a);

void OvlFunc_895_200856c(void)
{
    unsigned char *a;
    int t;

    a = (unsigned char *)__MapActor_GetActor(9);
    if (a == 0)
        return;
    t = *(int *)(a + 8) >> 20;
    __ClearFlag(0x302);
    __ClearFlag(0x303);
    if (t == 0x5d)
        __SetFlag(0x303);
    else if (t == 0x5f)
        __SetFlag(0x302);
}

void OvlFunc_895_20085ac(void)
{
    unsigned char *a;
    int t;

    a = (unsigned char *)__MapActor_GetActor(0xa);
    if (a == 0)
        return;
    t = *(int *)(a + 8) >> 20;
    __ClearFlag(0xc0 << 2);
    __ClearFlag(0x301);
    if (t == 0x73)
        __SetFlag(0xc0 << 2);
    else if (t == 0x71)
        __SetFlag(0x301);
}

void OvlFunc_895_20085ec(void)
{
    unsigned char *a;
    int t;

    a = (unsigned char *)__MapActor_GetActor(9);
    if (a == 0)
        return;
    t = *(int *)(a + 8) >> 20;
    __ClearFlag(0xc4 << 2);
    __ClearFlag(0x311);
    if (t == 0x63)
        __SetFlag(0x311);
    else if (t == 0x65)
        __SetFlag(0xc4 << 2);
    OvlFunc_895_20097c0(0);
}

void OvlFunc_895_2008634(void)
{
    unsigned char *a;
    int t;

    a = (unsigned char *)__MapActor_GetActor(0xa);
    if (a == 0)
        return;
    t = *(int *)(a + 8) >> 20;
    __ClearFlag(0x312);
    __ClearFlag(0x313);
    if (t == 0x67)
        __SetFlag(0x313);
    else if (t == 0x69)
        __SetFlag(0x312);
    OvlFunc_895_20097c0(0);
}

void OvlFunc_895_200867c(void)
{
    unsigned char *a;
    int t;

    a = (unsigned char *)__MapActor_GetActor(0xb);
    if (a == 0)
        return;
    t = *(int *)(a + 8) >> 20;
    __ClearFlag(0xc5 << 2);
    __ClearFlag(0x315);
    if (t == 0x6b)
        __SetFlag(0x315);
    else if (t == 0x6d)
        __SetFlag(0xc5 << 2);
    OvlFunc_895_20097c0(0);
}

void OvlFunc_895_20086c4(void)
{
    unsigned char *a;
    int t;

    a = (unsigned char *)__MapActor_GetActor(0xc);
    if (a == 0)
        return;
    t = *(int *)(a + 8) >> 20;
    __ClearFlag(0x316);
    __ClearFlag(0x317);
    if (t == 0x6f)
        __SetFlag(0x317);
    else if (t == 0x71)
        __SetFlag(0x316);
    OvlFunc_895_20097c0(0);
}

void OvlFunc_895_200870c(void)
{
    unsigned char *a;
    int t;

    a = (unsigned char *)__MapActor_GetActor(0xd);
    if (a == 0)
        return;
    t = *(int *)(a + 8) >> 20;
    __ClearFlag(0xc6 << 2);
    __ClearFlag(0x319);
    if (t == 0x73)
        __SetFlag(0x319);
    else if (t == 0x75)
        __SetFlag(0xc6 << 2);
    OvlFunc_895_20097c0(0);
}

void OvlFunc_895_2008754(void)
{
    unsigned char *a;
    int t;

    a = (unsigned char *)__MapActor_GetActor(0xe);
    if (a == 0)
        return;
    t = *(int *)(a + 8) >> 20;
    __ClearFlag(0x31a);
    __ClearFlag(0x31b);
    if (t == 0x77)
        __SetFlag(0x31b);
    else if (t == 0x79)
        __SetFlag(0x31a);
    OvlFunc_895_20097c0(0);
}
