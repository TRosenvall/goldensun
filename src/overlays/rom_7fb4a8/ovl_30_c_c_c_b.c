/* Cluster OvlFunc_971_2009228..OvlFunc_971_2009228 extracted from
 * goldensun/asm/overlays/rom_7fb4a8/ovl_30_c_c.s.
 *
 * The twin of OvlFunc_971_20091bc, differing in exactly two constants -- the
 * message ids 0x292c/0x292d against 0x292a/0x292b.  Read
 * src/overlays/rom_7fb4a8/ovl_30_c_c_b.c for why __CloseUIBox is declared
 * twice, once returning void under an __asm__ alias and once returning int;
 * the two call sites need different argument orders and the return type is
 * what selects between them.
 */
extern void __PlaySound(int id);
extern int __Func_8017658(int id, int a, int b, int c);
extern int __Func_8017364(void);
extern void __WaitFrames(int n);
extern void __Func_801faa8(void);
extern int __CloseUIBox(int h, int n);
extern void CloseBoxV(int h, int n) __asm__("__CloseUIBox");

int OvlFunc_971_2009228(void)
{
    int h;

    __PlaySound(0x55);
    h = __Func_8017658(0x292c, 5, 4, 1);
    while (__Func_8017364() == 0)
        __WaitFrames(1);
    __Func_801faa8();
    CloseBoxV(h, 1);
    __WaitFrames(1);
    h = __Func_8017658(0x292d, 5, 4, 1);
    while (__Func_8017364() == 0)
        __WaitFrames(1);
    return __CloseUIBox(h, 1);
}
