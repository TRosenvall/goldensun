/* Cluster OvlFunc_932_200a020..OvlFunc_932_200a020 extracted from
 * goldensun/asm/overlays/rom_7b9cb4/ovl_30_a_c_c_a_c_c_a_a_a_a_c_a.s.
 *
 * A nine-way dispatch on the area id.  Matched on the first screen; two
 * readings decided it and both are already on record.
 *
 *   ALL NINE COMPARISONS ARE SYMBOLS.  Every id -- 0x4d, 0x4f, 0x50, 0x51,
 *   0x52, 0x53, 0x55, 0x56, 0x57 -- fits `cmp Rn, #imm8`, and the ROM pools
 *   every one of them.  That is the pooled-constant tell, the halfword at
 *   gState+0x1c0 is the area id by area.sym's own header, and all nine were
 *   already in the file.  Check before adding: I created duplicates of two of
 *   them in an earlier round by not looking first.
 *
 *   THE COMPARISONS ARE A LINEAR CHAIN, NOT A SWITCH.  `cmp / bne / bl / b` nine
 *   times in SOURCE order with no `bhi` or `blt` anywhere is the batch-148 tell
 *   that gcc did not build a balanced tree -- a switch would have sorted the
 *   cases and compared against midpoints.  Written as if/else-if it reproduces
 *   directly.
 *
 *   THE OFFSET 0x1c0 IS A NAMED LOCAL.  The ROM builds it once with
 *   `mov r2, #0xe0 / lsl r2, #1` and then uses r2 as the REGISTER OFFSET twice,
 *   against two different bases -- `str r3, [r1, r2]` into the iwram block and
 *   `ldrsh r2, [r3, r2]` out of gState.  One named `o` gives both; writing the
 *   offset into each expression separately would rebuild it.
 *
 * The reference keeps its literal pool inside the function, so tryc.py cannot
 * see PC-relative distance; verified with make compare.
 */
extern unsigned char *iwram_3001ebc;
extern unsigned char gState[];

extern int _AREA_4d;
extern int _AREA_4f;
extern int _AREA_50;
extern int _AREA_51;
extern int _AREA_52;
extern int _AREA_53;
extern int _AREA_55;
extern int _AREA_56;
extern int _AREA_57;

extern void OvlFunc_932_200a0d0(void);
extern void OvlFunc_932_200a310(void);
extern void OvlFunc_932_200a428(void);
extern void OvlFunc_932_200a490(void);
extern void OvlFunc_932_200a5c0(void);
extern void OvlFunc_932_200a6c0(void);
extern void OvlFunc_932_200a804(void);
extern void OvlFunc_932_200a934(void);
extern void OvlFunc_932_200a9dc(void);

int OvlFunc_932_200a020(void)
{
    unsigned char *p;
    unsigned char *g;
    int o;
    int area;

    p = iwram_3001ebc;
    o = 0xe0 << 1;
    *(int *)(p + o) = 0x81 << 2;
    g = gState;
    area = *(short *)(g + o);
    if (area == (int)&_AREA_4d)
        OvlFunc_932_200a0d0();
    else if (area == (int)&_AREA_4f)
        OvlFunc_932_200a310();
    else if (area == (int)&_AREA_50)
        OvlFunc_932_200a428();
    else if (area == (int)&_AREA_51)
        OvlFunc_932_200a490();
    else if (area == (int)&_AREA_52)
        OvlFunc_932_200a5c0();
    else if (area == (int)&_AREA_53)
        OvlFunc_932_200a6c0();
    else if (area == (int)&_AREA_55)
        OvlFunc_932_200a804();
    else if (area == (int)&_AREA_56)
        OvlFunc_932_200a934();
    else if (area == (int)&_AREA_57)
        OvlFunc_932_200a9dc();
    return 0;
}
