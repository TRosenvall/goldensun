/* OvlFunc_947_200a4cc
 *
 * Cut out of goldensun/asm//overlays/rom_7d0e88/ovl_1528_c_c_c_c_c_a.s.
 *
 * Drops the bridge once the player reaches the right column.
 *
 * The stored zero at +0x55 is the __GetFlag RESULT, not a separate literal --
 * the ROM writes `strb r6` where r6 holds the flag value the `if` has just
 * proved is zero. Needs CSE_CFLAGS.
 *
 * BUILT WITH CSE_CFLAGS.
 *
 * Drafted by a parallel screening agent; re-screened here before wiring.
 */
extern void *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void OvlFunc_947_2008528(int a, int b, int c, int d, int e, int f);
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __CutsceneWait(int n);
extern void __PlaySound(int id);

void OvlFunc_947_200a4cc(void)
{
    unsigned char *a;
    int f;

    a = (unsigned char *)__MapActor_GetActor(0xa);
    __CutsceneStart();
    OvlFunc_947_2008528(2, *(int *)(a + 8) >> 20, *(int *)(a + 0x10) >> 20,
                        1, 1, 0xff);
    if ((*(int *)(a + 8) >> 20) == 0x10) {
        f = __GetFlag(0x81 << 2);
        if (f == 0) {
            __CutsceneWait(0xa);
            __PlaySound(0x9f);
            a[0x55] = f;
            *(int *)(a + 0x14) = 0xfffe0000;
            *(int *)(a + 0xc) = 0xfffe0000;
            __SetFlag(0x81 << 2);
        }
    }
    __CutsceneEnd();
}
