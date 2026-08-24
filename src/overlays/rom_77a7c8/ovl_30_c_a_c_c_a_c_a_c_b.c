/* Cluster OvlFunc_881_200a8a8..OvlFunc_881_200a8a8 extracted from goldensun/asm/overlays/rom_77a7c8/ovl_30_c_a_c_c_a_c_a_c.s.
 *
 * Preserves the original ROM layout when slotted between
 * asm/overlays/rom_77a7c8/ovl_30_c_a_c_c_a_c_a_c_a.o and asm/overlays/rom_77a7c8/ovl_30_c_a_c_c_a_c_a_c_c.o in
 * goldensun/overlays/rom_77a7c8/overlay.ld.
 *
 * A cutscene that sets one halfword in the iwram block if a flag is set.
 *
 * A NEW COROLLARY OF narrow_constant, and it runs the opposite way to every
 * other instance of that class. Here gcc POOLS a constant the ROM builds with a
 * `mov`:
 *
 *     rom    mov r3, #0x1
 *     ours   ldrh r3, .L1        (a halfword pool entry holding 1)
 *
 * The store is `*(u16 *)p = 1`, so gcc narrows the whole expression to sixteen
 * bits and materialises the literal as a halfword pool constant rather than an
 * eight-bit immediate. Assigning through an `int` local first --
 *
 *     v = 1;  *(u16 *)p = v;
 *
 * -- keeps the value word-wide until the store and gcc emits `mov r3, #1`.
 *
 * Every previous member of this class had the ROM using a WIDE constant where
 * gcc narrowed (`mov #0xd / neg` for ~0xc against `mov #0xf3`). This is the
 * same mechanism -- gcc narrows the operation, not the operand -- observed from
 * the other side: narrowing can turn a `mov` into a POOL LOAD as well as a
 * pool load into a `mov`. Worth knowing because the symptom looks like the
 * pool tell, which would send you looking for a symbol that does not exist.
 *
 * The offset is built at runtime (`mov r1, #0xb9 / lsl r1, #1`) and the sum
 * goes into a THIRD register (`add r2, r3, r1`, not destructive), so the base,
 * the offset and the sum are three separate named locals.
 */
extern unsigned int iwram_3001ebc;
extern void __Func_801776c(int a, int b);

void OvlFunc_881_200a8a8(void)
{
    unsigned char *base;
    unsigned char *p;
    unsigned int off;
    int v;

    __CutsceneStart();
    __Func_808c44c();
    __Func_801776c(0x264c, 1);
    if (__GetFlag(0x8d << 2)) {
        base = (unsigned char *)iwram_3001ebc;
        off = 0xb9;
        off <<= 1;
        p = base + off;
        v = 1;
        *(unsigned short *)p = v;
    }
    __Func_808c4c0();
    __CutsceneEnd();
}
