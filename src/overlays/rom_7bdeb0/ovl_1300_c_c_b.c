/* Cluster OvlFunc_934_20095cc..OvlFunc_934_20095cc extracted from goldensun/asm/overlays/rom_7bdeb0/ovl_1300_c_c.s.
 *
 * Total .text for this TU = 208 bytes (= 0x00d0).
 * Preserves the original ROM layout when slotted between
 * asm/overlays/rom_7bdeb0/ovl_1300_c_c_a.o and asm/overlays/rom_7bdeb0/ovl_1300_c_c_c.o
 * in goldensun/overlays/rom_7bdeb0/overlay.ld.  The target was the LAST function
 * and the .s carries a trailing .data, so the split is genuinely three-way: the
 * data is isolated in _c and this TU is pure text.
 *
 * A SIX-WORD STRUCT PASSED BY VALUE.  OvlFunc_934_2008758 fills a local through
 * a pointer, and OvlFunc_934_20088ec then takes it BY VALUE -- which is what the
 * ROM's `ldmia r3!, {r0, r1} / stmia r2!, {r0, r1}` is: four words in r0-r3 and
 * the two-word tail block-copied into the outgoing argument area.  Declared
 * `void f(struct S s)` it reproduces with no help; see the by-value note in
 * docs/elevation.md.
 *
 * THREE SPELLINGS ARE LOAD-BEARING:
 *   q1, q2   the two shifted __MapActor_SetSpeed arguments, named before the
 *            `if` so the ROM's `mov r0, #0xb` lands between the movs and the
 *            shifts.  46 differing -> 43, and the first difference moved from
 *            line 22 to 44.
 *   two      the constant 2 is SHARED between the byte store at +0x23 and the
 *            first spilled argument.  The ROM reuses the register; passing a
 *            literal makes gcc rematerialise it.
 *   zero     the second spilled argument, named so the pair gets two registers
 *            rather than one reused -- the two-stack-arguments rule.
 * With all three, exact; `two` and `zero` together were the last 43 lines.
 */
struct S { int a, b, c, d, e, f; };

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __PlaySound(int id);
extern void __SetFlag(int id);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __Func_809228c(int a, int b, int c);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern int OvlFunc_934_2008758(struct S *s);
extern void OvlFunc_934_20088ec(struct S s);
extern void OvlFunc_934_2008528(int a, int b, int c, int d, int e, int f);

void OvlFunc_934_20095cc(void)
{
    struct S s;
    unsigned char *p;
    int k;
    int q1, q2;
    int two, zero;

    q1 = 0x80 << 7;
    q2 = 0x80 << 8;
    __CutsceneStart();
    if (OvlFunc_934_2008758(&s)) {
        OvlFunc_934_20088ec(s);
        __MapActor_SetAnim(0xb, 3);
        __MapActor_SetSpeed(0xb, q1, q2);
        __Func_809228c(0xb, 0, -0x10);
        __CutsceneWait(0x2d);
        __PlaySound(0xf0);
        __MapActor_SetAnim(0xb, 8);
        p = __MapActor_GetActor(0xb);
        two = 2;
        p[0x23] = two;
        zero = 0;
        OvlFunc_934_2008528(0, 0xd, (s.e >> 20) - 1, 4, two, zero);
        if ((s.e >> 20) == 0x14) {
            __SetFlag(0x205);
        } else {
            __SetFlag(0x81 << 2);
            k = 0xe;
            __Func_8010704(0xe, 0x11, 2, 1, k, 0x10);
            __Func_8010704(0xe, 0xd, 1, 1, k, 0xf);
        }
    }
    __CutsceneEnd();
}
