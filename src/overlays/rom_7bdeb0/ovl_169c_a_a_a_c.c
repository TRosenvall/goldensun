/* Cluster OvlFunc_934_20096f0..OvlFunc_934_20096f0 extracted from goldensun/asm/overlays/rom_7bdeb0/ovl_169c_a_a_a_c.s.
 *
 * The .s held ONLY this function and no data, so no split was needed -- the .o
 * keeps its name and its slot in goldensun/overlays/rom_7bdeb0/overlay.ld is
 * unchanged.
 *
 * Writes a mode word into the block at iwram_3001ebc, and in one particular
 * area writes a different one and runs a short setup instead.
 *
 * THE POOL TELL FIRES ON 0x5d. The ROM compares with
 *
 *     ldr r3, =0x5d / cmp r2, r3
 *
 * where `cmp r2, #0x5d` would encode fine. gcc never pools a constant it can
 * build with an eight-bit `mov`, so that operand was a SYMBOL -- and it is read
 * from `gState + 0x1c0`, which is the same halfword GetEntrances compares in
 * src/overlays/rom_79aad8/ovl_314_a.c. So this is an area id, `_AREA_5d` was
 * already defined in area.sym from an earlier batch, and the comparison needed
 * no new name.
 *
 * That is the first time the area namespace has been reused rather than
 * extended, which is a small piece of evidence for it: a symbol defined for one
 * overlay turned out to be exactly what another overlay's unrelated function
 * needed, at the same struct offset.
 *
 * THE OFFSET 0x1c0 IS BUILT ONCE AND USED TWICE -- as a byte offset into the
 * iwram block and as the register operand of an `ldrsh` against gState:
 *
 *     mov r2, #0xe0 / lsl r2, #1 / add r1, r3, r2 / ... / ldrsh r2, [r3, r2]
 *
 * so it is written as a named local built in statement form, not folded into
 * either access. Folded, both become addressing-mode constants and the shared
 * `r2` disappears.
 *
 * One declaration: __Func_8092b08 wants r0 filled first at both of its call
 * sites and gcc filled it last. Everything else is left implicit.
 */
extern void __Func_8092b08(int a, int b);

typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern int _AREA_5d;
extern unsigned int iwram_3001ebc;

int OvlFunc_934_20096f0(void)
{
    unsigned char *base;
    unsigned int off;
    unsigned int *p;

    base = (unsigned char *)iwram_3001ebc;
    off = 0xe0;
    off <<= 1;
    p = (unsigned int *)(base + off);
    *p = 0x81 << 2;
    if (*(short *)((char *)&gState + off) == (int)(&_AREA_5d)) {
        *p = 0x80 << 1;
        __WaitFrames(1);
        __Func_8092b08(0xb, 3);
        __Func_8092b08(0xc, 3);
        __ClearFlag(0x12f);
    }
    OvlFunc_934_2009984();
    return 0;
}
