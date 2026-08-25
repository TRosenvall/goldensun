/* Cluster OvlFunc_952_200c034..OvlFunc_952_200c034 extracted from goldensun/asm/overlays/rom_7d768c/ovl_30_c_c.s.
 *
 * Total .text for this TU = 90 bytes (= 0x5a).
 * The remaining function and the .data section stay in the original .s, which
 * keeps its name and its .data line in goldensun/overlays/rom_7d768c/overlay.ld;
 * only the .text line is repointed here.
 *
 * Selects a script/table pointer from the AREA ID at gState+0x1C0.
 *
 * The compared constants are pooled by the ROM where `cmp r2, #imm8` would do
 * -- the pool tell -- so they are symbols, and they were already defined in
 * area.sym. The namespace comes from the CONSUMER, not the value: 95 small
 * values are defined in more than one .sym file, and a value compared against
 * gState+0x1C0 is an area id. See tools/sym_candidates.py.
 *
 * The two arms are SYMMETRIC -- the same pair of flag tests on each side of the
 * area check, selecting from six tables. Writing the area test as
 * `if (v != _AREA_8b) goto other;` keeps the matching arm as the fall-through,
 * which is where the ROM has it.
 */
typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern int _AREA_8b;
extern unsigned char L5ad8[] __asm__(".L5ad8");
extern unsigned char L5a48[] __asm__(".L5a48");
extern unsigned char L59e8[] __asm__(".L59e8");
extern unsigned char L5688[] __asm__(".L5688");
extern unsigned char L5394[] __asm__(".L5394");
extern unsigned char L5004[] __asm__(".L5004");
extern int __GetFlag(int id);

void *OvlFunc_952_200c034(void)
{
    unsigned char *g;
    unsigned int k;
    unsigned int o;
    int v;

    k = 0xe0 << 1;
    g = (unsigned char *)&gState + k;
    o = 0;
    v = *(short *)(g + o);
    if (v != (int)(&_AREA_8b))
        goto other;
    if (__GetFlag(0x95 << 4) != 0)
        return L5ad8;
    if (__GetFlag(0x962) != 0)
        return L5a48;
    return L59e8;
other:
    if (__GetFlag(0x95 << 4) != 0)
        return L5688;
    if (__GetFlag(0x962) != 0)
        return L5394;
    return L5004;
}
