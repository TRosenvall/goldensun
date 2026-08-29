/* Cluster OvlFunc_921_200816c..OvlFunc_921_200816c extracted from goldensun/asm/overlays/rom_7a7298/ovl_30_c_c_a_a.s.
 *
 * The .s held this function and nothing else, so the C replaces the whole
 * translation unit and the linker script is untouched.
 *
 * Selects a script pointer from the AREA ID at gState+0x1C0, patching the
 * selected table in place when a flag is set.
 *
 * The pooled 0x33 is `_AREA_33`, already defined in area.sym. The namespace
 * comes from the CONSUMER -- a value compared against gState+0x1C0 is an area
 * id -- not from the value, which is also defined in file_table.sym.
 *
 * THE ZERO OFFSET IS INLINED AS A CAST, `*(short *)(g + (unsigned int)0)`, and
 * that is the whole difference between 9 of 46 and an exact match. Thumb
 * `ldrsh` has no immediate form so the zero must live in a register, but naming
 * it as a local makes it a VALUE gcc can reuse -- here it reused it for the
 * `strb` of 0 further down, dropping the ROM's separate `mov r3, #0` and
 * pulling a callee-saved register into the prologue. Inlining the cast gives
 * the register without giving gcc a reusable value.
 *
 * That is a refinement of the usual advice. The named-offset form is right when
 * the zero is the only zero in the function; when another zero is stored later,
 * inline it instead. Writing `*t = 0;` for the store rather than through a local
 * does NOT fix it -- tried, byte-identical -- because the merge happens on the
 * offset side.
 */
typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern int _AREA_33;
extern unsigned char L2ad0[] __asm__(".L2ad0");
extern unsigned char L29e0[] __asm__(".L29e0");
extern unsigned char gOvl_0200aa58[];
extern void __Func_808b868(void *p);
extern int __GetFlag(int id);

void *OvlFunc_921_200816c(void)
{
    unsigned char *g;
    unsigned char *p;
    unsigned char *t;
    unsigned int k;
    int v;
    int z;
    int w;

    k = 0xe0 << 1;
    g = (unsigned char *)&gState + k;
    v = *(short *)(g + (unsigned int)0);
    if (v != (int)(&_AREA_33))
        goto other;
    p = L2ad0;
    __Func_808b868(p);
    if (__GetFlag(0x881) != 0) {
        k = 0x83 << 1;
        t = p + k;
        z = 0;
        *t = z;
        w = 0xb6 << 16;
        *(int *)(p + 0x50) = w;
        w = 0x8d << 18;
        *(int *)(p + 0x58) = w;
        w = 2;
        *(int *)(p + 0x4c) = w;
    }
    return p;
other:
    if (__GetFlag(0x881) != 0)
        return gOvl_0200aa58;
    return L29e0;
}
