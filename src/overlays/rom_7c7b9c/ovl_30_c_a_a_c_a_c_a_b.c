/* Cluster OvlFunc_943_2008c28..OvlFunc_943_2008c28 extracted from goldensun/asm/overlays/rom_7c7b9c/ovl_30_c_a_a_c_a_c_a.s.
 *
 * Preserves the original ROM layout when slotted between
 * asm/overlays/rom_7c7b9c/ovl_30_c_a_a_c_a_c_a_a.o and the rest of the overlay
 * in goldensun/overlays/rom_7c7b9c/overlay.ld.
 *
 * Dispatches on an interaction halfword, then either moves the player or plays
 * a refusal sound.
 *
 * THE LEVER WIDENS: it is not only about SHIFTED constants. The third argument
 * to __Func_8092208 is -0xa, which gcc materialises as a two-instruction
 * `mov r2,#0xa / neg r2,r2` pair -- and the ROM splits that pair around the
 * other two arguments exactly as it splits a mov/lsl pair:
 *
 *     rom    mov r2,#0xa / mov r0,#0 / mov r1,#1 / neg r2,r2
 *     ours   mov r2,#0xa / neg r2,r2 / mov r0,#0 / mov r1,#1
 *
 * Assigning `n = -0xa;` at the top of the function -- a different basic block
 * from the call, which is inside the `if` -- fixes it, the same way it fixes a
 * shifted constant. So the rule is about ANY two-instruction materialisation,
 * not about shifts. See reports/arg-interleave.md.
 *
 * THE DISPATCH IS A `switch`, NOT AN if/else CHAIN. The ROM tests both cases up
 * front and branches away from each (`cmp #1 / beq / cmp #3 / beq / b`); an
 * if/else chain gives `cmp / bne` and comes out sixteen positions different.
 * Same discriminator as OvlFunc_881_200b448's unsigned selector: the shape of
 * the chain reports what the source was.
 *
 * The gState-style offset uses the NON-destructive `add r3, r6, r2`, so it is
 * written as one expression rather than as a walked pointer -- the opposite of
 * src/overlays/rom_7c460c/ovl_314_a_c_a_c_c_c_c_c.c, where the ROM walks.
 */
extern unsigned int iwram_3001ebc;
extern void __CutsceneStart(void);
extern void OvlFunc_943_2008bb8(void);
extern void OvlFunc_943_2008bf0(void);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __Func_8092208(int a, int b, int c);
extern void __CutsceneWait(int n);
extern void __PlaySound(int id);
extern void __Func_8091e9c(int a);

void OvlFunc_943_2008c28(void)
{
    unsigned char *base;
    short v;
    int flag;
    int a;
    int b;
    int n;

    n = -0xa;
    a = 0x9999;
    b = 0x4ccc;
    base = (unsigned char *)iwram_3001ebc;
    __CutsceneStart();
    v = *(short *)(base + (0xb6 << 1));
    flag = 0;
    switch (v) {
    case 1:
        flag = 1;
        OvlFunc_943_2008bb8();
        break;
    case 3:
        flag = 1;
        OvlFunc_943_2008bf0();
        break;
    }
    if (flag) {
        __MapActor_SetSpeed(0, a, b);
        __Func_8092208(0, 1, n);
        __CutsceneWait(0xa);
    } else {
        __PlaySound(0x7b);
    }
    __Func_8091e9c(*(short *)(base + (0xb6 << 1)));
}
