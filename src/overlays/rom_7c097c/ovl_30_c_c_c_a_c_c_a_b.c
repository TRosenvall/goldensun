/* Cluster OvlFunc_936_2009858..OvlFunc_936_2009858 extracted from goldensun/asm/overlays/rom_7c097c/ovl_30_c_c_c_a_c_c_a.s.
 *
 * Preserves the original ROM layout when slotted between
 * asm/overlays/rom_7c097c/ovl_30_c_c_c_a_c_c_a_a.o and the rest of the overlay
 * in goldensun/overlays/rom_7c097c/overlay.ld.
 *
 * Three independent guards run in sequence: a one-shot init, a facing reset,
 * and an area check. Note they are three separate `if`s and not a chain -- the
 * ROM's labels sit one after another with no branch between them.
 *
 * `*(short *)(a + 6) = z` goes through a named `int`, the inverted
 * narrow_constant lever; written as a literal gcc pools the zero as a halfword.
 * The gState offset is a walked pointer (`g +=`), which is what the ROM's
 * destructive `add r3, r2` needs.
 */
typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern int __GetFlag(int id);
extern void OvlFunc_936_200ba3c(int a);
extern void *__MapActor_GetActor(int slot);
extern void OvlFunc_936_200a6c0(void);

void OvlFunc_936_2009858(void)
{
    unsigned char *a;
    unsigned char *g;
    int z;

    if (!__GetFlag(0xfd6))
        OvlFunc_936_200ba3c(0xc);
    if (__GetFlag(0x915)) {
        a = (unsigned char *)__MapActor_GetActor(8);
        z = 0;
        *(short *)(a + 6) = z;
    }
    g = (unsigned char *)&gState;
    g += 0xe1 << 1;
    if (*(short *)g == 0xa)
        OvlFunc_936_200a6c0();
}
