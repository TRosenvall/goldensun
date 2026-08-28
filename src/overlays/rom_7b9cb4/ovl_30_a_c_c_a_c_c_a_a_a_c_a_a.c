/* Cluster OvlFunc_932_200a490..OvlFunc_932_200a490, the whole of
 * goldensun/asm/overlays/rom_7b9cb4/ovl_30_a_c_c_a_c_c_a_a_a_c_a_a.s -- a single-function TU
 * with no data, so no split was needed and overlay.ld is untouched.
 *
 * Total .text for this TU = 304 bytes (= 0x0130).
 *
 * EXACT ON THE FIRST SCREEN, because it is a structural sibling of
 * OvlFunc_932_200a310 and both of that function's lessons carried over:
 *
 *   - THE OFFSET IS NAMED, NOT THE BASE.  `off = 0xe1 << 1` keeps gcc from
 *     folding gState+450 into one pooled constant while also keeping the base
 *     out of a callee-saved register across the preceding __GetFlag.  See the
 *     offset-naming note in docs/elevation.md, including its precondition --
 *     the inline form has to come out SHORT for the lever to be the answer, and
 *     here it does.
 *   - THE ACTOR BYTE AT 0x23 IS A NAMED STRUCT FIELD.  `a->flags |= 2` rather
 *     than pointer arithmetic; the arithmetic form is what parked
 *     OvlFunc_932_200a5c0 until batch 136.
 *
 * The four stack-argument pairs are named as pairs and s1/s2 are reused, each
 * pair dying before the next is set, which is what the ROM does.
 */
struct Actor {
    unsigned char pad0[0x23];
    unsigned char flags;
};

extern unsigned char gState[];
extern char *iwram_3001e70;

extern int __GetFlag(int id);
extern struct Actor *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __Func_8092950(int a, int b);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern void OvlFunc_932_200ae84(void);
extern void OvlFunc_932_200ba44(void);
extern void OvlFunc_932_200b460(int n);
extern void OvlFunc_932_200abb0(int a, int b, int c, int d);

void OvlFunc_932_200a490(void)
{
    char *w;
    struct Actor *a;
    int off;
    int s1, s2;

    if (__GetFlag(0x907)) {
        w = iwram_3001e70;
        *(unsigned short *)(w + 0x14) = 0xfdff & *(unsigned short *)(w + 0x14);
        __MapActor_SetPos(0xa, 0, 0);
    } else {
        off = 0xe1 << 1;
        if (__GetFlag(0x109) == 0 && *(short *)(gState + off) == 0x63)
            OvlFunc_932_200ae84();
        OvlFunc_932_200ba44();
        if (__GetFlag(0x907) == 0) {
            __Func_8092950(0xa, 2);
            __MapActor_SetAnim(0xa, 3);
            OvlFunc_932_200abb0(0xbb << 18, 0x80 << 12, 0x8c << 17, 0x80 << 8);
        }
    }
    OvlFunc_932_200b460(9);
    if (__GetFlag(0x80 << 2)) {
        __MapActor_SetAnim(9, 5);
        s1 = 0x19;
        s2 = 0xd;
        __Func_8010704(0x17, 0xd, 1, 1, s1, s2);
        a = __MapActor_GetActor(9);
        a->flags |= 2;
    }
    if (__GetFlag(0x325)) {
        s1 = 0xb;
        s2 = 0x49;
        __Func_8010704(0xa, 0x48, 1, 1, s1, s2);
        s1 = 1;
        s2 = 2;
        __CopyMapTiles(0x31, 0x20, 0xb, 4, s1, s2);
    } else {
        s1 = 0xb;
        s2 = 0x49;
        __Func_8010704(0xc, 0x48, 1, 1, s1, s2);
        s1 = 1;
        s2 = 2;
        __CopyMapTiles(0x30, 0x20, 0xb, 4, s1, s2);
    }
}
