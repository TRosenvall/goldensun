/* OvlFunc_964_2009a10  --  0x02009a10
 *
 * The whole of goldensun/asm/overlays/rom_7ed0a0/ovl_30_a_c_c_c_c_c_c.s, which
 * held this function and no data, so the linker script's existing line for that
 * object now picks up this file's.
 *
 * The end of a cutscene: put actor 9 back under player control, clear its
 * scripted-motion bit, record the scene with save bit 0x204, repaint the tile
 * it is standing on, and hand both actors back to the idle callback.
 *
 * THE POSITION IS READ WITH TWO SEPARATE __MapActor_GetActor CALLS, one per
 * coordinate, exactly as in src/overlays/rom_7ed0a0/ovl_30_c_c_c_c_c_a.c.
 * Fetching once into a local drops a call.
 *
 * THE MASK IS A PLAIN LITERAL, AND THAT MATTERS. `*p &= 0xfd` matches;
 * a named `unsigned char mask = 0xfd` does NOT, and puts the loaded byte in the
 * `and`'s destination where the ROM has the constant.
 *
 * That is the reverse of batch 97's finding on
 * src/overlays/rom_7ced6c/ovl_30_c_c_c_c_c_a_a_c_b.c, where a named
 * `unsigned char` was exactly what put the constant in the destination of an
 * `orr`. So the type-of-the-named-constant lever is not a rule about matching
 * the field's width; it is a spelling to TRY, and here the untouched literal is
 * the one that works. `int mask` and `*p = 0xfd & *p` also match.
 *
 * THE CALLBACK POINTER IS NOT NAMED. The ROM keeps it in r5 across two calls,
 * which reads as a local; writing it as one hoists the pool load above the
 * first __MapActor_GetActor and costs two positions. Repeating the symbol at
 * both sites lets gcc discover r5 itself. Batch 94's standing non-signal.
 */
struct A {
    unsigned char pad00[8];
    int f8;
    unsigned char pad0c[4];
    int f10;
    unsigned char pad14[0x6c - 0x14];
    void *f6c;
};

extern struct A *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __SetFlag(int id);
extern void __MapActor_SetAnim(int slot, int n);
extern void __Func_8092b08(int a, int b);
extern void __Func_8092950(int a, int b);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void OvlFunc_964_2008ec8(void);

void OvlFunc_964_2009a10(void)
{
    unsigned char *p;

    __CutsceneStart();
    __Func_8092b08(9, 1);
    __MapActor_SetAnim(9, 1);
    __Func_8092950(9, 0);
    __MapActor_SetAnim(9, 2);
    p = (unsigned char *)__MapActor_GetActor(9) + 0x23;
    *p &= 0xfd;
    __SetFlag(0x81 << 2);
    __Func_8010704(0x1a, 8, 1, 1,
                   __MapActor_GetActor(9)->f8 >> 20,
                   __MapActor_GetActor(9)->f10 >> 20);
    __MapActor_GetActor(9)->f6c = OvlFunc_964_2008ec8;
    __MapActor_GetActor(8)->f6c = OvlFunc_964_2008ec8;
    __CutsceneEnd();
}
