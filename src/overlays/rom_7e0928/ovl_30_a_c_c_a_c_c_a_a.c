/* OvlFunc_956_20081c8  --  0x020081c8, was
 * asm/overlays/rom_7e0928/ovl_30_a_c_c_a_c_c_a_a.s (this function alone), so it
 * converts whole.
 *
 * Third member of the pre-header load merge (see Func_80064b8 in
 * src/rom_c0/rom_5cf8_a_a_c_c.c for the mechanism): the same `do { } while (0);`
 * barrier after the pre-header load, plus `i++` moved below __WaitFrames(1).
 */
extern int L5480 __asm__(".L5480");
extern int L5484 __asm__(".L5484");
extern void __WaitFrames(int n);

void OvlFunc_956_20081c8(void)
{
    int i;
    int v;

    __WaitFrames(0xa);
    v = L5480;
    do { } while (0);
    i = 0;
    goto check;
loop:
    __WaitFrames(1);
    i++;
    if (i > 0x77)
        return;
    v = L5480;
check:
    if (v != 3)
        goto loop;
    if (L5484 != 1)
        goto loop;
}
