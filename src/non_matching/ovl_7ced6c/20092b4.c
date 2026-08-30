/*
 * OvlFunc_946_20092b4 -- asm/overlays/rom_7ced6c/ovl_30_c_c_a_a_c_c_c.s
 *
 * BLOCKER: register roles in a read-modify-write. 79 lines against 79, TWO
 * differing:
 *
 *      rom   ldrb r2, [r1] / mov r3, #0x2 / orr r3, r2
 *      ours  ldrb r3, [r1] / mov r2, #0x2 / orr ...
 *
 * The ROM loads the byte into r2 and materialises the constant into r3; we do
 * the reverse. Everything else matches.
 *
 * TRIED AND REJECTED, both byte-identical to the version below:
 *
 *   * `e[0x23] = 2 | e[0x23];` -- gcc canonicalises the operand order.
 *   * Naming the address in a pointer local (`q = e + 0x23; *q |= 2;`).
 *
 * SETTLED, and these two are why the function got from unmatched to 2:
 *
 *   1. A POOLED SMALL CONSTANT THAT CANNOT BE FOLDED MEANS A LINKER SYMBOL.
 *      The ROM emits `ldr r3, =0x7e / ldr r2, =0x8d2 / sub r2, r3`, computing
 *      0x854 at RUNTIME from two pooled values. 0x7e fits an eight-bit `mov`,
 *      so pooling it is the symbol tell from area.sym's header; and gcc would
 *      have folded the subtraction of two literals at compile time, so it
 *      cannot be two literals. The source is
 *      `*g + 0x8d2 - (int)&_AREA_7e`, using the existing area.sym entry.
 *
 *   2. DO NOT NAME THE SYMBOL DIFFERENCE. Assigning it to `int base` first --
 *      the obvious readable spelling, and the one that reflects what the value
 *      means -- emits its two pooled loads and the subtraction BEFORE the
 *      gState address block, where the ROM emits them after. That is 14
 *      differing. Writing the whole expression inline at both call sites and
 *      letting gcc common it into r10 itself gives 2.
 *
 *      That is a fourth case for the naming lever: a value built from pooled
 *      symbols gets its block HOISTED when named, because the pool loads
 *      become the definition of a named pseudo rather than part of the
 *      expression they appear in.
 *
 *   3. The gState offset must be built at runtime through a local pointer, or
 *      gcc folds it into a single `ldr =gState+448`. Same lever as
 *      Func_808b25c and Func_809b5dc; third instance.
 */
extern unsigned char gState[];
extern int _AREA_7e;
extern unsigned char *__MapActor_GetActor(int slot);
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __CutsceneWait(int n);
extern void OvlFunc_946_2008e00(int n);
extern void __PlaySound(int id);
extern void __Actor_SetSpriteFlags(unsigned char *a, int n);
extern void __Func_8092b08(int a, int b);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

void OvlFunc_946_20092b4(void)
{
    unsigned char *e;
    unsigned char *f;
    unsigned char *gp;
    int d;
    int r;
    int s2;

    e = __MapActor_GetActor(8);
    d = *(int *)(e + 8) >> 20;
    if (d == 0x28) {
        gp = gState;
        r = __GetFlag(*(short *)(gp + (0xe0 << 1)) + 0x8d2 - (int)&_AREA_7e);
        if (r == 0) {
            f = e + 0x55;
            *f = 3;
            __CutsceneWait(8);
            OvlFunc_946_2008e00(8);
            __PlaySound(0x88);
            __CutsceneWait(0x28);
            __Actor_SetSpriteFlags(__MapActor_GetActor(8), 0);
            __Func_8092b08(8, 3);
            *f = r;
            e[0x23] |= 2;
            s2 = 0xa;
            __Func_8010704(0x2a, 0xa, 1, 1, d, s2);
            __SetFlag(*(short *)(gp + (0xe0 << 1)) + 0x8d2 - (int)&_AREA_7e);
        }
    }
}
