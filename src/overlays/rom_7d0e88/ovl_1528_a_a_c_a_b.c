/* OvlFunc_947_200975c -- 173 instructions, 476 bytes, 181 encodings and 54
 * relocations identical.  Split out of asm/overlays/rom_7d0e88/ovl_1528_a_a_c_a.s
 * (7 functions); text-only, no data sections.
 *
 * NO asm/ CHANGE AT ALL, AND THE FIRST READING THAT SAID OTHERWISE WAS A GREP ERROR.
 * Its five dot-labels -- .L2da8, .L2dd2, .L2dfc, .L2e26, .L2e50 -- looked like they
 * needed exports, but they are ALREADY `.global` at lines 5-9 of
 * asm/overlays/rom_7d0e88/ovl_1528_c_c_c_c_c_c.s.  The first grep had searched only
 * the reference file.  **When checking whether a dot-label needs exporting, grep the
 * whole of asm/, not the reference** -- these labels are routinely declared in a
 * sibling and defined elsewhere.  The C reaches them with
 * `extern unsigned char L2da8[] __asm__(".L2da8");`, the tree's existing convention.
 *
 * ONE REAL LEVER: `__GetFlag(0x80 << 2)` / `__SetFlag` / `__ClearFlag` commoned 0x200
 * into r6, which WIDENED THE PROLOGUE to `push {r5,r6,lr}`.  Pinning each of the three
 * sites SEPARATELY restored the ROM's fresh `mov/lsl` at each.  A prologue one register
 * too wide is a repeated-constant-CSE symptom, and it is cheaper to read than the
 * register roles it causes.
 *
 * The ascending-source rule for pinned fills (see
 * src/overlays/rom_794ac0/ovl_30_c_a_a_c_b.c) applied at two sites here.
 *
 * No .sym entry.  No per-file Makefile flag override applies to this stem.
 */
/* The five .L labels are data blocks in ovl_1528_c_c_c_c_c_c.s and are ALREADY
 * exported there (.global .L2da8/.L2dd2/.L2dfc/.L2e26/.L2e50, lines 5-9), so no
 * linker or export change is needed -- only a text split of the 7-function stem.
 * The __asm__ alias spelling is the tree's convention (rom_77a7c8/ovl_30_c_a_c_a_c_a_b.c). */
extern unsigned char L2da8[] __asm__(".L2da8");
extern unsigned char L2dd2[] __asm__(".L2dd2");
extern unsigned char L2dfc[] __asm__(".L2dfc");
extern unsigned char L2e26[] __asm__(".L2e26");
extern unsigned char L2e50[] __asm__(".L2e50");

extern unsigned char *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __PlaySound(int id);
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __ClearFlag(int id);
extern void __MapActor_SetPos(int slot, int x, int z);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern void __Func_8010560(unsigned char *p, int a, int b);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __Func_801776c(int a, int b);
extern void __Func_8092b08(int a, int b);
extern void __Func_80933d4(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_8093530(void);
extern void OvlFunc_947_20095fc(void);
extern void OvlFunc_947_2008ec8(int a);
extern void OvlFunc_947_2008f58(int a);

#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")
#define PIN4 PIN3; register int q3 __asm__("r3")

void OvlFunc_947_200975c(void)
{
    unsigned char *p;
    int f;
    int s1;
    int s2;

    __CutsceneStart();
    { PIN2; q0 = 0x80; q1 = 0x80; q0 <<= 9; q1 <<= 6; __Func_80933d4(q0, q1); }
    { PIN4; q0 = 0x1190000; q1 = 0x1; q1 = -q1; q2 = 0xd8; q2 <<= 17; q3 = 0x1; __Func_80933f8(q0, q1, q2, q3); }
    __Func_8093530();
    __Func_801776c(0x1528, 1);
    { PIN1; q0 = 0x80; q0 <<= 2; f = __GetFlag(q0); }
    if (!f) {
        __PlaySound(0xe8);
        { PIN3; q2 = 0x18; q1 = 0x54; q0 = (int)L2da8; __Func_8010560((unsigned char *)q0, q1, q2); }
        __CutsceneWait(0x1e);
        __PlaySound(0xf0);
        { PIN2; q1 = 0x1; q0 = 0x10; __Func_8092b08(q0, q1); }
        p = __MapActor_GetActor(0x10) + 0x55;
        *p = f;
        p = __MapActor_GetActor(0x10);
        *(int *)(p + 0xc) = 0xffe00000;
        { PIN3; q1 = 0x88; q2 = 0xd0; q0 = 0x10; q1 <<= 17; q2 <<= 17; __MapActor_SetPos(q0, q1, q2); }
        __MapActor_SetAnim(0x10, 1);
        { PIN3; q0 = (int)L2dfc; q1 = 0x50; q2 = 0x18; __Func_8010560((unsigned char *)q0, q1, q2); }
        { PIN3; q0 = (int)L2e50; q1 = 0x50; q2 = 0x1c; __Func_8010560((unsigned char *)q0, q1, q2); }
        s1 = 2;
        s2 = 4;
        __CopyMapTiles(0x41, 0x28, 0x10, 0x1b, s1, s2);
        OvlFunc_947_20095fc();
        OvlFunc_947_2008ec8(9);
        OvlFunc_947_2008ec8(0xa);
        OvlFunc_947_2008ec8(0xb);
        OvlFunc_947_2008ec8(0xc);
        OvlFunc_947_2008ec8(0xd);
        OvlFunc_947_2008ec8(0xe);
        OvlFunc_947_2008ec8(0xf);
        s1 = 0x18;
        s2 = 8;
        __Func_8010704(0x18, 3, 1, 1, s1, s2);
        { PIN1; q0 = 0x80; q0 <<= 2; __SetFlag(q0); }
    } else {
        __PlaySound(0xe8);
        { PIN3; q1 = 0x54; q2 = 0x18; q0 = (int)L2dd2; __Func_8010560((unsigned char *)q0, q1, q2); }
        __CutsceneWait(0x1e);
        __PlaySound(0xe6);
        p = __MapActor_GetActor(0x10) + 0x55;
        *p = 0;
        p = __MapActor_GetActor(0x10);
        *(int *)(p + 0xc) = 0xffe00000;
        { PIN3; q1 = 0x88; q2 = 0xda; q0 = 0x10; q1 <<= 17; q2 <<= 17; __MapActor_SetPos(q0, q1, q2); }
        __MapActor_SetAnim(0x10, 2);
        s1 = 2;
        s2 = 4;
        __CopyMapTiles(0x41, 0x2d, 0x10, 0x1b, s1, s2);
        { PIN3; q1 = 0x50; q2 = 0x18; q0 = (int)L2e26; __Func_8010560((unsigned char *)q0, q1, q2); }
        OvlFunc_947_20095fc();
        OvlFunc_947_2008f58(9);
        OvlFunc_947_2008f58(0xa);
        OvlFunc_947_2008f58(0xb);
        OvlFunc_947_2008f58(0xc);
        OvlFunc_947_2008f58(0xd);
        OvlFunc_947_2008f58(0xe);
        OvlFunc_947_2008f58(0xf);
        s1 = 0x18;
        s2 = 8;
        __Func_8010704(0x18, 4, 1, 1, s1, s2);
        { PIN1; q0 = 0x80; q0 <<= 2; __ClearFlag(q0); }
    }
    __CutsceneEnd();
}
