/* Cluster OvlFunc_910_200809c..OvlFunc_910_200809c extracted from
 * goldensun/asm/overlays/rom_79dd90/ovl_30_c_c_a_a_a_a.s.
 *
 * Total .text for this TU = 70 bytes (= 0x46).
 * Placed in the run in goldensun/overlays/rom_79dd90/overlay.ld.
 *
 * Picks the area's script table, patching two bytes inside it first when the
 * corresponding save flags are set.
 *
 * `_AREA_22` is a pool tell: 0x22 fits `mov r3, #0x22`, so pooling it means
 * the operand was a symbol. It was already defined, and it is compared against
 * the gState halfword at +0x1c0 -- area.sym's own criterion.
 *
 * The two `__GetFlag` ids, 0x84f and 0x845, are NOT tells: both exceed 255 and
 * genuinely need a pool.
 */

typedef struct {
    unsigned char pad[0x1c0];
    short area;
} GlobalState;

extern GlobalState gState;
extern int _AREA_22;
extern unsigned char Lc7c[] __asm__(".Lc7c");
extern unsigned char gScript_889__02008c64[];
extern int __GetFlag(int id);

void *OvlFunc_910_200809c(void)
{
    if (gState.area == (int)(&_AREA_22)) {
        if (__GetFlag(0x84f) != 0)
            Lc7c[0x76] = 1;
        if (__GetFlag(0x845) != 0)
            Lc7c[0x46] = 0;
        return Lc7c;
    }
    return gScript_889__02008c64;
}
