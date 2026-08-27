/* OvlFunc_924_2009d3c
 *
 * Cut out of goldensun/asm//overlays/rom_7ac2d8/ovl_f84_c_c_b.s.
 *
 * The sibling of OvlFunc_924_2009568 with different tiles and a different
 * script blob -- a clean first screen once its twin was solved. Needs
 * CSE_CFLAGS.
 *
 * Two labels had to be exported for the split, `.L608e` and `.L6064`, each in
 * its own commit. A label emits no bytes.
 *
 * BUILT WITH CSE_CFLAGS.
 *
 * Drafted by a parallel screening agent; re-screened here before wiring.
 */
extern unsigned char L608e[] __asm__(".L608e");
extern int __GetFlag(int id);
extern void __ClearFlag(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void *__MapActor_GetActor(int slot);
extern void __CutsceneWait(int n);
extern int __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern void __PlaySound(int id);
extern void __Func_8010560(unsigned char *s, int a, int b);

void OvlFunc_924_2009d3c(void)
{
    unsigned char *a;
    unsigned char *b;

    if (__GetFlag(0x256) != 0) {
        __CutsceneStart();
        __ClearFlag(0x256);
        a = (unsigned char *)__MapActor_GetActor(0);
        *(int *)(a + 0xc) += 0x80 << 10;
        b = (unsigned char *)__MapActor_GetActor(0);
        *(int *)(b + 0x3c) = *(int *)((unsigned char *)__MapActor_GetActor(0) + 0xc);
        __CutsceneWait(5);
        __CopyMapTiles(8, 0x1d, 0xa, 0x17, 1, 1);
        __PlaySound(0xd9);
        __Func_8010560(L608e, 0xa, 0x12);
        __CutsceneEnd();
    }
}
