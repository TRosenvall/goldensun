/* OvlFunc_907_2008fa0  [overlays/rom_79b154]
 *
 * *** THIS C MATCHES BYTE-EXACT. IT IS NOT A NON-MATCH. ***
 *
 * It is filed here because the park corpus is where the next person looks for
 * a function's prior art, not because anything is wrong with the code. The
 * blocker is FILE STRUCTURE, not codegen.
 *
 * Verified: `tryc.py --ref asm/overlays/rom_79b154/ovl_30_c_c_c.s` reports
 * OK at 59 lines. Screened twice.
 *
 * WHY IT IS NOT INSTALLED. That .s holds this function AND 21 `.incbin` blobs
 * pulling data straight out of overlays/rom_79b154/orig.bin, under 19 global
 * labels -- ActorCmd_ARRAY_907__020091c0, gOvl_02009d3c, gScript_907__02009d7c
 * and others -- which the rest of the overlay references. Converting the file
 * wholesale deletes that data and the link dies in a page of undefined
 * references. I did exactly that once; it is reverted.
 *
 * tools/split_s.py REFUSES this file by design and says why. Run it and read
 * its output rather than assuming: it reports the blob and label counts and
 * states that converting would break the link. To install this function
 * someone has to split code from data BY HAND -- a new _b.s holding only the
 * function, a _c.s holding the blobs, and both listed in the overlay script in
 * the original order -- then verify `make compare` is still green BEFORE
 * dropping this C in. That is a separate job from decompiling it, and it is
 * the only thing standing between this file and an elevation.
 *
 * THE DECOMPILATION ITSELF IS SETTLED, and the one non-obvious part is the
 * stack-argument pair. Each `else` arm needs its OWN block-scoped pair of
 * locals: sharing one pair across both else arms is 8 differing, and bare
 * literals everywhere is 6 -- both emit `mov r3 / str / mov r3 / str` where
 * the ROM has `mov r3 / mov r2 / str / str`. The `==` arms keep literals,
 * because there the ROM reuses the already-computed shifted value as the sixth
 * argument instead of rebuilding it.
 */
extern unsigned char *__MapActor_GetActor(int slot);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

void OvlFunc_907_2008fa0(void)
{
    unsigned char *a;

    a = __MapActor_GetActor(8);
    if (a == 0)
        return;
    if (*(int *)(a + 0x10) >> 20 == 6) {
        __Func_8010704(2, 0, 1, 1, 0xe, 6);
    } else {
        int e0 = 0xe;
        int f0 = 6;
        __Func_8010704(0, 0, 1, 1, e0, f0);
    }
    if (*(int *)(a + 0x10) >> 20 == 9) {
        __Func_8010704(2, 0, 1, 1, 0xe, 9);
    } else {
        int e1 = 0xe;
        int f1 = 9;
        __Func_8010704(1, 0, 1, 1, e1, f1);
    }
}
