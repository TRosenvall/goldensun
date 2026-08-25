/* Cluster OvlFunc_963_200808c..OvlFunc_963_200808c extracted from goldensun/asm/overlays/rom_7ec968/ovl_30_c_c_a_a_a.s.
 *
 * The .s held this function and nothing else, so the C replaces the whole
 * translation unit and the linker script is untouched.
 *
 * Selects a script/table pointer from the AREA ID at gState+0x1C0.
 *
 * UNBLOCKED BY NAMING TWO CONSTANTS. `_AREA_a9` and `_AREA_aa` did not exist
 * before batch 67; 0xa9 was defined only in file_table.sym. The evidence for
 * adding them is area.sym's own stated criterion -- both values are COMPARED
 * AGAINST the halfword at gState+0x1C0 -- and a file id and an area id sharing
 * a number is not a contradiction, since the consumer is what distinguishes
 * them.
 *
 * This is the first function in the corpus elevated by ADDING a symbol rather
 * than by finding one that already existed. Seven more values were added on the
 * same evidence and eleven functions between them were unblocked; see the
 * batch-67 block in area.sym.
 */
typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern int _AREA_a9;
extern int _AREA_aa;
extern unsigned char Lba8[] __asm__(".Lba8");
extern unsigned char Lc98[] __asm__(".Lc98");
extern unsigned char Lb90[] __asm__(".Lb90");
extern unsigned char gOvl_02008c50[];
extern int __GetFlag(int id);

void *OvlFunc_963_200808c(void)
{
    unsigned char *g;
    unsigned int k;
    int v;

    k = 0xe0 << 1;
    g = (unsigned char *)&gState + k;
    v = *(short *)(g + (unsigned int)0);
    if (v == (int)(&_AREA_aa))
        return Lba8;
    if (v == (int)(&_AREA_a9)) {
        if (__GetFlag(0x96f) != 0)
            return Lc98;
        return gOvl_02008c50;
    }
    return Lb90;
}
