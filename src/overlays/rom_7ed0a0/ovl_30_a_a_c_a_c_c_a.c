/* OvlFunc_964_2008e20  --  0x02008e20
 *
 * The whole of goldensun/asm/overlays/rom_7ed0a0/ovl_30_a_a_c_a_c_c_a.s.
 *
 * Decides whether the camera should track the party leader, by comparing the
 * leader's depth against a per-room threshold.
 *
 * THE OUTER AREA TEST IS A SYMBOL, THE INNER CASES ARE NOT. `ldr r3, =0xac`
 * where `mov r3, #0xac` would encode is the symbol tell, and area.sym has
 * `_AREA_ac`. The inner switch's case values (3..13) are compared with plain
 * immediates in the range check, so they are literals -- the assembly
 * distinguishes the two even though both are area numbers.
 *
 * THE `if` ARMS ARE ROUND THE OTHER WAY. The ROM branches on `bgt` to the arm
 * that stores the actor, so the source tests `<= lim` and stores zero in the
 * fall-through. Written the natural way round it costs six positions.
 */
struct A { unsigned char pad00[0x10]; int f10; };
struct B { unsigned char pad00[0x18]; struct A *f18; };

extern unsigned char gState[];
extern struct B *iwram_3001ee0;
extern int _AREA_ac;
extern struct A *__MapActor_GetActor(int slot);

void OvlFunc_964_2008e20(void)
{
    struct A *a;
    struct B *b;
    unsigned char *g;
    int lim;

    a = __MapActor_GetActor(0);
    b = iwram_3001ee0;
    g = gState;
    lim = 0;
    if (*(short *)(g + (0xe0 << 1)) == (int)&_AREA_ac) {
        switch (*(short *)(g + (0xe1 << 1))) {
        case 3:
        case 4:
            lim = 0x5e;
            break;
        case 8:
        case 9:
            lim = 0x4a;
            break;
        case 12:
        case 13:
            lim = 0x76;
            break;
        }
    } else if (*(short *)(g + (0xe1 << 1)) == 0xc) {
        lim = 0x5d;
    }
    if ((a->f10 >> 19) <= lim)
        b->f18 = 0;
    else
        b->f18 = a;
}
