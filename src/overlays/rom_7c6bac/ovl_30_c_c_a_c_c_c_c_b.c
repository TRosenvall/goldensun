/* Cluster OvlFunc_942_200886c..OvlFunc_942_200886c extracted from goldensun/asm/overlays/rom_7c6bac/ovl_30_c_c_a_c_c_c_c.s.
 *
 * Slotted between ovl_30_c_c_a_c_c_c_c_a.o and the rest of the overlay.
 *
 * TWO INDEPENDENT READS OUT OF gState, AND THEY NEED SEPARATE VARIABLES. The
 * first is at +0x1c2 and compared against a LITERAL 0x5a -- `cmp r3, #0x5a`,
 * an immediate, so not the pool tell and not an area id. The second is the area
 * at +0x1c0, compared against three pooled ids.
 *
 * Reusing one offset variable and one value variable for both reads is 6 of 39:
 * the whole register assignment shifts, because gcc keeps the reused locals in
 * different registers than the ROM's two independent ones. Giving each read its
 * own offset, pointer and value matches.
 *
 * That is the same theme as batch 49's stack-arg finding -- recycling a local
 * across independent operations perturbs allocation -- pointing the other way:
 * there the fix was to stop reusing, here it is the same. Prefer a fresh local
 * per independent operation unless the ROM shows a register genuinely held.
 *
 * gState's base IS genuinely held, in r5 across both reads, so that one is a
 * single local.
 */
typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern int _AREA_6b;
extern int _AREA_70;
extern int _AREA_6c;
extern void __SetFlag(int id);
extern void OvlFunc_942_20088cc(void);
extern void OvlFunc_942_2008958(void);
extern void OvlFunc_942_2008ad4(void);

int OvlFunc_942_200886c(void)
{
    unsigned char *g;
    unsigned char *p;
    unsigned char *q;
    unsigned int o1;
    unsigned int o2;
    short v1;
    short v2;

    g = (unsigned char *)&gState;
    o1 = 0xe1;
    o1 <<= 1;
    p = g + o1;
    o1 = 0;
    v1 = *(short *)(p + o1);
    if (v1 == 0x5a)
        __SetFlag(0x95 << 4);
    o2 = 0xe0;
    o2 <<= 1;
    q = g + o2;
    o2 = 0;
    v2 = *(short *)(q + o2);
    if (v2 == (int)(&_AREA_6b))
        OvlFunc_942_20088cc();
    else if (v2 == (int)(&_AREA_70))
        OvlFunc_942_2008958();
    else if (v2 == (int)(&_AREA_6c))
        OvlFunc_942_2008ad4();
    return 0;
}
