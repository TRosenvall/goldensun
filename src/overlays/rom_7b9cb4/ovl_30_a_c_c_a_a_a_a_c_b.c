/* Cluster OvlFunc_932_20083b4..OvlFunc_932_20083b4 extracted from goldensun/asm/overlays/rom_7b9cb4/ovl_30_a_c_c_a_a_a_a.s.
 *
 * Preserves the original ROM layout when slotted between
 * asm/overlays/rom_7b9cb4/ovl_30_a_c_c_a_a_a_a_c_a.o and asm/overlays/rom_7b9cb4/ovl_30_a_c_c_a_a_a_a_c_c.o in
 * goldensun/overlays/rom_7b9cb4/overlay.ld.
 *
 * Second of three identical stubs differing only in the pooled id -- see
 * ovl_30_a_c_c_a_a_a_a_b.c for why _AREA_4f has to be a symbol rather than the
 * literal 0x4f, and area.sym for what is and is not being claimed about
 * the name.
 */
typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern int _AREA_4f;
extern void __Func_8091f90(int id, int b);
extern void __Func_8091eb0(int map, int entrance);

void OvlFunc_932_20083b4(void)
{
    unsigned char *gs;

    gs = (unsigned char *)&gState;
    gs[0x22b] = 3;
    __Func_8091f90((int) (&_AREA_4f), 0x63);
    __Func_8091eb0(0x35, 2);
}
