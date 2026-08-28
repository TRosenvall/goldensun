/* Cluster OvlFunc_932_200a5c0..OvlFunc_932_200a5c0 extracted from goldensun/asm/overlays/rom_7b9cb4/ovl_30_a_c_c_a_c_c_a_a_a_c_a.s.
 *
 * Total .text for this TU = 256 bytes (= 0x0100).
 * Slotted between ..._a.o and ..._c.o in goldensun/overlays/rom_7b9cb4/overlay.ld.
 *
 * RECOVERED FROM A PARK, and the park's diagnosis was wrong in a way worth
 * keeping.  It sat at 2 of 107 and was filed under the "commutative
 * register-role swap" -- the class that 21 parks describe and that seven
 * spellings and four flag groups had failed to move:
 *
 *      rom   ldrb r2, [r5] / mov r3, #0x2 / orr r3, r2 / strb r3, [r5]
 *      ours  ldrb r3, [r5] / mov r2, #0x2 / orr r3, r2 / strb r3, [r5]
 *
 * It is not a register-allocation wall.  It is a MISSING TYPE.  Written as
 * pointer arithmetic on an unsigned char* --
 *
 *      p += 0x23;  *p = 2 | *p;
 *
 * -- gcc puts the loaded byte in the orr's destination.  Written as a field of a
 * struct whose layout names that byte,
 *
 *      p->flags |= 2;
 *
 * it puts the constant there, which is what the ROM does.  Exact, first try.
 *
 * The struct is declared locally rather than including include/actor.h, which is
 * what the three other matched files touching this offset also do; only the two
 * bytes this function reaches need to be right.
 */
extern unsigned char gState[];
extern char *iwram_3001e70;

extern int __GetFlag(int id);
struct Actor {
    unsigned char pad0[0x23];
    unsigned char flags;
};
extern struct Actor *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void OvlFunc_932_200b460(int n);

void OvlFunc_932_200a5c0(void)
{
    unsigned char *g;
    struct Actor *p;
    char *w;
    int s1, s2;
    int x, y;

    g = gState;
    if (*(short *)(g + (0xe1 << 1)) == 2) {
        x = 0xb3 << 17;
        y = 0xd0 << 15;
        if (__GetFlag(0x109) == 0)
            __MapActor_SetPos(8, x, y);
    }
    OvlFunc_932_200b460(9);
    if (__GetFlag(0x80 << 2)) {
        p = __MapActor_GetActor(9);
        __MapActor_SetAnim(9, 5);
        s1 = 0x2b;
        s2 = 0x29;
        __Func_8010704(0x2d, 0x29, 1, 1, s1, s2);
        p->flags |= 2;
    }
    if (__GetFlag(0x907)) {
        w = iwram_3001e70;
        *(unsigned short *)(w + 0x14) = 0xfdff & *(unsigned short *)(w + 0x14);
    }
    if (__GetFlag(0x326)) {
        s1 = 0x10;
        s2 = 0x5c;
        __Func_8010704(0x11, 0x5d, 1, 1, s1, s2);
        s1 = 1;
        s2 = 2;
        __CopyMapTiles(0x2e, 0x1d, 0x10, 0x1c, s1, s2);
    } else {
        s1 = 0x10;
        s2 = 0x5c;
        __Func_8010704(0xf, 0x5d, 1, 1, s1, s2);
        s1 = 1;
        s2 = 2;
        __CopyMapTiles(0x2f, 0x1d, 0x10, 0x1c, s1, s2);
    }
}
