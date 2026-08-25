/* Cluster OvlFunc_952_2008070..OvlFunc_952_2008070 extracted from goldensun/asm/overlays/rom_7d768c/ovl_30_c_a_a_a_a.s.
 *
 * The .s held this function and nothing else, so the C replaces the whole
 * translation unit and the linker script is untouched.
 *
 * A four-way selector on the area id at gState+0x1C0.
 *
 * UNPARKED BY NAMING A CONSTANT. This sat in src/non_matching/ at 7 of 32 with
 * the pool tell as its diagnosis: the ROM spends a literal-pool word on 0x8b
 * where `cmp r2, #0x8b` would do, which means the operand was a SYMBOL. The
 * park said so and called it a maintainer's decision.
 *
 * It was not a decision -- `_AREA_8b` was ALREADY DEFINED in area.sym, along
 * with 117 other area ids, and the park simply had not checked. Declaring
 * `extern int _AREA_8b;` and comparing against `(int)(&_AREA_8b)` takes this
 * from 7 of 32 to an exact match. gcc pools a symbol's address and never pools
 * a constant it can build with an eight-bit `mov`, which is exactly the
 * asymmetry the pool tell describes.
 *
 * An absolute symbol definition emits no bytes, so the link is byte-identical
 * and `make compare` proves it.
 */
typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern int _AREA_8b;
extern unsigned char L4b3c[] __asm__(".L4b3c");
extern unsigned char L4e6c[] __asm__(".L4e6c");
extern unsigned char L4d64[] __asm__(".L4d64");
extern unsigned char L4b84[] __asm__(".L4b84");
extern int __GetFlag(int id);

void *OvlFunc_952_2008070(void)
{
    unsigned char *g;
    unsigned int k;
    unsigned int o;
    int v;

    k = 0xe0 << 1;
    g = (unsigned char *)&gState + k;
    o = 0;
    v = *(short *)(g + o);
    if (v == (int)(&_AREA_8b))
        return L4b3c;
    if (__GetFlag(0x95 << 4) != 0)
        return L4e6c;
    if (__GetFlag(0x962) != 0)
        return L4d64;
    return L4b84;
}
