/* OvlFunc_948_2009f78  --  0x02009f78
 *
 * Cut out of goldensun/asm/overlays/rom_7d30e0/ovl_30_c_c_c_c_c_c_c_c.s.
 *
 * Runs whichever of three area-specific setup routines matches the area the
 * party is in, after resetting a script word. The three areas are tested in
 * turn rather than as a switch, and each test re-reads the area id, because the
 * routines can change it.
 *
 * THE THREE COMPARISONS ARE AREA SYMBOLS, NOT LITERALS. The ROM writes
 * `ldr r3, =0x75` where `mov r3, #0x75` would encode -- gcc never pools what an
 * eight-bit `mov` can build, and it always pools a symbol's address. area.sym
 * already defines _AREA_75, _AREA_76 and _AREA_78, so the pool entries name
 * themselves. This is the same tell that put _AREA_7e into the tree in batch 67.
 *
 * THE BASE POINTER IS A LOCAL, AND THE OFFSET IS SHARED. 0x1c0 is used twice --
 * once to index the script table off [iwram_3001ebc] and once to reach the area
 * id in gState -- and the ROM builds it once into r2 and keeps it. Naming the
 * offset gets that; what took the last five instructions was ALSO naming the
 * iwram pointer:
 *
 *      rom    ldr r1, [r3] ... str r3, [r1, r2] / add r5, r3, r2
 *      inline ldr r2, [r3] ... str r3, [r2, r1] / add r5, r3, r1
 *
 * Same instructions, r1 and r2 exchanged. Assigning `iwram_3001ebc` to a local
 * before computing the offset settles which of the two gets the preferred
 * register. Computing the offset first, or reordering the two statements that
 * use it, does not.
 */
extern char *iwram_3001ebc;
extern unsigned char gState[];
extern int _AREA_75;
extern int _AREA_76;
extern int _AREA_78;
extern void OvlFunc_948_200a188(void);
extern void OvlFunc_948_200a290(void);
extern void OvlFunc_948_200a334(void);

int OvlFunc_948_2009f78(void)
{
    short *a;
    int off;
    char *p;

    p = iwram_3001ebc;
    off = 0xe0 << 1;
    *(int *)(p + off) = 0x81 << 2;
    a = (short *)(gState + off);
    if (*a == (int)&_AREA_75)
        OvlFunc_948_200a188();
    if (*a == (int)&_AREA_76)
        OvlFunc_948_200a290();
    if (*a == (int)&_AREA_78)
        OvlFunc_948_200a334();
    return 0;
}
