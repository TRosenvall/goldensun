/* OvlFunc_948_20099e8  --  0x020099e8
 *
 * The whole of goldensun/asm/overlays/rom_7d30e0/ovl_30_c_c_a_a_c_a.s, which
 * held this function and nothing else, so the linker script's existing line for
 * that object now picks up this file's.
 *
 * Four map-rectangle repaints and then a flag byte on actor 8 -- the tail of a
 * cutscene, putting the scenery back.
 *
 * THE LOCAL BELOW IS NOT FORCED -- MEASURED. The ROM sets `mov r5, #0x2a` once
 * before the first call and every call site then does `str r5, [sp, #4]`, which
 * reads as a named local surviving across four calls; the push list `{r5, lr}`
 * seems to confirm it. Passing 0x2a as a plain literal at all four sites
 * compiles to the SAME forty-three instructions. gcc hoists the repeated
 * constant into r5 by itself.
 *
 * The local is kept because it says what the function means, not because it was
 * needed. This is the third time in three batches that "the ROM holds a value
 * in a callee-saved register" has looked like evidence the source named it and
 * turned out not to be, so it is worth treating as a standing non-signal rather
 * than re-testing each time. What HAS been forced, repeatedly, is the opposite
 * shape: two DIFFERENT constants in the two stack slots, where naming both is
 * what stops gcc walking one register through both stores.
 *
 * The fifth argument varies -- 0x28, 0x29, the local, 0x25 -- and the ROM builds
 * each fresh into r3, so nothing there was ever a candidate for naming.
 *
 * Matched on the first screen.
 */
struct A { unsigned char pad00[0x55]; unsigned char f55; };

extern struct A *__MapActor_GetActor(int slot);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

void OvlFunc_948_20099e8(void)
{
    int f;

    f = 0x2a;
    __Func_8010704(0x39, 0x2a, 1, 1, 0x28, f);
    __Func_8010704(0x39, 0x2a, 1, 1, 0x29, f);
    __Func_8010704(0x3a, 0x2a, 1, 1, f, f);
    __Func_8010704(0x3e, 0x25, 3, 1, 0x25, f);
    __MapActor_GetActor(8)->f55 = 1;
}
