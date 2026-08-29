/* Cluster OvlFunc_926_200a574..OvlFunc_926_200a574 extracted from goldensun/asm/overlays/rom_7b2078/ovl_314_c_c_a_c_c_c_c_c.s.
 *
 * Total .text for this TU = 54 bytes (= 0x36).
 * Preserves the original ROM layout when slotted immediately before
 * asm/overlays/rom_7b2078/ovl_314_c_c_a_c_c_c_c_c_b.o in
 * goldensun/overlays/rom_7b2078/overlay.ld.
 *
 * A three-way selector on the area id at gState+0x1C0 and a second halfword at
 * +0x1C2.
 *
 * ELEVATED BY NAMING A CONSTANT. The ROM pools 0x3c where `cmp r2, #0x3c`
 * would do -- the pool tell -- so the operand is a symbol. `_AREA_3c` was
 * already defined in area.sym; `extern int _AREA_3c;` and
 * `(int)(&_AREA_3c)` reproduce the pool load exactly.
 *
 * Note the SECOND comparison is against a literal 3 and the ROM builds it with
 * `cmp r3, #0x3`, not a pool load. Both forms appear in one function, which is
 * the clearest possible demonstration that the pool tell distinguishes symbols
 * from literals rather than being an artifact.
 *
 * The two halfword reads need SEPARATE locals. Reusing one variable puts the
 * second value back in the first read's register and costs two instructions;
 * a fresh local gives the ROM's r3.
 */
typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern int _AREA_3c;
extern unsigned char L4b90[] __asm__(".L4b90");
extern unsigned char L5184[] __asm__(".L5184");
extern unsigned char L4d40[] __asm__(".L4d40");

void *OvlFunc_926_200a574(void)
{
    unsigned char *b;
    unsigned char *g;
    unsigned int k;
    unsigned int o;
    int v;
    int w;

    b = (unsigned char *)&gState;
    k = 0xe0 << 1;
    g = b + k;
    o = 0;
    v = *(short *)(g + o);
    if (v == (int)(&_AREA_3c))
        return L4b90;
    k = 0xe1 << 1;
    g = b + k;
    o = 0;
    w = *(short *)(g + o);
    if (w == 3)
        return L5184;
    return L4d40;
}
