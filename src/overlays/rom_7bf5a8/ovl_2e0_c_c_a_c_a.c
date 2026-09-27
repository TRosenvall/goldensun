/* OvlFunc_935_2008704  --  0x02008704, was asm/overlays/rom_7bf5a8/ovl_2e0_c_c_a_c_a.s
 * (this function alone), so it converts whole.
 *
 * Parked at 22 differing. Batch 254's "a dead register is `| zero`" lever,
 * applied as written: `int z = 0` among the pre-loop initialisers and the store
 * as `*p = t | m | z;` ((t|z)|m and (t|m)|z are also exact; t|(m|z) is not,
 * because m|z folds to a constant before loop.c runs). Six for/biv-reversal
 * spellings stayed at 22-23, so the ROM's r8 is not a leftover induction
 * variable.
 */
extern unsigned char *__MapActor_GetActor(int slot);

void OvlFunc_935_2008704(void)
{
    unsigned char *p;
    int i;
    int c;
    int m;
    int t;
    int z;

    i = 0x10;
    m = 2;
    c = 5;
    z = 0;
    do {
        p = __MapActor_GetActor(i);
        p += 0x23;
        t = *p;
        c--;
        *p = t | m | z;
        i++;
    } while (c >= 0);
}
