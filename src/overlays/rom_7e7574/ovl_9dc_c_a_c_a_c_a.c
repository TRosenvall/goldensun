/* Cluster OvlFunc_959_2009718..OvlFunc_959_2009718 extracted from goldensun/asm/overlays/rom_7e7574/ovl_9dc_c_a_c_a_c_a.s.
 *
 * The .s held ONLY this function and no data, so no split was needed.
 *
 * A polling task. Once its sub-check passes and a gState halfword has gone to
 * zero, it unregisters ITSELF and writes 0x5f to [iwram_3001ebc]+0x182.
 *
 * THE SELF-REFERENCE is why the forward declaration above the definition is
 * there: `__StopTask(OvlFunc_959_2009718)` needs the symbol in scope inside its
 * own body, and the ROM's `ldr r0, =OvlFunc_959_2009718` is the tell that a
 * task unregisters itself rather than being cancelled from outside.
 *
 * THE IWRAM BASE IS READ BEFORE THE CALL, not at the point of use -- the ROM
 * loads it into r5 in the prologue and keeps it across `OvlFunc_959_20098e4`.
 * Written at the point of use, gcc reloads it after the call and the two
 * streams part company at the prologue. This is the same "live at the right
 * moment" question as the stack-arg-pair lever, and here the answer is the
 * opposite one: the value has to be made live EARLIER, because the ROM's is.
 *
 * The 0x5f store goes through a named `int` for the inverted-narrow_constant
 * reason -- see src/overlays/rom_77a7c8/ovl_30_c_a_c_c_a_c_a_c_b.c -- and the
 * gState offset is a destructive `add` on a walked pointer, which is why `g` is
 * advanced with `+=` rather than indexed.
 */
typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern unsigned int iwram_3001ebc;

extern int OvlFunc_959_20098e4(int n);
extern void __StopTask(void *fn);
void OvlFunc_959_2009718(void);

void OvlFunc_959_2009718(void)
{
    unsigned char *base;
    unsigned char *g;
    short *p;
    int v;

    base = (unsigned char *)iwram_3001ebc;
    if (OvlFunc_959_20098e4(0xc)) {
        g = (unsigned char *)&gState;
        g += 0x93 << 2;
        if (*(short *)g == 0) {
            __StopTask(OvlFunc_959_2009718);
            p = (short *)(base + (0xc1 << 1));
            v = 0x5f;
            *p = v;
        }
    }
}
