/* Cluster OvlFunc_938_2008230..OvlFunc_938_2008230 extracted from goldensun/asm/overlays/rom_7c37ac/ovl_30_c_c_c_c_c.s.
 *
 * Slotted between ovl_30_c_c_c_c_c_a.o and the rest of the overlay.
 *
 * Writes 0x209 into the iwram_3001ebc block at +0x1c0 and dispatches on the
 * area, which is read at the SAME offset out of gState. The offset is one
 * variable because the ROM keeps it in r2 across both -- the store`s register
 * index and the `ldrsh` offset.
 *
 * The compared constant is POOLED where `cmp #imm` would do, so it is an area
 * id; `pop {r1}` is the return-value tell for `return 0`.
 *
 * Instruction-identical to ovl_7c3044/ovl_30_c_c_c_c_c_c_b.c except for the
 * area id and the callee.
 */
typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern unsigned char *iwram_3001ebc;

extern int _AREA_67;
extern void OvlFunc_938_2008264(void);

int OvlFunc_938_2008230(void)
{
    unsigned char *p;
    unsigned int off;

    p = iwram_3001ebc;
    off = 0xe0;
    off <<= 1;
    *(int *)(p + off) = 0x209;
    if (*(short *)((char *)&gState + off) == (int)(&_AREA_67))
        OvlFunc_938_2008264();
    return 0;
}
