/* OvlFunc_968_2009644 -- NOT MATCHING. 4 of 39, same length.
 *
 * Source asm: goldensun/asm/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_c_a.s
 *
 * Two defects, both scheduling:
 *
 *   `sub sp, #0x8` lands BEFORE the first argument in ours and AFTER it in the
 *   ROM (`mov r0,#0xd / sub sp,#8 / bl`). That is frame setup, not something
 *   the source places.
 *
 *   the __SetFlag argument's `mov r0,#0x80` and `lsl r0,#2` STRADDLE the two
 *   stores to +0x14 and +0xc in the ROM; gcc emits the pair together after
 *   them.
 *
 * THE BASIC-BLOCK LEVER WAS TRIED AND DOES NOTHING HERE -- and per batch 57 the
 * plain form was screened FIRST, so this is a genuine test rather than a
 * preemptive application. Naming `f = 0x80 << 2;` above the `if` leaves the
 * count at 4 of 39 exactly.
 *
 * That is worth recording: the lever moves a constant gcc REFUSES to
 * rematerialise at the call site. Here gcc already rematerialises it and the
 * ROM merely INTERLEAVES it with unrelated stores, which is a different
 * problem the lever does not address.
 *
 * The 0xfffe0000 stored to both +0x14 and +0xc is one local -- the ROM holds it
 * in r3 for both stores.
 */
extern void *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __PlaySound(int id);
extern void __SetFlag(int id);
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);

void OvlFunc_968_2009644(void)
{
    unsigned char *a;
    int v;
    int m;
    int n;

    a = (unsigned char *)__MapActor_GetActor(0xd);
    __CutsceneStart();
    if ((*(int *)(a + 8) >> 20) == 0x2a) {
        __CutsceneWait(0x1e);
        __PlaySound(0xbc);
        a[0x55] = 0;
        v = 0xfffe0000;
        *(int *)(a + 0x14) = v;
        *(int *)(a + 0xc) = v;
        __SetFlag(0x80 << 2);
        m = 3;
        n = 5;
        __CopyMapTiles(0x2c, 0x75, 0x29, 0x75, m, n);
    }
    __CutsceneEnd();
}
