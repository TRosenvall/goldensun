/* OvlFunc_946_2008f70  [overlays/rom_7ced6c]
 *
 * Source asm: goldensun/asm/overlays/rom_7ced6c/ovl_30_c_c_a_a_c_c_a.s
 *
 * BLOCKER CLASS: register-role flip plus argument-fill ordering. 98 aligned
 * of 284 (287 lines vs 284). A 268-instruction map-entry initialiser; not the
 * dispatcher family.
 *
 * THE BIGGEST SINGLE WIN WAS A SINGLE EXIT. Replacing five `return 0;` with
 * `goto out;` and one exit took it from 152 to 98 -- the six-instruction lever
 * already recorded in src/overlays/rom_7ced6c/ovl_30_c_c_a_a_a_a.c, applying
 * here five times over.
 *
 * THE GLOBAL MUST BE THE ARRAY IDIOM. `extern unsigned char gState[];` with
 * `g = gState;` is what produces the ROM's `mov r2, #0xe0 / lsl r2, #1 /
 * add r3, r6, r2`; a struct pointer lets gcc fold the whole thing into one
 * pool word `=gState+448`. Copied from the already-matched siblings in this
 * overlay.
 *
 * FACTS ESTABLISHED, so nobody re-derives them: the area ids are the linker
 * symbols _AREA_71 / _7b / _7d / _7e / _86 (all in area.sym), which is why the
 * ROM pools 0x7b/0x7d/0x7e where an imm8 would fit, while the flag ids
 * (0xef7, 0x8d1, 0x240-0x243, 0xef4, 0xfd7) are plain integers. The `.L1004`
 * block is a short-circuit `||`. The control flow is decoded and believed
 * correct.
 *
 * WHAT IS LEFT: a consistent r6/r7 flip between the gState base and the area
 * value, which propagates through most of the count; one extra join label that
 * renames every later label and inflates roughly 24 lines of the diff without
 * being 24 real defects; and a repeated ordering difference where the ROM
 * emits `mov r0, #imm` BETWEEN the two `lsl`s of the other arguments, at all
 * four __Func_8012078 sites and both __Func_808edac sites.
 *
 * MEASURED: struct-pointer global 145; array idiom 152; array idiom + single
 * exit 98; moving the `g = gState` assignment later 98; rewriting the `||` as
 * explicit gotos 98.
 */
extern unsigned char gState[];
extern int _AREA_71;
extern int _AREA_7b;
extern int _AREA_7d;
extern int _AREA_7e;
extern int _AREA_86;
extern unsigned char iwram_3001ebc[];
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __Actor_SetAnim(unsigned char *a, int n);
extern void __Actor_SetSpriteFlags(unsigned char *a, int n);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __Func_808edac(int a, int b, int c);
extern void __Func_8012078(int a, int b, int c, int d);
extern void OvlFunc_946_200991c(void);
extern void OvlFunc_946_200967c(void);
extern int OvlFunc_946_20088c0(int slot);
extern void OvlFunc_946_200aed8(int a);
extern void OvlFunc_946_2009214(void);
extern void OvlFunc_946_2009494(void);

int OvlFunc_946_2008f70(void)
{
    unsigned char *g;
    short *e;
    unsigned char *p;
    unsigned char *a;
    unsigned int k;
    int v;
    int t;

    g = gState;
    k = 0xe0 << 1;
    p = *(unsigned char **)iwram_3001ebc;
    *(int *)(p + k) = 0x81 << 2;
    e = (short *)(g + k);
    v = *e;
    if (v == (int)(&_AREA_7b)) {
        OvlFunc_946_200991c();
        goto out;
    }
    if (v == (int)(&_AREA_7d)) {
        if (__GetFlag(0xef7) == 0) {
            __Func_8010704(0, 3, 1, 1, 0xd, 0x28);
            __Func_8010704(0, 2, 1, 1, 0xf, 0x28);
            __Func_808edac(0x65, 0xd8 << 16, 0xa2 << 18);
        }
        if (*e == v) {
            if (*(short *)(g + (0xe1 << 1)) == 5 ||
                __GetFlag(0x8d1) != 0) {
                __SetFlag(0x8d1);
                __Func_8010704(0, 1, 1, 1, 0xd, 0x1e);
                __Func_808edac(0x64, 0xd8 << 16, 0xf4 << 17);
            }
            goto out;
        }
    }
    t = *(short *)(g + (0xe0 << 1));
    if (t == (int)(&_AREA_71)) {
        OvlFunc_946_200967c();
        *(int *)(__MapActor_GetActor(8) + 0x38) = 0x81 << 16;
        OvlFunc_946_20088c0(9);
        OvlFunc_946_20088c0(0xa);
        if (__GetFlag(0x240) != 0) {
            a = __MapActor_GetActor(0xb);
            if (a != 0) {
                *(char *)(a + 0x59) = 0;
                __Actor_SetAnim(a, 4);
                __Actor_SetSpriteFlags(a, 0);
            }
            __Func_8012078(0, 0x98 << 17, 0xb8 << 17, 0xfd);
        }
        if (__GetFlag(0x241) != 0) {
            a = __MapActor_GetActor(0xc);
            if (a != 0) {
                *(char *)(a + 0x59) = 0;
                __Actor_SetAnim(a, 4);
                __Actor_SetSpriteFlags(a, 0);
            }
            __Func_8012078(0, 0xa0 << 15, 0xb8 << 17, 0xfd);
        }
        if (__GetFlag(0x242) != 0) {
            a = __MapActor_GetActor(0xd);
            if (a != 0) {
                *(char *)(a + 0x59) = 0;
                __Actor_SetAnim(a, 4);
                __Actor_SetSpriteFlags(a, 0);
            }
            __Func_8012078(0, 0xc0 << 15, 0xa8 << 17, 0xfd);
        }
        if (__GetFlag(0x243) != 0) {
            a = __MapActor_GetActor(0xe);
            if (a != 0) {
                *(char *)(a + 0x59) = 0;
                __Actor_SetAnim(a, 4);
                __Actor_SetSpriteFlags(a, 0);
            }
            __Func_8012078(0, 0x90 << 16, 0xa0 << 17, 0xfd);
            __Func_8012078(0, 0xbc << 18, 0xa0 << 17, 0xfd);
        }
        if (__GetFlag(0xfd7) == 0)
            OvlFunc_946_200aed8(8);
        goto out;
    }
    if (t == (int)(&_AREA_7e)) {
        if (__GetFlag(0xef4) == 0) {
            __Func_8010704(0, 0, 1, 1, 0x25, 0xa);
            __Func_808edac(0x64, 0x96 << 18, 0xa8 << 16);
        }
    }
    t = *(short *)(g + (0xe0 << 1));
    if (t < (int)(&_AREA_7e))
        goto out;
    if (t > (int)(&_AREA_86))
        goto out;
    OvlFunc_946_2009214();
    if (*(short *)(g + (0xe1 << 1)) == 5)
        OvlFunc_946_2009494();
out:
    return 0;
}
