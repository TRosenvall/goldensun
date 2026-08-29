/* OvlFunc_920_2008214  --  0x02008214, cut from the tail of
 * goldensun/asm/overlays/rom_7a6ae4/ovl_30_c_a_c_c_a_a.s.
 *
 * Preserves the original ROM layout when slotted after
 * asm/overlays/rom_7a6ae4/ovl_30_c_a_c_c_a_a.o in
 * goldensun/overlays/rom_7a6ae4/overlay.ld.
 *
 * RevealSlotF: teleport slot 8 to the origin, set a save bit, and after a
 * forty-frame beat bring slot 0xF into view -- animation 2, its flag byte at
 * +0x55 cleared, bit 1 set at +0x23, draw priority 2, and one attribute cell
 * repainted.
 *
 * TWO NAMED LOCALS ARE LOAD-BEARING, and both are the ADD direction of the
 * lever batch 82 recorded in the other direction.
 *
 *   THE TWO STACK ARGUMENTS. `__Func_8010704(0, 0, 1, 1, 0x12, 0xe)` with
 *   literals gives `mov r3, #0x12 / str r3, [sp] / mov r3, #0xe / str r3,
 *   [sp, #4]` -- gcc computes and stores each in turn, reusing r3. The ROM
 *   builds BOTH first: `mov r3, #0x12 / mov r2, #0xe / str r3 / str r2`.
 *   Naming them as locals keeps two pseudos alive and reproduces it. Three of
 *   the five differences were this.
 *
 *   THE OR'S CONSTANT, AND ITS WIDTH. The ROM has `ldrb r2, [r0] / mov r3, #2 /
 *   orr r3, r2` -- the CONSTANT is the destination and the loaded byte the
 *   source. `*p |= 2` gives the opposite, and so do `*p = 2 | *p`, naming the
 *   loaded value, and naming the constant as an `int`. Naming it as an
 *   `unsigned char` -- the width of what it is ORed into -- and writing it
 *   first is what puts the operands the ROM's way round. That is the same
 *   question the pooled `_CONST_2` in src/overlays/rom_793768/ answers for a
 *   halfword field, and here, with the constant small enough for a `mov`, the
 *   matching-width local is enough on its own.
 *
 * Both `+0x55` and `+0x23` are past the `strb` immediate range, so the address
 * is materialised either way; taking it into a local is what keeps the actor
 * pointer from surviving the store.
 */
extern void *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __MapActor_SetPos(int slot, int x, int z);
extern void __MapActor_DoAnim(int slot, int a);
extern void __SetFlag(int id);
extern void __Func_8092b08(int slot, int n);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

void OvlFunc_920_2008214(void)
{
    unsigned char *p;
    unsigned char two;
    int e5, e6;

    __CutsceneStart();
    __MapActor_SetPos(8, 0, 0);
    __SetFlag(0x883);
    __CutsceneWait(0x28);
    __MapActor_DoAnim(0xf, 2);
    p = (unsigned char *)__MapActor_GetActor(0xf) + 0x55;
    *p = 0;
    p = (unsigned char *)__MapActor_GetActor(0xf) + 0x23;
    two = 2;
    *p = two | *p;
    __Func_8092b08(0xf, 2);
    e5 = 0x12;
    e6 = 0xe;
    __Func_8010704(0, 0, 1, 1, e5, e6);
    __CutsceneEnd();
}
